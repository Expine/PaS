#ifndef __AI_SCENE_H__
#define __AI_SCENE_H__

#include "cocos2d.h"

class Stage;

/*
 * SLG Game scene
 * For AI
 */
class AIScene : public cocos2d::Layer
{
private:
	Stage* _stage;
protected:
	AIScene()
		: _stage(nullptr), _endFunction(nullptr)
	{};
	~AIScene()
	{
		_stage = nullptr;
		_endFunction = nullptr;
	}
	virtual bool init(Stage* stage);
public:
	static cocos2d::Scene* createScene(Stage* stage);
	static AIScene* create(Stage* stage)
	{
		AIScene *pRet = new(std::nothrow) AIScene();
		if (pRet && pRet->init(stage))
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
	CC_SYNTHESIZE(std::function<void()>, _endFunction, EndFunction);
};

#endif // __AI_SCENE_H__
