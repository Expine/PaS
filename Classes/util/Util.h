#ifndef __UTIL_H__
#define __UTIL_H__

#include "cocos2d.h"

class StageTile;
class City;

USING_NS_CC;

/*********************************************************/
namespace util
{
	constexpr float elim = 0.0000001f;

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

	/** Instanceof */
	template <typename Of, typename What>
	bool instanceof(const What w) { return dynamic_cast<const Of*>(w) != NULL; };

	/** Get instance */
	template <typename Of, typename What>
	Of* instance(const What w) { return dynamic_cast<Of*>(w); };

	Node* createCutSkin(const std::string &file, int w, int h, int cut_mask, int opacity = 255);
	Node* createCutSkinAndAnimation(const std::string &file, int w, int h, int wnum, int hnum, int cut_mask, float duration, int opacity = 255);

	inline Vec2 getHexUp(Vec2 ori) { return ori + Vec2(0, -2); };
	inline Vec2 getHexUpRight(Vec2 ori) { return ori + Vec2((int)(ori.y) % 2, -1); };
	inline Vec2 getHexUpLeft(Vec2 ori) { return ori + Vec2(((int)(ori.y) % 2) - 1, -1); };
	inline Vec2 getHexDown(Vec2 ori) { return ori + Vec2(0, 2); };
	inline Vec2 getHexDownRight(Vec2 ori) { return ori + Vec2((int)(ori.y) % 2, 1); };
	inline Vec2 getHexDownLeft(Vec2 ori) { return ori + Vec2((int)(ori.y) % 2 - 1, 1); };
	/** Get turn right vector */
	inline Vec2 getVecRight(Vec2 vec) { return Vec2(vec.x == 0 ? -vec.y : vec.x == vec.y ? 0 : vec.x, vec.x + vec.y == 0 ? -vec.y : vec.y); };
	/** Get turn left vector */
	inline Vec2 getVecLeft(Vec2 vec) { return Vec2(vec.x == 0 ? vec.y : vec.x == vec.y ? vec.x : 0, vec.x == vec.y ? -vec.y : vec.y); };
	/** Get turn right position */
	inline Vec2 getPosRight(Vec2 in, Vec2 pos) { return pos + (in.x + in.y == 0 ? Vec2(0, 2 * in.y) : in.y == 1 ? Vec2((int)pos.y % 2, 1 - 2 * in.x) : Vec2((int)pos.y % 2 - 1, -2 * in.x - 1)); };
	/** Get turn left position */
	inline Vec2 getPosLeft(Vec2 in, Vec2 pos) { return pos + (in.x == in.y ? Vec2(0, 2 * in.y) : in.y == 1 ? Vec2((int)pos.y % 2 - 1, 1 + 2 * in.x) : Vec2((int)pos.y % 2, 2 * in.x - 1)); };
	inline Vec2 getDirection(const Vec2 diff, Vec2& point)
	{
		// Down
		if (abs(diff.x) < elim && diff.y > 0)
		{
			point += Vec2(0, 2);
			return Vec2(0, -1);
		}
		// Up
		else if (abs(diff.x) < elim && diff.y < 0)
		{
			point += Vec2(0, -2);
			return Vec2(0, 1);
		}
		// Down left
		else if (diff.x > 0 && diff.y > 0)
		{
			point += Vec2((int)(point.y) % 2 - 1, 1);
			return Vec2(1, -1);
		}
		// Up left
		else if (diff.x > 0 && diff.y <= 0)
		{
			point += Vec2(((int)(point.y) % 2) - 1, -1);
			return Vec2(1, 1);
		}
		// Down right
		else if (diff.x < 0 && diff.y > 0)
		{
			point += Vec2((int)(point.y) % 2, 1);
			return Vec2(-1, -1);
		}
		// Up right
		else if (diff.x < 0 && diff.y <= 0)
		{
			point += Vec2((int)(point.y) % 2, -1);
			return Vec2(-1, 1);
		}
		return Vec2::ZERO;
	}


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
	* Find element in vec. If vec has element satisfying condition, return true.
	*/
	template<typename V, typename T>
	bool find(const std::vector<V>& vec, T element, std::function<bool(V, T)> cond)
	{
		for (V comp : vec)
			if (cond(comp, element))
				return true;
		return false;
	};

	/*
 	 * Find element satisfying condition. If not found, return nullptr
 	 */
	template<typename V>
	V findElement(const std::vector<V>& vec, std::function<bool(V)> cond)
	{
		for (V comp : vec)
			if (cond(comp))
				return comp;
		return nullptr;
	};

	/*
	 * Collect element and return vector
	 */
	template<typename V>
	std::vector<V> collect(const std::vector<V>& vec, std::function<bool(V)> cond)
	{
		auto result = std::vector<V>();
		for (V comp : vec)
			if (cond(comp))
				result.push_back(comp);
		return result;
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

	SpriteBatchNode* cloneBatch(SpriteBatchNode* ori, int tag = INT_MIN);
}

#endif // __UTIL_H__