#include "nuimanager.h"
#include "ShareNUI.h"
#include "Settings.h"

#define SAMPLE_XML_PATH "./Sample-User.xml"
#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {																\
                printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return rc;													\
        }
#define CHECK_ERRORS(rc, errors, what)		\
        if (rc == XN_STATUS_NO_NODE_PRESENT)	\
{										\
        XnChar strError[1024];				\
        errors.ToString(strError, 1024);	\
        printf("%s\n", strError);			\
        return (rc);						\
}

xn::Context NuiManager::g_Context;
xn::ScriptNode NuiManager::g_ScriptNode;
xn::DepthGenerator NuiManager::g_DepthGenerator;
xn::UserGenerator NuiManager::g_UserGenerator;
xn::Recorder* NuiManager::g_pRecorder;

XnUserID NuiManager::g_nPlayer;
XnBool NuiManager::g_bCalibrated;
XnBool NuiManager::g_bPause;
NuiManager* NuiManager::me;


NuiManager::NuiManager(Settings* p_settings, ShareNUI* p_share_nui)
{
    g_nPlayer = 0;
    g_bCalibrated = false;

    g_bPause = false;

    share_nui = p_share_nui;
    settings = p_settings;

    init_device();
}
NuiManager::~NuiManager()
{

}
void NuiManager::run()
{
    while (me->share_nui->should_run)
    {
        // Read next available data
        g_Context.WaitOneUpdateAll(g_UserGenerator);

        if (g_nPlayer != 0)
        {
            // TODO:获取玩家数量
        	share_nui->any_player = 1; // 设定为已有玩家
            // 获取用户躯干
            XnSkeletonJointPosition joint_torso;
            g_UserGenerator.GetSkeletonCap()
                    .GetSkeletonJointPosition(g_nPlayer, XN_SKEL_TORSO, joint_torso);

            //sm->state_machine->player_x = joint_torso.position.X * 0.6 + sm->width() / 2  ;
            //printf("player_x : %lf", sm->state_machine->player_x);

            // 获取用户左右手的位置
            XnSkeletonJointPosition joint_left, joint_right;
            g_UserGenerator.GetSkeletonCap()
                .GetSkeletonJointPosition(g_nPlayer, XN_SKEL_LEFT_HAND, joint_left);
            g_UserGenerator.GetSkeletonCap()
                .GetSkeletonJointPosition(g_nPlayer, XN_SKEL_RIGHT_HAND, joint_right);
			//l_speed_x = joint_left.position.X - last_left_hand.X;
            // 添加到历史队列
            left_hand_history.push_back(joint_left.position);
            right_hand_history.push_back(joint_right.position);
            torso_history.push_back(joint_torso.position);
            // 如果队列超出大小， 从头弹出
            if (left_hand_history.size()>HAND_HIS_SIZE)
            {
            	left_hand_history.pop_front();
            }
            if (right_hand_history.size()>HAND_HIS_SIZE)
			{
            	right_hand_history.pop_front();
			}
            if (torso_history.size()>HAND_HIS_SIZE+20)
            {
            	torso_history.pop_front();
            }
            XnVector3D ljoint_ave = {0, 0, 0};
            XnVector3D rjoint_ave = {0, 0, 0};
            XnVector3D torso_ave = {0, 0, 0};

            for (deque<XnVector3D>::iterator step = left_hand_history.begin();
            		step != left_hand_history.end(); step++)
            {
            	ljoint_ave.X += step->X;
            	ljoint_ave.Y += step->Y;
            }
            ljoint_ave.X /= HAND_HIS_SIZE;
            ljoint_ave.Y /= HAND_HIS_SIZE;
            for (deque<XnVector3D>::iterator step = right_hand_history.begin();
					step != right_hand_history.end(); step++)
			{
				rjoint_ave.X += step->X;
				rjoint_ave.Y += step->Y;
			}
			rjoint_ave.X /= HAND_HIS_SIZE;
			rjoint_ave.Y /= HAND_HIS_SIZE;
			for (deque<XnVector3D>::iterator step = torso_history.begin();
					step != torso_history.end(); step++)
			{
				torso_ave.X += step->X;
				torso_ave.Y += step->Y;
			}
			torso_ave.X /= HAND_HIS_SIZE+20;
			torso_ave.Y /= HAND_HIS_SIZE+20;


            // 左手
            share_nui->hands_poses[0].X = (-ljoint_ave.X+torso_ave.X)/80;
            share_nui->hands_poses[0].Y = (ljoint_ave.Y-torso_ave.Y - 30)/80;
            // 右手
            share_nui->hands_poses[1].X = (rjoint_ave.X-torso_ave.X)/80;
            share_nui->hands_poses[1].Y = (rjoint_ave.Y-torso_ave.Y - 30)/80;
        }else
        {
        }
    }
}
void NuiManager::init(NuiManager* p_me)
{
	me = p_me;
	init_device();
	me->run();
}
int NuiManager::init_device()
{
    XnStatus rc = XN_STATUS_OK;
    xn::EnumerationErrors errors;

    rc = g_Context.InitFromXmlFile(SAMPLE_XML_PATH, g_ScriptNode, &errors);
    CHECK_ERRORS(rc, errors, "InitFromXmlFile");
    CHECK_RC(rc, "InitFromXml");

    rc = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(rc, "Find depth generator");
    rc = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    CHECK_RC(rc, "Find user generator");

    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON) ||
            !g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION))
    {
            printf("User generator doesn't support either skeleton or pose detection.\n");
            return XN_STATUS_ERROR;
    }

    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    rc = g_Context.StartGeneratingAll();
    CHECK_RC(rc, "StartGenerating");

    XnCallbackHandle hUserCBs, hCalibrationStartCB, hCalibrationCompleteCB, hPoseCBs;
    g_UserGenerator.RegisterUserCallbacks(NewUser, LostUser, NULL, hUserCBs);
    rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(CalibrationStarted, NULL, hCalibrationStartCB);
    CHECK_RC(rc, "Register to calbiration start");
    rc = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(CalibrationCompleted, NULL, hCalibrationCompleteCB);
    CHECK_RC(rc, "Register to calibration complete");
    rc = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(PoseDetected, NULL, hPoseCBs);
    CHECK_RC(rc, "Register to pose detected");

    return 0;
}

