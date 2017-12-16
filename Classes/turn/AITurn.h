#ifndef __AI_TURN_H__
#define __AI_TURN_H__

#include "TurnLayer.h"
#include "ai/Ai.h"

class Stage;
enum class Owner;

/*
 * SLG Game scene
 * For AI
 */
class AITurn : public ITurnLayer
{
private:
	Stage* _stage;
protected:
	AITurn();
	virtual ~AITurn();
	virtual bool init(Stage* stage, Owner owner);
public:
	virtual void initTurn() {};
	static cocos2d::Scene* createScene(Stage* stage, Owner owner);
	static AITurn* create(Stage* stage, Owner owner)
	{
		AITurn *pRet = new(std::nothrow) AITurn();
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
	CC_SYNTHESIZE_RETAIN(PlayerAI*, _ai, AI);
};

#endif // __AI_TURN_H__
