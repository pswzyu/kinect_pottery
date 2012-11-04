/*
 * ShareNUI.h
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#ifndef SHARENUI_H_
#define SHARENUI_H_

#include <XnCppWrapper.h>
#include <vector>

class Settings;

using namespace std;

class ShareNUI {
public:
	int any_player;
	bool should_run;
	bool nui_terminated;
	Settings* settings;
	vector<XnVector3D> hands_poses;

	ShareNUI(Settings* p_settings);
	virtual ~ShareNUI();
};

#endif /* SHARENUI_H_ */
