/*
 * Drawer.cpp
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#include "Drawer.h"
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include "Settings.h"
#include "ShareGL.h"
#include "ShareNUI.h"
#include "KPPoint.h"
#include <math.h>
#include <string>
#include <XnCppWrapper.h>
#include "def.h"
#include <thread/thread.hpp>
#include <thread/locks.hpp>

Drawer* Drawer::me;

Drawer::Drawer(Settings* p_settings, ShareGL* p_share_gl, ShareNUI* p_share_nui) {
	settings = p_settings;
	share_gl = p_share_gl;
	share_nui = p_share_nui;

	gl_rotation = 0;
}

Drawer::~Drawer() {
	// TODO Auto-generated destructor stub
}

void Drawer::draw()
{

}

void Drawer::updateData()
{

}
void Drawer::init(Drawer* p_me)
{
	// me 是指向自己的指针， 静态变量， 静态方法通过它找到对象
	me = p_me;
	me->scrape_on.loadFromFile("./images/scrape_on.bmp");
	me->scrape_off.loadFromFile("./images/scrape_off.bmp");
	me->add_on.loadFromFile("./images/add_on.bmp");
	me->add_off.loadFromFile("./images/add_off.bmp");
	me->pottery_piece.loadFromFile("./images/pottery_piece.bmp");
	me->myGlutInit();
	// 开始glut的循环
	glutMainLoop();
}

void Drawer::glutDisplay()
{
    glEnableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

    // 绘制按钮
    glTranslatef(0, -3, -12);
    glRasterPos2i(5, 6);
    if (me->share_gl->action_now == me->share_gl->ADD)
    {
        glDrawPixels(me->add_on.width, me->add_on.height,
            GL_BGR_EXT, GL_UNSIGNED_BYTE, me->add_on.bitmap_data);
    }else{
        glDrawPixels(me->add_off.width, me->add_off.height,
            GL_BGR_EXT, GL_UNSIGNED_BYTE, me->add_off.bitmap_data);
    }
    glRasterPos2i(5, 4);
    if (me->share_gl->action_now == me->share_gl->ADD)
    {
        glDrawPixels(me->scrape_off.width, me->scrape_off.height,
            GL_BGR_EXT, GL_UNSIGNED_BYTE, me->scrape_off.bitmap_data);
    }else{
        glDrawPixels(me->scrape_on.width, me->scrape_on.height,
            GL_BGR_EXT, GL_UNSIGNED_BYTE, me->scrape_on.bitmap_data);
    }

	glLoadIdentity();
	// 把绘图坐标向下移， 因为泥巴的y坐标都是正的
	glTranslatef(0, -3, -12);
	// 创建放顶点的数组
	GLfloat* verts = new GLfloat[3*3];
	GLfloat* text_coord = new GLfloat[2*3];
	GLfloat* normal = new GLfloat[3*3];

	glVertexPointer(3, GL_FLOAT, 0, verts);
	glTexCoordPointer(2, GL_FLOAT, 0, text_coord);
	glNormalPointer(GL_FLOAT, 0, normal);
	//创建放颜色的数组， 不是定点数组， 只是为了能够动态设定颜色
	GLfloat* color = new GLfloat[3]{0.5f, 0, 0.0f};
	int data_rotation = me->share_gl->rotation;
	// 如果数据处理的比绘图快， 则将旋转跟上， 比如数据处理到第4片， 绘图只绘制到第3片， 就要跟上
	me->gl_rotation += (data_rotation - me->gl_rotation)*0.1;
	// 当数据的旋转数增大到一定程度后， 就要将数据和绘图的旋转都清掉一圈， *4是要保证都为正
	// 都清掉一圈后数据和绘图的相对大小不会发生变化
	if (data_rotation > me->settings->pieces_in_pottery*4)
	{
		me->share_gl->rotation -= me->settings->pieces_in_pottery;
		me->gl_rotation -= me->settings->pieces_in_pottery;
	}

	// 进行模型的旋转
	glRotatef(2*360*me->gl_rotation/me->settings->pieces_in_pottery, 0, -1, 0);

	// 每一片画
	for (int step_outer = 0; step_outer != me->settings->pieces_in_pottery ; step_outer ++)
	{
		color[1] += 0.01f;
		color[2] = 0;
		int step_outer_next = step_outer + 1;
		// 如果转一圈就指向第一片
		if (step_outer == me->settings->pieces_in_pottery - 1)
		{
			step_outer_next = 0;
		}
		// 锁住这两片
		boost::unique_lock<boost::shared_mutex> lock1(
                                                *(me->share_gl->pieces[step_outer].piece_mutex));
		boost::unique_lock<boost::shared_mutex> lock2(
                                                *(me->share_gl->pieces[step_outer_next].piece_mutex));

		// 一片中画
		for (list<KPPoint>::iterator point_iter =
                    me->share_gl->pieces[step_outer].kppoints.begin(),
                next_piece_point_iter =
                    me->share_gl->pieces[step_outer_next].kppoints.begin();
			point_iter != --(me->share_gl->pieces[step_outer].kppoints.end());
			++point_iter, ++next_piece_point_iter )
        {

            /*for (int step = 0; step != me->settings->points_in_piece - 1; step ++)*/
			color[2] += 0.02f;
			//glColor3f(color[0], color[1], color[2]);
			/*XnVector3D point1 = me->share_gl->pieces[step_outer].kppoints[step].position;
			XnVector3D point2 = me->share_gl->pieces[step_outer_next].kppoints[step].position;
			XnVector3D point3 = me->share_gl->pieces[step_outer_next].kppoints[step + 1].position;
			XnVector3D point4 = me->share_gl->pieces[step_outer].kppoints[step + 1].position;*/

			XnVector3D point1 = point_iter->position;
			XnVector3D point2 = next_piece_point_iter->position;
			XnVector3D point3 = (++next_piece_point_iter)->position; --next_piece_point_iter;
			XnVector3D point4 = (++point_iter)->position; --point_iter;
			//printf("%f\n", me->share_gl->pieces->at(step_outer).kppoints->at(step).position.X);
			//printf("%d\n", step_outer_next);
			verts[0] = point1.X * cos(2 * PI * step_outer / me->settings->pieces_in_pottery);
			verts[1] = point1.Y;
			verts[2] = - point1.X * sin(2 * PI * step_outer / me->settings->pieces_in_pottery);
			verts[3] = point2.X * cos(2 * PI * step_outer_next / me->settings->pieces_in_pottery);
			verts[4] = point2.Y;
			verts[5] = - point2.X * sin(2 * PI * step_outer_next / me->settings->pieces_in_pottery);
			verts[6] = point3.X * cos(2 * PI * step_outer_next / me->settings->pieces_in_pottery);
			verts[7] = point3.Y;
			verts[8] = - point3.X * sin(2 * PI * step_outer_next / me->settings->pieces_in_pottery);
			//printf("%f, %f, %f, %f, %f, %f, %f, %f\n", verts[0],verts[1],verts[2],verts[3],verts[4],
					//verts[5],verts[6],verts[7],verts[8]);
            text_coord[0] = 0; text_coord[1] = 0; text_coord[2] = 1;
            text_coord[3] = 0; text_coord[4] = 1; text_coord[5] = 1;

            // 获取点的法线
            XnVector3D temp_normal = me->getNormal(verts[0], verts[1], verts[2], verts[3], verts[4], verts[5],
                                               verts[6], verts[7], verts[8]);
            normal[0] = normal[3] = normal[6] = temp_normal.X*NORMAL_SCALE;
            normal[1] = normal[4] = normal[7] = temp_normal.Y*NORMAL_SCALE;
            normal[2] = normal[5] = normal[8] = temp_normal.Z*NORMAL_SCALE;

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, me->texture_id);
			glDrawArrays(GL_TRIANGLES, 0, 3);
            //printf("%f==%f==%f\n",normal[0], normal[1], normal[2]);
            //glDrawArrays(GL_POINTS, 1, 1);

			verts[3] = verts[6];
			verts[4] = verts[7];
			verts[5] = verts[8];
			verts[6] = point4.X * cos(2 * PI * step_outer / me->settings->pieces_in_pottery);
			verts[7] = point4.Y;
			verts[8] = - point4.X * sin(2 * PI * step_outer / me->settings->pieces_in_pottery);

			// 获取点的法线
            temp_normal = me->getNormal(verts[0], verts[1], verts[2], verts[3], verts[4], verts[5],
                                               verts[6], verts[7], verts[8]);
            normal[0] = normal[3] = normal[6] = temp_normal.X*NORMAL_SCALE;
            normal[1] = normal[4] = normal[7] = temp_normal.Y*NORMAL_SCALE;
            normal[2] = normal[5] = normal[8] = temp_normal.Z*NORMAL_SCALE;
            //printf("%f-%f-%f\n",normal[0], normal[1], normal[2]);

            text_coord[0] = 0; text_coord[1] = 0; text_coord[2] = 1;
            text_coord[3] = 1; text_coord[4] = 0; text_coord[5] = 1;
			glDrawArrays(GL_TRIANGLES, 0, 3);
			glDisable(GL_TEXTURE_2D);
		}
		//对锁进行解锁
		lock2.~unique_lock();
		lock1.~unique_lock();
	}

	// 绘制NUI中手的点和其实际位置（如果离泥巴太近， 移动距离是有限制的， 不能移动到里面去）
	glLoadIdentity();
	glTranslatef(0, -3, -12);
	verts[0] = -me->share_nui->hands_poses[0].X;
	verts[1] = me->share_nui->hands_poses[0].Y;
	verts[2] = 0;
	normal[0] = 0; normal[1] = 0; normal[2] = 1;
	glPointSize(10);
	glColor3f(1, 0.3f, 0.5f);
	glDrawArrays(GL_POINTS, 0, 1);

	verts[0] = me->share_nui->hands_poses[1].X;
	verts[1] = me->share_nui->hands_poses[1].Y;
	verts[2] = 0;
	glPointSize(10);
	glColor3f(1, 0.3f, 0.5f);
	glDrawArrays(GL_POINTS, 0, 1);

    glColor3f(1.0f, 1.0f, 1.0f); // 重置颜色选择， 避免对贴图绘制产生影响
