/*
 * KPPoint.cpp
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#include "KPPoint.h"

KPPoint::KPPoint()
{

}

KPPoint::KPPoint(XnVector3D p_position, XnVector3D p_normal) {
	position = p_position;
	normal = p_normal;

}

KPPoint::~KPPoint() {
	// TODO Auto-generated destructor stub
}

