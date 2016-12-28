#include "Menu.h"
#include "Tile.h"

#include "entity/EntityToTile.h"
#include "entity/Entity.h"

#include "util/MultiListener.h"
#include "util/Util.h"

#define EN_FONT "Arial"
#define JP_FONT "MS ゴシック"
#define INFO_SIZE 20
#define MODIFY 30

USING_NS_CC;

/*
 * Initialize
 * Set sprite and listener
 */
bool MenuLayer::init()
{
	if (!Node::init())
		return false;

	auto winSize = Director::getInstance()->getWinSize();

	// Set unit frame
	_unit = Sprite::create();
	_unit->setTextureRect(Rect(0, 0, 250, 200));
	_unit->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_unit->setPosition(10, 230);
	_unit->setColor(Color3B(0, 148, 255));
	_unit->setOpacity(100);
	this->addChild(_unit);


	// Set unit label
	for (std::string name : { "U", "N", "I", "T" })
	{
		auto unitLabel = Label::createWithSystemFont(name, EN_FONT, INFO_SIZE);
		unitLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		unitLabel->setColor(Color3B::BLACK);
		if (_unitLabels.size() == 0)
			unitLabel->setPosition(5, _unit->getTextureRect().getMaxY() - 5);
		else
			unitLabel->setPosition(_unitLabels.back()->getPosition().x + _unitLabels.back()->getContentSize().width, _unit->getTextureRect().getMaxY() - 5);
		unitLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		_unit->addChild(unitLabel);
		_unitLabels.push_back(unitLabel);
	}

	// Set mao frame
	_map = Sprite::create();
	_map->setTextureRect(Rect(0, 0, 250, 150));
	_map->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_map->setPosition(10, 20);
	_map->setColor(Color3B(0, 148, 255));
	_map->setOpacity(100);
	this->addChild(_map);

	// Set map label
	for (std::string name : { "M", "A", "P" })
	{
		auto mapLabel = Label::createWithSystemFont(name, EN_FONT, INFO_SIZE);
		mapLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		mapLabel->setColor(Color3B::BLACK);
		if(_mapLabels.size() == 0)
			mapLabel->setPosition(5, _map->getTextureRect().getMaxY() - 5);
		else
			mapLabel->setPosition(_mapLabels.back()->getPosition().x + _mapLabels.back()->getContentSize().width, _map->getTextureRect().getMaxY() - 5);
		mapLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		_map->addChild(mapLabel);
		_mapLabels.push_back(mapLabel);
	}

	// Set menu frame
	_menu = Sprite::create();
	_menu->setTextureRect(Rect(0, 0, 200, 400));
	_menu->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	_menu->setPosition(winSize.width - 20, winSize.height - 20);
	_menu->setColor(Color3B(0, 148, 255));
	_menu->setOpacity(100);
	this->addChild(_menu);

	// Set menu label
	for (std::string name : { "M", "E", "N", "U" })
	{
		auto menuLabel = Label::createWithSystemFont(name, EN_FONT, INFO_SIZE);
		menuLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
		menuLabel->setColor(Color3B::BLACK);
		if(_menuLabels.size() == 0)
			menuLabel->setPosition(5, _menu->getTextureRect().getMaxY() - 5);
		else
			menuLabel->setPosition(_menuLabels.back()->getPosition().x + _menuLabels.back()->getContentSize().width, _menu->getTextureRect().getMaxY() - 5);
		menuLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		_menu->addChild(menuLabel);
		_menuLabels.push_back(menuLabel);
	}

	// Set menu item
	for (auto name : { u8"フェイズ終了", u8"次の都市へ", u8"次の部隊へ", u8"参謀と話す", u8"セーブ", u8"ロード", u8"オプション" })
	{
		static Label* preItem = nullptr;
		auto item = Label::createWithSystemFont(name, JP_FONT, INFO_SIZE + 5);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		item->setColor(Color3B::BLACK);
		item->setPosition(_menu->getTextureRect().getMaxX() - 10, preItem ? preItem->getPosition().y - 50 : _menu->getTextureRect().getMaxY() - 50);
		item->setTag(preItem ? preItem->getTag() + 1 : 10);
		_menu->addChild(item);
		preItem = item;
	}

	// Set info frame
	_info = Sprite::create();
	_info->setTextureRect(Rect(0, 0, 350, 40));
	_info->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	_info->setPosition(winSize.width, 0);
	_info->setColor(Color3B(0, 0, 0));
	_info->setOpacity(100);
	this->addChild(_info);

	// Set listener
	setFrameListener(_unit, _unitLabels, FrameType::unit, 250 - MODIFY);
	setFrameListener(_map, _mapLabels, FrameType::map, 250 - MODIFY);
	setFrameListener(_menu, _menuLabels, FrameType::menu, -200 + MODIFY);

	setMenuListener(_menu->getChildByTag(10), [this] { if (endPhase) endPhase(); });
	setMenuListener(_menu->getChildByTag(11), [this] { if (nextCity) nextCity(); });
	setMenuListener(_menu->getChildByTag(12), [this] { if (nextUnit) nextUnit(); });
	setMenuListener(_menu->getChildByTag(13), [this] { if (talkStaff) talkStaff(); });
	setMenuListener(_menu->getChildByTag(14), [this] { if (save)	save(); });
	setMenuListener(_menu->getChildByTag(15), [this] { if (load)	load(); });
	setMenuListener(_menu->getChildByTag(16), [this] { if (option)	option(); });

	return true;
}