/*
	verts[0] = me->share_gl->act_hands_poses[0].X;
	verts[1] = me->share_gl->act_hands_poses[0].Y;
	verts[2] = 0;
	glColor3f(0.3, 0.3f, 0.5f);
	glDrawArrays(GL_POINTS, 0, 1);
*/


	glutSwapBuffers();
	delete verts;
	delete color;
	delete text_coord;
	delete normal;

    glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void Drawer::myGlutInit()
{
	int p_argc = 1;
	glutInit(&p_argc, 0);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(settings->screen_width, settings->screen_height);
	glutCreateWindow ("kipottery");
	//glutFullScreen();
	//glutSetCursor(GLUT_CURSOR_NONE);

	glutKeyboardFunc(glutKeyboard);
	glutDisplayFunc(glutDisplay);
	glutIdleFunc(glutIdle);
	glutReshapeFunc(myGlutReSizeGLScene);
	glutMouseFunc(myGlutMouseFunc);
	glutMotionFunc(myGlutMotionFunc);
	glutCloseFunc(myglutCloseFunc);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);		// This Will Clear The Background Color To Black
	glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
	glDisable(GL_DITHER);
	glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);				// The Type Of Depth Test To Do
	glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();				// Reset The Projection Matrix

	gluPerspective(45.0f,(GLfloat)(settings->screen_width/settings->screen_height)
			,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

     //打开光照
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightAmbientBuffer[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat lightDiffuseBuffer[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightPositionBuffer[] = {0.0f, 5.0f, 10.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbientBuffer);		//Setup The Ambient Light ( NEW )
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseBuffer);		//Setup The Diffuse Light ( NEW )
    glLightfv(GL_LIGHT0, GL_POSITION, lightPositionBuffer);	//Position The Light ( NEW )

	glMatrixMode(GL_MODELVIEW);

	// 加载和设定贴图
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &me->texture_id);
	glBindTexture(GL_TEXTURE_2D, me->texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, me->pottery_piece.width,
                 me->pottery_piece.height, 0, GL_BGR, GL_UNSIGNED_BYTE, me->pottery_piece.bitmap_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glDisable(GL_TEXTURE_2D);




}

XnVector3D Drawer::getNormal(GLfloat x1, GLfloat y1, GLfloat z1,
                      GLfloat x2, GLfloat y2, GLfloat z2,
                      GLfloat x3, GLfloat y3, GLfloat z3)
{
    XnVector3D temp;
    temp.X = (y2-y1)*(z3-z2)-(y3-y2)*(z2-z1);
    temp.Y = (z2-z1)*(x3-x2)-(x2-x1)*(z3-z2);
    temp.Z = (x2-x1)*(y3-y2)-(x3-x2)*(y2-y1);
    return temp;
}

void Drawer::glutKeyboard(unsigned char key, int x, int y)
{
	// 按下esc的时候退出
	switch (key)
	{
		case 27:
		{
            glutLeaveMainLoop();
            break;
		}
	}
}

void Drawer::glutIdle()
{
	// 如果退出位为真， 则退出
	glutPostRedisplay();
}
void Drawer::myglutCloseFunc()
{
    me->share_nui->should_run = false;
    //让出事件片，直到nui线程退出， 否则openni的wait**update会段错误
    while (!me->share_nui->nui_terminated)
    {
        boost::this_thread::yield();
    }
    //boost::this_thread::sleep(boost::posix_time::seconds(1));
}

void Drawer::myGlutReSizeGLScene(int Width, int Height)
{
	// glut窗口大小改变的时候呼叫
	if (Height==0) // Prevent A Divide By Zero If The Window Is Too Small
	{
		Height=1;
	}

	glViewport(0, 0, Width, Height); // Reset The Current Viewport And Perspective Transformation

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void Drawer::myGlutMouseFunc(int button,int state,int x,int y)
{
	// 鼠标点击的时候会呼叫这个方法， 不做处理
}
void Drawer::myGlutMotionFunc(int x,int y)
{
	// 点击着鼠标的移动才会呼叫这个方法， 先用这个模拟NUI，
	// 其点是2d的， z轴为0
	me->share_nui->hands_poses[0].X = (x-512) / 100.0;
	me->share_nui->hands_poses[0].Y = (650-y) / 100.0;
	me->share_nui->hands_poses[1].X = (x-512) / 100.0;
	me->share_nui->hands_poses[1].Y = (650-y) / 100.0;
}
