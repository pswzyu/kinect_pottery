#ifndef NUIMANAGER_H
#define NUIMANAGER_H

#include "manager.h"


#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <deque>
#include "def.h"

using namespace std;

class ShareNUI;
class Settings;

class NuiManager : public Manager
{
public:
    ShareNUI* share_nui;

    static xn::Context g_Context;
    static xn::ScriptNode g_ScriptNode;
    static xn::DepthGenerator g_DepthGenerator;
    static xn::UserGenerator g_UserGenerator;
    static xn::Recorder* g_pRecorder;

    static XnUserID g_nPlayer;
    static XnBool g_bCalibrated;
    static XnBool g_bPause;
    bool should_run;
    Settings* settings;
    static NuiManager* me;
    deque<XnVector3D> left_hand_history;
    deque<XnVector3D> right_hand_history;
    deque<XnVector3D> torso_history;

    NuiManager();
    NuiManager(Settings* p_settings, ShareNUI* p_share_nui);
    ~NuiManager();
    void run();
    static void init(NuiManager* p_me);
    static int init_device();
    static int writeToShareNUI();

    static XnBool AssignPlayer(XnUserID);
    static void NewUser(xn::UserGenerator&, XnUserID, void*);
    static void FindPlayer();
    static void LostPlayer();
    static void LostUser(xn::UserGenerator&, XnUserID, void*);
    static void PoseDetected(xn::PoseDetectionCapability&, const XnChar*, XnUserID, void*);
    static void CalibrationStarted(xn::SkeletonCapability&, XnUserID, void*);
    //static void CalibrationEnded(xn::SkeletonCapability&, XnUserID, XnBool, void*);
    static void CalibrationCompleted(xn::SkeletonCapability&, XnUserID, XnCalibrationStatus, void*);
};

#endif // NUIMANAGER_H
