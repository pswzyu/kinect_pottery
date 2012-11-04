/*
 * ShareGL.cpp
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#include "ShareGL.h"
#include "Settings.h"
#include "Piece.h"

ShareGL::ShareGL(Settings* p_settings) : act_hands_poses(vector<XnVector3D>(2)) {
	settings = p_settings;
	rotation = 0;
	action_now = SCRAPE;

	for (int step = 0; step != settings->pieces_in_pottery; step ++)
	{
		pieces.push_back(Piece(settings));
	}

}

ShareGL::~ShareGL() {
	// TODO Auto-generated destructor stub
}

