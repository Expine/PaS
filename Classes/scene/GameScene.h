#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Entity;

/*
 * SLG Game scene
 * For player turn
 */
class Game: public cocos2d::Layer
{
private:
	cocos2d::Vector<cocos2d::Sprite*> _preTiles;
	Entity* _preUnit;
protected:
	Game()
		: _preUnit(nullptr)
	{};
	virtual bool init();
public:
	CREATE_FUNC(Game);
	CC_SYNTHESIZE(std::function<void()>, _endFunction, EndFunction);
	static cocos2d::Scene* createScene();
};

#endif // __GAME_SCENE_H__
