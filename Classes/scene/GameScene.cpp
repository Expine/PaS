#include "GameScene.h"

#include "stage/Menu.h"
#include "stage/Stage.h"
#include "stage/Tile.h"
#include "entity/Entity.h"

USING_NS_CC;

/*
 * Create scene
 */
Scene* Game::createScene()
{
	CCLOG("GameScene Create");
	auto scene = Scene::create();
    auto layer = Game::create();
    scene->addChild(layer);
    return scene;
}

/*
 * Initialize
 */
bool Game::init()
{
    if ( !Layer::init() )
        return false;

	// Set stage
	auto stage = Stage::parseStage("stage/test.mps");
	this->addChild(stage);

	// Set menu
	auto menu = MenuLayer::create();
	menu->setStage(stage);
	this->addChild(menu, 10);

	// Set listener
	stage->onTap = [this, menu, stage](Vec2 v, std::vector<StageTile*> tiles)
	{
		//Set unit information
		auto unit = stage->getUnit(v.x, v.y);
		if (unit)
		{
			// Remove pre-unit's blink
			if (_preUnit)
				_preUnit->removeAllChildren();

			// If tap same unit, only remove
			if (unit == _preUnit)
				unit = nullptr;
			// else, create blink
			else
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
			auto tex = tile->getTexture();
			auto size = stage->getChipSize();
			auto white = Sprite::createWithTexture(tex, Rect(tex->getContentSize().width - size.x, tex->getContentSize().height - size.y, size.x, size.y));
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
	// if menu action, swipe is disable
	stage->onSwipeCheck = [menu] (Vec2 v, Vec2 diff, float time)
	{
		return menu->checkAllAction(diff);
	};
	// if menu action, swipe is disable
	stage->onFlickCheck = [menu](Vec2 v, Vec2 diff, float time)
	{
		auto result = menu->checkAllAction(diff);
		menu->resetOnFrame();
		return result;
	};

    return true;
}
