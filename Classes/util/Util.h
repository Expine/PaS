#ifndef __UTIL_H__
#define __UTIL_H__

#include "cocos2d.h"

USING_NS_CC;

/*********************************************************/
namespace util
{
	inline std::string loadText(const std::string &file);

	std::vector<std::string> splitString(const std::string& input, char delim);

	std::vector<std::string> splitFile(const std::string &file);

	/*
	* Checking node range contains point by point and node data.
	* down_x or y is extension of left or lower.
	* append_x or y is extension of right or upper.
	*/
	bool isTouchInEvent(Vec2 point, Node* t, int down_x, int down_y, int append_x, int append_y);
	

	/*
	* Checking event range contains touch point by touch and event data.
	* down_x or y is extension of left or lower.
	* append_x or y is extension of right or upper.
	*/
	bool isTouchInEvent(Touch* touch, Event* event, int down_x, int down_y, int append_x, int append_y);

	/*
	* Checking node range contains point by point and node data.
	*/
	bool isTouchInEvent(Vec2 point, Node* t);

	/*
	* Checking event range contains point by point and event data.
	*/
	bool isTouchInEvent(Vec2 point, Event* event);

	/*
	* Checking event range contains touch point by touch and event data.
	*/
	bool isTouchInEvent(Touch* touch, Event* event);
}

#endif // __UTIL_H__