/*
 * Set tile information
 */
void MenuLayer::setTile(StageTile * tile, Entity* unit)
{
	// Remove pre-information
	if (_map->getChildByTag(0) != NULL)
	{
		_map->removeChildByTag(0);
		_map->removeChildByTag(1);
	}
	if (_map->getChildByTag(2) != NULL)
	{
		_map->removeChildByTag(2);
		_map->removeChildByTag(3);
	}

	if (!tile)
		return;

	// Set tile image
	auto id = tile->getId();
	auto tileImage = Sprite::create("image/tileExp.png", Rect((id % 8) * 18, id / 8 * 18, 18, 18));
	tileImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	tileImage->setPosition(10, _map->getTextureRect().getMidY());
	tileImage->setScale(3.5f);
	tileImage->setTag(0);
	_map->addChild(tileImage);

	// Set explanation
	auto exp = Label::createWithSystemFont(tile->getExplanation(), JP_FONT, INFO_SIZE);
	exp->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	exp->setPosition(75, 120);
	exp->setColor(Color3B::BLACK);
	exp->setWidth(_map->getTextureRect().getMaxX() - 75 - MODIFY);
	exp->setTag(1);
	_map->addChild(exp);

	if (!unit)
		return;

	// Set move cost
	auto cost = Label::createWithSystemFont(StringUtils::format("Move cost : %d", EntityToTile::getInstance()->getSearchCost(tile->getTerrainType(), unit->getType())), EN_FONT, INFO_SIZE);
	cost->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	cost->setPosition(90, 25);
	cost->setColor(Color3B::BLACK);
	cost->setTag(2);
	_map->addChild(cost);

	// Set effect
	auto effect = Label::createWithSystemFont(StringUtils::format("Effect : %d%%", EntityToTile::getInstance()->getEffect(tile->getTerrainType(), unit->getType())), EN_FONT, INFO_SIZE);
	effect->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	effect->setPosition(90, 5);
	effect->setColor(Color3B::BLACK);
	effect->setTag(3);
	_map->addChild(effect);
}

/*
 * Set unit information
 */
void MenuLayer::setUnit(Entity* unit)
{
	if (_unit->getChildByTag(0) != NULL)
	{
		_unit->removeChildByTag(0);
		_unit->removeChildByTag(1);
	}

	if (!unit)
		return;

	// Set unit image
	auto unitImage = Sprite::create("image/unit.png", Rect(0, static_cast<int>(unit->getType()) * 32, 32, 32));
	unitImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	unitImage->setPosition(10, _unit->getTextureRect().getMidY());
	unitImage->setScale(2.0f);
	unitImage->setTag(0);
	_unit->addChild(unitImage);

	// Set unit explanation
	auto exp = Label::createWithSystemFont(unit->getExplanation(), JP_FONT, INFO_SIZE);
	exp->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	exp->setPosition(75, 160);
	exp->setColor(Color3B::BLACK);
	exp->setWidth(_map->getTextureRect().getMaxX() - 75 - MODIFY);
	exp->setTag(1);
	_unit->addChild(exp);
}

/*
 * Set info data
 */
void MenuLayer::setInfo(int x, int y)
{
	if (_info->getChildByTag(0) != NULL)
	{
		_info->removeChildByTag(0);
		_info->removeChildByTag(1);
	}

	//Set x coordinate
	auto xLabel = Label::createWithSystemFont(StringUtils::format("X = %d", x), EN_FONT, INFO_SIZE);
	xLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	xLabel->setPosition(5, _info->getTextureRect().getMidY());
	xLabel->setTag(0);
	_info->addChild(xLabel);

	//Set y coordinate
	auto yLabel = Label::createWithSystemFont(StringUtils::format("Y = %d", y), EN_FONT, INFO_SIZE);
	yLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	yLabel->setPosition(80, _info->getTextureRect().getMidY());
	yLabel->setTag(1);
	_info->addChild(yLabel);
}

/*
 * Set frame listener
 */
