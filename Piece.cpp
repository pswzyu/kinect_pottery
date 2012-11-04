/*
 * Piece.cpp
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#include "Piece.h"
#include "Settings.h"
#include "KPPoint.h"
#include <math.h>
#include <XnCppWrapper.h>
#include "def.h"

Piece::Piece(){}
Piece::Piece(Settings* p_settings) : last_hands_poses(vector<XnVector3D>(2)) {

	settings = p_settings;
	piece_mutex = new boost::shared_mutex();

	initPoint();
	initHandsPositions();
}

Piece::~Piece() {
}
void Piece::initHandsPositions()
{
	last_hands_poses[0].X = settings->pottery_width+sqrt(HAND_THRESHOLD)+0.1;
	last_hands_poses[0].Y = settings->pottery_width;
	last_hands_poses[0].Z = 0;
	last_hands_poses[1].X = settings->pottery_width+sqrt(HAND_THRESHOLD)+0.1;
	last_hands_poses[1].Y = settings->pottery_width;
	last_hands_poses[1].Z = 0;
}

void Piece::initPoint()
{
	// TODO: 先初始化为一个球， 以后初始化为圆柱
	for (int step = 0; step != settings->points_in_piece; step ++)
	{
		KPPoint temp_point;
		temp_point.position.X = settings->pottery_width
				* sin(PI * step / (settings->points_in_piece - 1));
		temp_point.position.Y = settings->pottery_width
				* (1 - cos(PI * step / (settings->points_in_piece - 1)));
		temp_point.position.Z = 0;

		kppoints.push_back(temp_point);
	}
}

