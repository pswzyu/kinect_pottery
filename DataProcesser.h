/*
 * DataProcesser.h
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#ifndef DATAPROCESSER_H_
#define DATAPROCESSER_H_

#include "manager.h"
#include <XnCppWrapper.h>

class Settings;
class ShareGL;
class ShareNUI;
class Piece;

class DataProcesser : public Manager{
public:
	Settings* settings;
	ShareGL* share_gl;
	ShareNUI* share_nui;
	int check_point;

	DataProcesser();
	DataProcesser(Settings* p_settings, ShareGL* p_share_gl, ShareNUI* p_share_nui);
	virtual ~DataProcesser();

	static void init(DataProcesser* me);
	void updateData();
	void makePieces();
	void getPointsInHands();
	void checkNormalOrientation();
	void checkButtonAndAction();
	XnVector3D getPointMove(int point_pos, XnVector3D direction, float move);
	void doModify(Piece*, int);
	void doFixPiece();

};

#endif /* DATAPROCESSER_H_ */
