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
		//Set unit information
		auto unit = stage->getUnit(v.x, v.y);
		if (unit)
		{
			if (_preUnit)
				_preUnit->removeAllChildren();

			if (unit != _preUnit)
			{
				auto inner = Sprite::createWithTexture(unit->getTexture(), Rect(32, static_cast<int>(unit->getType()) * 32, 32, 32));
				inner->setBlendFunc(BlendFunc::ADDITIVE);
				inner->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
				inner->setOpacity(0);
				inner->runAction(RepeatForever::create(Sequence::create(
					EaseExponentialIn::create(FadeTo::create(0.5f, 100)),
					EaseExponentialOut::create(FadeTo::create(0.5f, 0)),
					NULL)));
				unit->addChild(inner);
			}
			else
				unit = nullptr;
			menu->setUnit(unit);
			_preUnit = unit;
		}

		//Reset tile information
		for (auto preSprite : _preTiles)
			preSprite->removeAllChildren();

		_preTiles.clear();
		//Set tile information
		for (auto tile : tiles)
		{
			menu->setTile(tile, _preUnit);
			auto wnum = (int)(tile->getTexture()->getContentSize().width / stage->getChipSize().x);
			auto hnum = (int)(tile->getTexture()->getContentSize().height / stage->getChipSize().y);;
			auto id = wnum * hnum - 1;
			auto white = Sprite::createWithTexture(tile->getTexture(), Rect((id % wnum) * stage->getChipSize().x, (int)(id / wnum) * stage->getChipSize().y, stage->getChipSize().x, stage->getChipSize().y));
			white->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			white->setOpacity(0);
			white->runAction(RepeatForever::create(Sequence::create(
				EaseExponentialIn::create(FadeTo::create(0.5f, 100)),
				EaseExponentialOut::create(FadeTo::create(0.5f, 0)),
				NULL)));
			tile->addChild(white);
			_preTiles.pushBack(tile);
		}

		//Set base information
		menu->setInfo(v.x, v.y);
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
