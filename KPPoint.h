/*
 * KPPoint.h
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#ifndef KPPOINT_H_
#define KPPOINT_H_

#include <XnCppWrapper.h>

class KPPoint {
public:
	XnVector3D position;
	XnVector3D normal;

	KPPoint();
	KPPoint(XnVector3D p_position, XnVector3D p_normal);
	virtual ~KPPoint();
};

#endif /* KPPOINT_H_ */
