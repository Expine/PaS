#include "GameScene.h"
#include "stage/Stage.h"

USING_NS_CC;

Scene* Game::createScene()
{
	CCLOG("GameScene Create");
	auto scene = Scene::create();
    auto layer = Game::create();
    scene->addChild(layer);
    return scene;
}

bool Game::init()
{
    if ( !Layer::init() )
        return false;

	auto stage = Stage::parseStage("stage/test.mps");
	this->addChild(stage);

    return true;
}
