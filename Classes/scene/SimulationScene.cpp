#include "SimulationScene.h"
#include "GameScene.h"
#include "AIScene.h"

#include "ai/Owner.h"
#include "stage/Stage.h"
#include "util/Util.h"

USING_NS_CC;

/*
 * Create Scene
 */
Scene * SimulationScene::createScene()
{
	CCLOG("SimulationScene is created");
	auto scene = Scene::create();
	auto layer = SimulationScene::create();
	scene->addChild(layer);
	return scene;
}

/*
 * Constructor
 */
SimulationScene::SimulationScene()
	: _stage(nullptr)
	, _game_scene(nullptr), _ai_scene(nullptr)
{}

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

	/*
	if (owner == Owner::player)
	{
		auto scene = Game::createScene(_stage);
		util::instance<Game>(scene->getChildByTag(0))->setEndFunction(std::bind(&SimulationScene::nextTurn, this, false));
		Director::getInstance()->pushScene(scene);
	}
	else*/
	{
		auto scene = AIScene::createScene(_stage, owner);
		util::instance<AIScene>(scene->getChildByTag(0))->setEndFunction(std::bind(&SimulationScene::nextTurn, this, false));
		Director::getInstance()->pushScene(scene);
	}
}