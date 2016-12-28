#include "SimulationScene.h"
#include "GameScene.h"
#include "AIScene.h"

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

SimulationScene::~SimulationScene()
{
	_turn = 0;
	CC_SAFE_RELEASE_NULL(_stage);
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
	CC_SAFE_RELEASE(_stage);
	_stage = Stage::parseStage("stage/test.mps");
	CC_SAFE_RETAIN(_stage);

	this->runAction(Sequence::create(
		DelayTime::create(0.5f),
		CallFunc::create([this] {
			nextTurn();
		}),
	NULL));
		
	return true;
}

/*
 * Next turn
 */
void SimulationScene::nextTurn()
{
	Scene* nextScene;
	// For player
	if (_turn == 0)
	{
		nextScene = Game::createScene(_stage);
		if (util::instanceof<Game>(nextScene->getChildren().at(0)))
			util::instance<Game>(nextScene->getChildren().at(0))->setEndFunction([this] { nextTurn(); });
	}
	// For AI
	else
	{
		nextScene = AIScene::createScene();
	}
	Director::getInstance()->pushScene(nextScene);
}