void MenuLayer::setFrameListener(Sprite *target, const std::vector<Label*>& targets, FrameType type, int moveX)
{
	auto unitListener = SingleTouchListener::create();
	// Touch check. if isTouchInEvent is false, all process cancelled
	unitListener->onTouchBeganChecking = [this, type](Touch* touch, Event* event)
	{
		auto result = util::isTouchInEvent(touch, event);
		//Set on frame
		switch(type)
		{
		case FrameType::unit:	setOnUnitFrame(result);	break;
		case FrameType::map:	setOnMapFrame(result);	break;
		case FrameType::menu:	setOnMenuFrame(result);	break;
		}
		return result;
	};
	// Tap process
	unitListener->onTap = [this, target, targets, type, moveX](Touch* touch, Event* event)
	{
		//Unless on frame, do nothing
		switch (type)
		{
		case FrameType::unit:	if (!getOnUnitFrame()) return; break;
		case FrameType::map:	if (!getOnMapFrame())  return; break;
		case FrameType::menu:	if (!getOnMenuFrame()) return; break;
		}
		// if hided, show frame
		if (isHided(type))
		{
			target->runAction(EaseSineIn::create(MoveBy::create(0.5f, Vec2(moveX, 0))));
			auto x = 0.0f;
			auto y = 0.0f;
			for (auto unitLabel : targets)
			{
				unitLabel->runAction(Sequence::create(
					EaseSineIn::create(MoveBy::create(x / 300, Vec2(0, y))),
					EaseSineIn::create(MoveBy::create(0.5f - x / 300, Vec2(x + (type == FrameType::menu ? 0 : -moveX), 0))),
					NULL));
				x += unitLabel->getContentSize().width;
				y += unitLabel->getContentSize().height;
			}
		}
	};
	//long tap began equals tap
	unitListener->onLongTapBegan = [unitListener, type](Vec2 v)
	{
		unitListener->onTap(nullptr, nullptr);
	};
	// Swipe process
	unitListener->onSwipe = [this, target, targets, type, moveX, unitListener](Vec2 v, Vec2 diff, float time)
	{
		//Unless on frame, do nothing
		switch (type)
		{
		case FrameType::unit:	if (!getOnUnitFrame()) return; break;
		case FrameType::map:	if (!getOnMapFrame())  return; break;
		case FrameType::menu:	if (!getOnMenuFrame()) return; break;
		}
		// if action excuting, do nothing
		if (target->getNumberOfRunningActions() != 0)
			return;

		//If unhidable swipe, equals onTap
		if (isUnHidableBySwipe(type, diff))
			unitListener->onTap(nullptr, nullptr);
		//If hidable swipe, hide frame
		else if (isHidableBySwipe(type, diff))
		{
			target->runAction(EaseSineOut::create(MoveBy::create(0.5f, Vec2(-moveX, 0))));
			auto x = 0.0f;
			auto y = 0.0f;
			for (auto unitLabel : targets)
			{
				unitLabel->runAction(Sequence::create(
					EaseSineOut::create(MoveBy::create(0.5f - x / 300, Vec2((type == FrameType::menu ? 0 : moveX) - x, 0))),
					EaseSineOut::create(MoveBy::create(x / 300, Vec2(0, -y))),
					NULL));
				x += unitLabel->getContentSize().width;
				y += unitLabel->getContentSize().height;
			}
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(unitListener, target);
}

/*
 * Set menu item listener
 * if tap on target, call func
 */
void MenuLayer::setMenuListener(cocos2d::Node * target, std::function<void()> func)
{
	auto listener = SingleTouchListener::create();
	listener->setSwallowTouches(true);
	listener->onTouchBeganChecking = [](Touch* touch, Event* event)
	{
		return util::isTouchInEvent(touch, event);
	};
	listener->onTap = [func](Touch* touch, Event* event)
	{
		if(func)
			func();
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, target);
}

/*
 * Check whether action unlock onFrame
 */
bool MenuLayer::checkAction(Vec2 diff, FrameType type, Sprite *target, bool *onTarget, bool isMenu)
{
	if ((diff.x > 0 && !isMenu || diff.x < 0 && isMenu) && !isHided(type) && target->getNumberOfRunningActions() == 0)
		*onTarget = false;
	if ((diff.x < 0 && !isMenu || diff.x > 0 && isMenu) && isHided(type) && target->getNumberOfRunningActions() == 0)
		*onTarget = false;
	return !*onTarget;
}

/*
 * Check frame is hided
 */
bool MenuLayer::isHided(FrameType type)
{
	switch (type)
	{
	case FrameType::unit:	return _unit->getPosition().x < 0;
	case FrameType::map:	return _map->getPosition().x < 0;
	case FrameType::menu:	return _menu->getPosition().x > cocos2d::Director::getInstance()->getWinSize().width;
	case FrameType::info:	return _info->getPosition().x > cocos2d::Director::getInstance()->getWinSize().width;
	}
	return false;
}

/*
 * Check swipe makes frame hided
 */
bool MenuLayer::isHidableBySwipe(FrameType type, cocos2d::Vec2 diff)
{
	if (isHided(type))
		return false;

	switch (type)
	{
	case FrameType::unit:	return diff.x < 0;
	case FrameType::map:	return diff.x < 0;
	case FrameType::menu:	return diff.x > 0;
	case FrameType::info:	return diff.x > 0;
	}
	return false;
}

/*
 * Check swipe makes frame showing
 */
bool MenuLayer::isUnHidableBySwipe(FrameType type, cocos2d::Vec2 diff)
{
	if (!isHided(type))
		return false;

	switch (type)
	{
	case FrameType::unit:	return diff.x > 0;
	case FrameType::map:	return diff.x > 0;
	case FrameType::menu:	return diff.x < 0;
	case FrameType::info:	return diff.x < 0;
	}
	return false;
}