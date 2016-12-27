#ifndef __UTIL_H__
#define __UTIL_H__

#include "cocos2d.h"

USING_NS_CC;

/*********************************************************/
namespace util
{
	std::string loadText(const std::string &file);
	std::vector<std::string> splitString(const std::string& input, char delim);
	std::vector<std::string> splitFile(const std::string &file);

	bool isTouchInEvent(Vec2 point, Node* t, int down_x, int down_y, int append_x, int append_y);
	bool isTouchInEvent(Touch* touch, Event* event, int down_x, int down_y, int append_x, int append_y);
	bool isTouchInEvent(Vec2 point, Node* t);
	bool isTouchInEvent(Vec2 point, Event* event);
	bool isTouchInEvent(Touch* touch, Event* event);

	/*
	* Instanceof
	*/
	template <typename Of, typename What>
	bool instanceof(const What w) { return dynamic_cast<const Of*>(w) != NULL; };

	/*
	* Get instance
	*/
	template <typename Of, typename What>
	Of* instance(const What w) { return dynamic_cast<Of*>(w); };
}

#endif // __UTIL_H__