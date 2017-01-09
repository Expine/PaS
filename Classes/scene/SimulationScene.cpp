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
	layer->setTag(0);
	scene->addChild(layer);
	return scene;
}

SimulationScene::~SimulationScene()
{
	CC_SAFE_RELEASE_NULL(_stage);
	CC_SAFE_RELEASE_NULL(_gameScene);
	CC_SAFE_RELEASE_NULL(_aiScene);
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

//	setGameScene(Game::createScene(_stage));
//	setAiScene(AIScene::createScene(_stage));
//	util::instance<Game>(_gameScene)->setEndFunction(std::bind(&SimulationScene::nextTurn, this, Owner::player));

	// Delay push initial scene
	runAction(Sequence::create(
		DelayTime::create(0.1f),
		CallFunc::create([this] {
//			Director::getInstance()->pushScene(_gameScene);
			nextTurn(Owner::enemy);
		}),
	NULL));

	return true;
}


/*
 * Next turn
 */
void SimulationScene::nextTurn(Owner owner)
{
	if(!util::instanceof<SimulationScene>(Director::getInstance()->getRunningScene()->getChildByTag(0)))
		Director::getInstance()->popScene();

	_stage->removeFromParentAndCleanup(true);

	if (owner == Owner::player)
	{
		auto scene = AIScene::createScene(_stage);
		util::instance<AIScene>(scene->getChildByTag(0))->setEndFunction(std::bind(&SimulationScene::nextTurn, this, Owner::enemy));
		Director::getInstance()->pushScene(scene);
	}
	else
	{
		auto scene = Game::createScene(_stage);
		util::instance<Game>(scene->getChildByTag(0))->setEndFunction(std::bind(&SimulationScene::nextTurn, this, Owner::player));
		Director::getInstance()->pushScene(scene);
	}
}