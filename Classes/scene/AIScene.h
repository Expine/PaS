#ifndef __AI_SCENE_H__
#define __AI_SCENE_H__

#include "cocos2d.h"
#include "ai/Ai.h"

class Stage;
enum class Owner;

/*
 * SLG Game scene
 * For AI
 */
class AIScene : public cocos2d::Layer
{
private:
	Stage* _stage;
protected:
	AIScene();
	virtual ~AIScene();
	virtual bool init(Stage* stage, Owner owner);
public:
	static cocos2d::Scene* createScene(Stage* stage, Owner owner);
	static AIScene* create(Stage* stage, Owner owner)
	{
		AIScene *pRet = new(std::nothrow) AIScene();
		if (pRet && pRet->init(stage, owner))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
	CC_SYNTHESIZE(std::function<void()>, _end_function, EndFunction);
	CC_SYNTHESIZE_RETAIN(PlayerAI*, _ai, AI);
};

#endif // __AI_SCENE_H__
