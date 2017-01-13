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

	/*
	 * Find element in vec. If vec has element, return true.
	 */
	template<typename V>
	bool find(const std::vector<V>& vec, V element)
	{
		for (V comp : vec)
			if (comp == element)
				return true;
		return false;
	};
	/*
	* Find element in vec. If vec has element satisfying func, return true.
	*/
	template<typename V, typename T>
	bool find(const std::vector<V>& vec, T element, std::function<bool(V, T)> func)
	{
		for (V comp : vec)
			if(func(comp, element))
				return true;
		return false;
	};

	/*
	 * Sort map by value
	 */
	template<typename V, typename T>
	std::vector<std::pair<V, T>> sortMap(const std::map<V, T>& map, bool isGreater = true)
	{
		std::vector<std::pair<V, T>> arr;
		
		for (std::pair<V, T> pair : map)
			arr.push_back(pair);

		if (isGreater)
			std::sort(arr.begin(), arr.end(), [](const std::pair<V, T>& left, const std::pair<V, T> right) {return left.second > right.second; });
		else
			std::sort(arr.begin(), arr.end(), [](const std::pair<V, T>& left, const std::pair<V, T> right) {return left.second < right.second; });

		return arr;
	};

	void initRand();

	int getRand(int min, int max);

	double getRand(double min, double max);
}

#endif // __UTIL_H__