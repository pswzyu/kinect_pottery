/*
 * main.cpp
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */
#include <thread/thread.hpp>
#include "DataProcesser.h"
#include "Settings.h"
#include "ShareGL.h"
#include "ShareNUI.h"
#include "Drawer.h"
#include "nuimanager.h"

int main()
{
	Settings* settings = new Settings();
	ShareNUI* share_nui = new ShareNUI(settings);
	ShareGL* share_gl = new ShareGL(settings);
	DataProcesser* data_processer = new DataProcesser(settings, share_gl, share_nui);
	Drawer* drawer = new Drawer(settings, share_gl, share_nui);
	NuiManager* nuimanager = new NuiManager(settings, share_nui);

	boost::thread data_thread(DataProcesser::init, data_processer);
	boost::thread draw_thread(Drawer::init, drawer);
	boost::thread nui_thread(NuiManager::init, nuimanager);

	data_thread.join();
	nui_thread.join();
	share_nui->nui_terminated = true;
	draw_thread.join();

	return 0;
}

