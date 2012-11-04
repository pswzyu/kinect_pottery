/*
 * Settings.h
 *
 *  Created on: 2012-6-7
 *      Author: pswz-zhangy
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

class Settings {
public:
	int points_in_piece;
	int pieces_in_pottery;

	float pottery_width;
	//float pottery_height;

	int screen_width;
	int screen_height;

	Settings();
	virtual ~Settings();
};

#endif /* SETTINGS_H_ */
