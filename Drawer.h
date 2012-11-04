/*
 * Drawer.h
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#ifndef DRAWER_H_
#define DRAWER_H_

#include "manager.h"
#include "Bitmap.h"
#include <GL/gl.h>
#include <XnCppWrapper.h>
class Settings;
class ShareGL;
class ShareNUI;

class Drawer : public Manager {
public:
	Settings* settings;
	ShareGL* share_gl;
	ShareNUI* share_nui;
	static Drawer* me;
	GLfloat gl_rotation;

	Bitmap scrape_on;
	Bitmap scrape_off;
	Bitmap add_on;
	Bitmap add_off;
	Bitmap pottery_piece;
	GLuint texture_id;

	Drawer(Settings* p_settings, ShareGL* p_share_gl, ShareNUI* p_share_nui);
	virtual ~Drawer();

	static void init(Drawer* p_me);
	void draw();
	void updateData();
	XnVector3D getNormal(GLfloat x1, GLfloat y1, GLfloat z1,
                      GLfloat x2, GLfloat y2, GLfloat z2,
                      GLfloat x3, GLfloat y3, GLfloat z3);

	static void glutDisplay();
	void myGlutInit();
	static void glutKeyboard(unsigned char key, int x, int y);
	static void glutIdle();
	static void myGlutReSizeGLScene(int Width, int Height);
	static void myGlutMouseFunc(int button,int state,int x,int y);
	static void myGlutMotionFunc(int x,int y);
	static void myglutCloseFunc();

};

#endif /* DRAWER_H_ */
