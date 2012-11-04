/*
 * Piece.h
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#ifndef PIECE_H_
#define PIECE_H_

#include <XnCppWrapper.h>
#include <list>
#include <vector>
#include "KPPoint.h"
#include <boost/thread/shared_mutex.hpp>

class Settings;

using namespace std;

class Piece {
public:
    boost::shared_mutex* piece_mutex; // 用于防止删除点时正在绘图
	list<KPPoint> kppoints;
	vector<XnVector3D> last_hands_poses;
	Settings* settings;

    Piece();
	Piece(Settings* p_settings);
	virtual ~Piece();

	void initPoint();
	void initHandsPositions();
	XnVector3D calculateNormal(int kppoint_index, bool is_init = false);
};

#endif /* PIECE_H_ */