XnBool NuiManager::AssignPlayer(XnUserID user)
{
    return true;
}

void NuiManager::NewUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    printf("*********NewUser**********\nnew user id : %d \n now users:", user);
    g_UserGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", user);
    XnUserID aUsers[20];
    XnUInt16 nUsers = 20;
    g_UserGenerator.GetUsers(aUsers, nUsers);

    for (int i = 0; i < nUsers; ++i)
    {
        printf(" %d ", aUsers[i]);
    }
    printf("\n********************\n");
}
void NuiManager::FindPlayer()
{
}
void NuiManager::LostPlayer()
{
}
void NuiManager::LostUser(xn::UserGenerator& generator, XnUserID user, void* pCookie)
{
    printf("*********LostPlayer**********\nlost user id : %d \n now users:", user);
    XnUserID aUsers[20];
    XnUInt16 nUsers = 20;
    g_UserGenerator.GetUsers(aUsers, nUsers);

    for (int i = 0; i < nUsers; ++i)
    {
        printf(" %d ", aUsers[i]);
    }
    printf("\n********************\n");
}
void NuiManager::PoseDetected(xn::PoseDetectionCapability& pose,
    const XnChar* strPose, XnUserID user, void* cxt)
{
    printf("*********PoseDetected**********\n");
    printf("Found pose \"%s\" for user %d\n", strPose, user);
    g_UserGenerator.GetSkeletonCap().RequestCalibration(user, true);
    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(user);
    printf("*********************\n");
}

void NuiManager::CalibrationStarted(xn::SkeletonCapability& skeleton,
    XnUserID user, void* cxt)
{
    printf("*********CalibrationStarted**********\n");
    printf("Calibration started for user: %d\n", user);
    printf("******************************\n");
}

void NuiManager::CalibrationCompleted(xn::SkeletonCapability& skeleton,
    XnUserID user, XnCalibrationStatus eStatus, void* cxt)
{
    printf("*********CalibrationCompleted*********\n");
    printf("Calibration done for user [%d] %ssuccessfully\n", user, (eStatus == XN_CALIBRATION_STATUS_OK)?"":"un");
    if (eStatus == XN_CALIBRATION_STATUS_OK)
    {
    	//g_UserGenerator.GetSkeletonCap().SaveCalibrationData(user, 0);
        g_UserGenerator.GetSkeletonCap().StartTracking(user);
        printf("users now: ");
        XnUserID aUsers[20];
        XnUInt16 nUsers = 20;
        g_UserGenerator.GetUsers(aUsers, nUsers);
        for (int i = 0; i < nUsers; ++i)
        {
            printf(" %d ", aUsers[i]);
        }
        g_nPlayer = user;


    }else
    {
        printf("Request Calibration on user %d again\n", user);
        g_UserGenerator.GetSkeletonCap().RequestCalibration(user, false);
    }
    printf("\n*************************\n");
}

