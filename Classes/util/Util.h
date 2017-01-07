#ifndef __UTIL_H__
#define __UTIL_H__

#include "cocos2d.h"

class StageTile;

USING_NS_CC;

/*********************************************************/
namespace util
{
	constexpr int CUT_MASK_RIGHT = 1;
	constexpr int CUT_MASK_LEFT = 2;
	constexpr int CUT_MASK_UP = 4;
	constexpr int CUT_MASK_DOWN = 8;

	std::string loadText(const std::string &file);
	std::vector<std::string> splitString(const std::string& input, char delim);
	std::vector<std::string> splitFile(const std::string &file);

	bool isTouchInEvent(Vec2 point, Node* t, int down_x, int down_y, int append_x, int append_y);
	bool isTouchInEvent(Touch* touch, Event* event, int down_x, int down_y, int append_x, int append_y);
	bool isTouchInEvent(Vec2 point, Node* t);
	bool isTouchInEvent(Vec2 point, Event* event);
	bool isTouchInEvent(Touch* touch, Event* event);

	Vec2 getCoordinateInScreen(Vec2 pos, Node* target);

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

	Node* createCutSkin(const std::string &file, int w, int h, int cut_mask, int opacity = 255);
	Node* createCutSkinAndAnimation(const std::string &file, int w, int h, int wnum, int hnum, int cut_mask, float duration, int opacity = 255);

	template<typename V>
	bool find(const std::vector<V>& vec, V element)
	{
		for (V comp : vec)
			if (comp == element)
				return true;
		return false;
	};
	template<typename V, typename T>
	bool find(const std::vector<V>& vec, T element, std::function<bool(V, T)> func)
	{
		for (V comp : vec)
			if(func(comp, element))
				return true;
		return false;
	};
}

#endif // __UTIL_H__