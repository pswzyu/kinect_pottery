/*
 * ShareNUI.cpp
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#include <math.h>
#include "ShareNUI.h"
#include "Settings.h"
#include "def.h"

ShareNUI::ShareNUI(Settings* p_settings)
	: hands_poses(vector<XnVector3D>(2))
{
	settings = p_settings;
	should_run = true;
	nui_terminated = false;

	hands_poses[0].X = settings->pottery_width+sqrt(HAND_THRESHOLD)+0.1;
	hands_poses[0].Y = settings->pottery_width;
	hands_poses[0].Z = 0;
	hands_poses[1].X = settings->pottery_width+sqrt(HAND_THRESHOLD)+0.1;
	hands_poses[1].Y = settings->pottery_width;
	hands_poses[1].Z = 0;

}

ShareNUI::~ShareNUI() {

}

