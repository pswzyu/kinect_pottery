/*
 * DataProcesser.cpp
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#include "DataProcesser.h"
#include "ShareGL.h"
#include "ShareNUI.h"
#include <thread/xtime.hpp>
#include <thread/thread.hpp>
#include "Settings.h"
#include "def.h"
#include <math.h>

DataProcesser::DataProcesser()
{

}

DataProcesser::DataProcesser(Settings* p_settings, ShareGL* p_share_gl, ShareNUI* p_share_nui) {
	settings = p_settings;
	share_gl = p_share_gl;
	share_nui = p_share_nui;
	check_point = 0;

}
void DataProcesser::init(DataProcesser* me)
{
	while(me->share_nui->should_run)
	{
		me->updateData();
		boost::xtime xt;
		boost::xtime_get(&xt, boost::TIME_UTC);
		xt.nsec += 10000000;

		boost::thread::sleep(xt);
	}
}

DataProcesser::~DataProcesser() {
	// TODO Auto-generated destructor stub
}

void DataProcesser::makePieces()
{

}
void DataProcesser::getPointsInHands()
{

}
void DataProcesser::checkNormalOrientation()
{

}
XnVector3D getPointMove(int point_pos, XnVector3D direction, float move)
{

}
void DataProcesser::updateData()
{
	// 因为share_gl中的rotation是持续增长的， 因此要做个求余
	int abs_rotation = (share_gl->rotation) % (settings->pieces_in_pottery);

	//检查手有没有触发按钮
	checkButtonAndAction();
	// 先检测右手
	Piece* this_piece = &(share_gl->pieces[abs_rotation]);
	doModify(this_piece, 1);
	// 再检左手
	abs_rotation = (abs_rotation + settings->pieces_in_pottery/2)%
			(settings->pieces_in_pottery);
	this_piece = &(share_gl->pieces[abs_rotation]);
	doModify(this_piece, 0);

	//doFixPiece();

	// 旋转
	share_gl->rotation += 1;
}
void DataProcesser::checkButtonAndAction()
{
    // 检查手的位置并且如果手在按钮上， 则将按钮的状态改变， 并改变操作模式
    // 添加还是削去
    int add_button_x = 5, add_button_y = 6, scrape_button_x = 5, scrape_button_y = 4;
    for (int step = 0; step != 2; ++ step)
    {
        // 手在添加按钮肿
        if (share_nui->hands_poses[step].X < add_button_x+1.3 &&
             share_nui->hands_poses[step].X > add_button_x &&
             share_nui->hands_poses[step].Y < add_button_y+1.3 &&
             share_nui->hands_poses[step].Y > add_button_y )
        {
            share_gl->action_now = share_gl->ADD;
        }
        // 手在削去按钮肿
        if (share_nui->hands_poses[step].X < scrape_button_x+1.3 &&
             share_nui->hands_poses[step].X > scrape_button_x &&
             share_nui->hands_poses[step].Y < scrape_button_y+1.3 &&
             share_nui->hands_poses[step].Y > scrape_button_y )
        {
            share_gl->action_now = share_gl->SCRAPE;
        }

    }
}
void DataProcesser::doModify(Piece* this_piece, int hand_index)
{
	// 当rotation 为n的时候， 手可能和 piece[n] 这一片相碰
	// 检测手和点的碰撞
	XnVector3D hand_point_vector;
	hand_point_vector.Z = 0;
	// 如果list中删除了元素， 则指针for时不能加一
	bool has_del_points = false;
	KPPoint temp_point; // 如需添加点
	for (list<KPPoint>::iterator point_iter = ++(this_piece->kppoints.begin());
			point_iter != this_piece->kppoints.end();   )
	{
	    has_del_points = false;
		XnFloat distant = (point_iter->position.X-share_nui->hands_poses[hand_index].X)*
				(point_iter->position.X-share_nui->hands_poses[hand_index].X)+
				(point_iter->position.Y-share_nui->hands_poses[hand_index].Y)*
				(point_iter->position.Y-share_nui->hands_poses[hand_index].Y);
		// 如果手的点离片上的点比较近， 则碰撞
		// TODO： 不能穿透
		//printf("%f\n", distant);
		if (distant < HAND_THRESHOLD)
		{
		    // 如果当前操作是add
		    if (share_gl->action_now == share_gl->ADD)
		    {
		        point_iter->position.X += 0.05;
		        if (point_iter->position.X > 6)
		        {
		            point_iter->position.X = 6;
		        }
		    }else
		    {
		        point_iter->position.X -= 0.05;
		        if (point_iter->position.X < 0)
		        {
		            point_iter->position.X = 0;
		        }
		    }

		    //修正点
		    boost::unique_lock<boost::shared_mutex> lock1(
                                                *(this_piece->piece_mutex));
		    // 如果点之间距离太小
		    list<KPPoint>::iterator last_point = --point_iter; ++point_iter;
		    double dist_to_last_point_2  = pow(point_iter->position.X - last_point->position.X, 2) +
                pow(point_iter->position.Y - last_point->position.Y, 2);

            printf("%lf\n", dist_to_last_point_2);
		    if (dist_to_last_point_2 <0.02)
		    {
		        point_iter = this_piece->kppoints.erase(point_iter);
		        has_del_points = true;
		        printf("DEL\n");
		    }
		    if  (dist_to_last_point_2 > 0.05)
		    {
                temp_point.position.X = (point_iter->position.X + last_point->position.X)/2;
                temp_point.position.Y = (point_iter->position.Y + last_point->position.Y)/2;
                this_piece->kppoints.insert(point_iter, temp_point);
                printf("ADD\n");
		    }
		    lock1.~unique_lock();

		}
		if (!has_del_points)
		{
		    point_iter++;
		}
	}
}

void DataProcesser::doFixPiece()
{

}
