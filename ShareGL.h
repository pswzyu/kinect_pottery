/*
 * ShareGL.h
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#ifndef SHAREGL_H_
#define SHAREGL_H_

#include "Piece.h"
#include <vector>
#include <XnCppWrapper.h>

class Settings;

using namespace std;

class ShareGL {
public:
	vector<Piece> pieces;
	int rotation;
	vector<XnVector3D> act_hands_poses;
	Settings* settings;

	enum ACTION_TYPE
	{
	    SCRAPE, ADD
	};
	ACTION_TYPE action_now;

	ShareGL(Settings* p_settings);
	virtual ~ShareGL();
};

#endif /* SHAREGL_H_ */
