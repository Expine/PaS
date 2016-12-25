#include "GameScene.h"
#include "stage/Menu.h"
#include "stage/Stage.h"
#include "stage/Tile.h"

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

	auto menu = MenuLayer::create();

	auto stage = Stage::parseStage("stage/test.mps");
	stage->onTap = [menu, stage](Vec2 v, std::vector<StageTile*> tiles)
	{
		for (StageTile* tile : tiles)
		{
			CCLOG("TILE IS %d", tile->getId());
			menu->setTile(tile);
			menu->setInfo(v.x, v.y);
		}
	};
	stage->onSwipeCheck = [menu] (Vec2 v, Vec2 diff, float time)
	{
		return menu->checkAllAction(diff);
	};
	stage->onFlickCheck = [menu](Vec2 v, Vec2 diff, float time)
	{
		auto result = menu->checkAllAction(diff);
		menu->resetOnFrame();
		return result;
	};
	this->addChild(stage);

	this->addChild(menu, 10);

    return true;
}
