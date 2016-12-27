#ifndef __AI_SCENE_H__
#define __AI_SCENE_H__

#include "cocos2d.h"

/*
 * SLG Game scene
 * For AI
 */
class AIScene : public cocos2d::Layer
{
protected:
	AIScene()
	{};
public:
	CREATE_FUNC(AIScene);
	static cocos2d::Scene* createScene();
};

#endif // __AI_SCENE_H__
