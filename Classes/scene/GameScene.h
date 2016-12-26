#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Game: public cocos2d::Layer
{
private:
	cocos2d::Vector<cocos2d::Sprite*> _preTiles;
protected:
	virtual bool init();
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(Game);
};

#endif // __GAME_SCENE_H__
