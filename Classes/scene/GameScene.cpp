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
	stage->onTap = [this, menu, stage](Vec2 v, std::vector<StageTile*> tiles)
	{
		for (StageTile* tile : tiles)
		{
			CCLOG("TILE IS %d", tile->getId());
			menu->setTile(tile);
			menu->setInfo(v.x, v.y);
		}
		for (auto preSprite : _preTiles)
		{
			preSprite->stopAllActions();
			preSprite->setColor(Color3B::WHITE);
			preSprite->removeAllChildren();
		}
		_preTiles.clear();
		for (auto sprite : stage->getTiles(v.x, v.y))
		{
			auto batch = SpriteBatchNode::create("TileSet/" + stage->getTileFile());
			auto wnum = (int)(batch->getTextureAtlas()->getTexture()->getContentSize().width / stage->getChipSize().x);
			auto hnum = (int)(batch->getTextureAtlas()->getTexture()->getContentSize().height / stage->getChipSize().y);;
			auto id = wnum * hnum - 1;
			auto white = Sprite::createWithTexture(batch->getTexture(), Rect((id % wnum) * stage->getChipSize().x, (int)(id / wnum) * stage->getChipSize().y, stage->getChipSize().x, stage->getChipSize().y));
			white->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			white->setOpacity(0);
			white->runAction(RepeatForever::create(Sequence::create(
				EaseExponentialIn::create(FadeTo::create(0.5f, 100)),
				EaseExponentialOut::create(FadeTo::create(0.5f, 0)),
				NULL)));
			sprite->addChild(white);
			_preTiles.pushBack(sprite);
		}
		auto unit = stage->getUnit(v.x, v.y);
		if (unit)
		{
			menu->setUnit(unit);
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
