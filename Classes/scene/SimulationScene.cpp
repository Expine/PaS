#include "SimulationScene.h"
#include "turn/PlayerTurn.h"
#include "turn/AITurn.h"

#include "ai/Owner.h"
#include "stage/Stage.h"
#include "util/Util.h"

USING_NS_CC;

ITurnLayer* TurnFactory::getTurn(Owner owner, Stage* stage)
{
	switch (owner)
	{
	case Owner::player:
		return PlayerTurn::create(stage);
	case Owner::enemy:
		return AITurn::create(stage);
	}
	CCASSERT(false, "Owner is not defined");
	return nullptr;
}


/*
 * Destructor
 */
SimulationScene::~SimulationScene()
{
	CC_SAFE_RELEASE_NULL(_stage);
	CC_SAFE_RELEASE_NULL(_game_scene);
	CC_SAFE_RELEASE_NULL(_ai_scene);
}

/*
 * Initialize
 * call nextTurn() late
 */
bool SimulationScene::init()
{
	if (!Layer::init())
		return false;

	// Set stage
	_stage = Stage::parseStage("stage/test.mps");
	CC_SAFE_RETAIN(_stage);

	// Set player
	addPlayer(Owner::player);
	addPlayer(Owner::enemy);

	// Delay call nect turn
	runAction(Sequence::create(
		DelayTime::create(0.1f),
		CallFunc::create([this] {
			nextTurn(true);
		}),
	NULL));

	return true;
}


/*
 * Next turn
 */
void SimulationScene::nextTurn(bool init)
{
	if(!init)
		Director::getInstance()->popScene();

	_stage->removeFromParentAndCleanup(true);

	auto owner = _players.front();
	_players.pop();
	_players.push(owner);

	if (owner == Owner::player)
	{
		auto scene = PlayerTurn::createScene(_stage);
		util::instance<PlayerTurn>(scene->getChildByTag(0))->setNextTurn(std::bind(&SimulationScene::nextTurn, this, false));
		Director::getInstance()->pushScene(scene);
	}
	else
	{
		auto scene = AITurn::createScene(_stage, owner);
		util::instance<AITurn>(scene->getChildByTag(0))->setNextTurn(std::bind(&SimulationScene::nextTurn, this, false));
		Director::getInstance()->pushScene(scene);
	}
}

