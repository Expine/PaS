#include "Menu.h"

#include "entity/EntityToTile.h"

#include "util/MultiListener.h"
#include "util/Util.h"

#define MODIFY 30

USING_NS_CC;

MenuLayer::MenuLayer()
	: _unit(nullptr)
	, _map(nullptr)
	, _menu(nullptr)
	, _info(nullptr)
	, _onUnitFrame(false)
	, _onMapFrame(false)
	, _onMenuFrame(false)
{
}

MenuLayer::~MenuLayer()
{
	_unit = nullptr;
	_map = nullptr;
	_info = nullptr;
	_menu = nullptr;
	_onUnitFrame = false;
	_onMapFrame = false;
	_onMenuFrame = false;
	_unitLabels.clear();
	_mapLabels.clear();
	_menuLabels.clear();
}

bool MenuLayer::init()
{
	if (!Node::init())
		return false;

	auto winSize = Director::getInstance()->getWinSize();

	_unit = Sprite::create();
	_unit->setTextureRect(Rect(0, 0, 250, 200));
	_unit->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_unit->setPosition(10, 230);
	_unit->setColor(Color3B(0, 148, 255));
	_unit->setOpacity(100);
	this->addChild(_unit);


	std::string unitName[] = { "U", "N", "I", "T" };
	for (std::string name : unitName)
	{
		auto unitLabel = Label::createWithSystemFont(name, "Arial", 20);
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

	_map = Sprite::create();
	_map->setTextureRect(Rect(0, 0, 250, 150));
	_map->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_map->setPosition(10, 20);
	_map->setColor(Color3B(0, 148, 255));
	_map->setOpacity(100);
	this->addChild(_map);

	std::string mapName[] = { "M", "A", "P" };
	for (std::string name : mapName)
	{
		auto mapLabel = Label::createWithSystemFont(name, "Arial", 20);
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


	_menu = Sprite::create();
	_menu->setTextureRect(Rect(0, 0, 200, 400));
	_menu->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	_menu->setPosition(winSize.width - 20, winSize.height - 20);
	_menu->setColor(Color3B(0, 148, 255));
	_menu->setOpacity(100);
	this->addChild(_menu);

	std::string menuName[] = { "M", "E", "N", "U" };
	for (std::string name : menuName)
	{
		auto menuLabel = Label::createWithSystemFont(name, "Arial", 20);
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

	return true;
}

void MenuLayer::setUnit()
{
}

void MenuLayer::setTile(StageTile * tile, Entity* unit)
{
	_map->removeChildByTag(0);
	_map->removeChildByTag(1);
	_map->removeChildByTag(2);
	_map->removeChildByTag(3);

	if (!tile)
		return;

	auto id = tile->getId();
	auto tileImage = Sprite::create("image/tileExp.png", Rect((id % 8) * 18, id / 8 * 18, 18, 18));
	tileImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	tileImage->setPosition(10, _map->getTextureRect().getMidY());
	tileImage->setScale(3.5f);
	tileImage->setTag(0);
	_map->addChild(tileImage);

	auto exp = Label::createWithSystemFont(tile->getExplanation(), "MS ゴシック", 20);
	exp->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	exp->setPosition(75, 120);
	exp->setColor(Color3B::BLACK);
	exp->setWidth(_map->getTextureRect().getMaxX() - 75 - MODIFY);
	exp->setTag(1);
	_map->addChild(exp);

	if (!unit)
		return;

	auto cost = Label::createWithSystemFont(StringUtils::format("Move cost : %d", EntityToTile::getInstance()->getSearchCost(tile->getTerrainType(), unit->getType())), "Arial", 18);
	cost->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	cost->setPosition(90, 25);
	cost->setColor(Color3B::BLACK);
	cost->setTag(2);
	_map->addChild(cost);

	auto effect = Label::createWithSystemFont(StringUtils::format("Effect : %d%%", EntityToTile::getInstance()->getEffect(tile->getTerrainType(), unit->getType())), "Arial", 18);
	effect->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	effect->setPosition(90, 5);
	effect->setColor(Color3B::BLACK);
	effect->setTag(3);
	_map->addChild(effect);
}

void MenuLayer::setUnit(Entity* unit)
{
	_unit->removeChildByTag(0);
	_unit->removeChildByTag(1);

	if (!unit)
		return;

	auto id = static_cast<int>(unit->getType());
	auto unitImage = Sprite::create("image/unit.png", Rect(0, id * 32, 32, 32));
	unitImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	unitImage->setPosition(10, _unit->getTextureRect().getMidY());
	unitImage->setScale(2.0f);
	unitImage->setTag(0);
	_unit->addChild(unitImage);

	auto exp = Label::createWithSystemFont(unit->getExplanation(), "MS ゴシック", 20);
	exp->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	exp->setPosition(75, 160);
	exp->setColor(Color3B::BLACK);
	exp->setWidth(_map->getTextureRect().getMaxX() - 75 - MODIFY);
	exp->setTag(1);
	_unit->addChild(exp);
}

void MenuLayer::setInfo(int x, int y)
{
	_info->removeChildByTag(0);
	_info->removeChildByTag(1);

	auto xLabel = Label::createWithSystemFont(StringUtils::format("X = %d", x), "Arial", 20);
	xLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	xLabel->setPosition(5, _info->getTextureRect().getMidY());
	xLabel->setTag(0);
	_info->addChild(xLabel);

	auto yLabel = Label::createWithSystemFont(StringUtils::format("Y = %d", y), "Arial", 20);
	yLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	yLabel->setPosition(80, _info->getTextureRect().getMidY());
	yLabel->setTag(1);
	_info->addChild(yLabel);
}

void MenuLayer::setFrameListener(Sprite *target, const std::vector<Label*>& targets, FrameType type, int moveX)
{
	auto unitListener = SingleTouchListener::create();
	unitListener->onTouchBeganChecking = [this, type](Touch* touch, Event* event)
	{
		switch(type)
		{
		case FrameType::unit:	setOnUnitFrame(util::isTouchInEvent(touch, event));	break;
		case FrameType::map:	setOnMapFrame(util::isTouchInEvent(touch, event));	break;
		case FrameType::menu:	setOnMenuFrame(util::isTouchInEvent(touch, event));	break;
		}
		return util::isTouchInEvent(touch, event);
	};
	unitListener->onTap = [this, target, targets, type, moveX](Touch* touch, Event* event)
	{
		switch (type)
		{
		case FrameType::unit:	if (!getOnUnitFrame()) return; break;
		case FrameType::map:	if (!getOnMapFrame())  return; break;
		case FrameType::menu:	if (!getOnMenuFrame()) return; break;
		}
		if (isHided(type))
		{
			std::vector<Label*> preLabels;
			target->runAction(EaseSineIn::create(MoveBy::create(0.5f, Vec2(moveX, 0))));
			for (auto unitLabel : targets)
			{
				auto x = 0.0f;
				auto y = 0.0f;
				for (auto preLabel : preLabels)
				{
					x += preLabel->getContentSize().width;
					y += preLabel->getContentSize().height;
				}
				if (type == FrameType::menu)
					unitLabel->runAction(Sequence::create(
						EaseSineIn::create(MoveBy::create(x / 300, Vec2(0, y))),
						EaseSineIn::create(MoveBy::create(0.5f - x / 300, Vec2(x, 0))),
						NULL));
				else
					unitLabel->runAction(Sequence::create(
						EaseSineIn::create(MoveBy::create(x / 300, Vec2(0, y))),
						EaseSineIn::create(MoveBy::create(0.5f - x / 300, Vec2(-moveX + x, 0))),
						NULL));

				preLabels.push_back(unitLabel);
			}
			preLabels.clear();
		}
	};
	unitListener->onLongTapBegan = [unitListener, type](Vec2 v)
	{
		unitListener->onTap(nullptr, nullptr);
	};
	unitListener->onSwipe = [this, target, targets, type, moveX, unitListener](Vec2 v, Vec2 diff, float time)
	{
		CCLOG("SWIPE : (%f) %d", diff.x, getOnUnitFrame());
		switch (type)
		{
		case FrameType::unit:	if (!getOnUnitFrame()) return; break;
		case FrameType::map:	if (!getOnMapFrame())  return; break;
		case FrameType::menu:	if (!getOnMenuFrame()) return; break;
		}
		if (target->getNumberOfRunningActions() == 0)
		{
			CCLOG("SWIPE : (%f) %d", diff.x, isHided(type));
			std::vector<Label*> preLabels;
			if (isUnHidableBySwipe(type, diff))
			{
				unitListener->onTap(nullptr, nullptr);
			}
			else if (isHidableBySwipe(type, diff))
			{
				target->runAction(EaseSineOut::create(MoveBy::create(0.5f, Vec2(-moveX, 0))));
				for (auto unitLabel : targets)
				{
					auto x = 0.0f;
					auto y = 0.0f;
					for (auto preLabel : preLabels)
					{
						x += preLabel->getContentSize().width;
						y += preLabel->getContentSize().height;
					}
					if (type == FrameType::menu)
						unitLabel->runAction(Sequence::create(
							EaseSineOut::create(MoveBy::create(0.5f - x / 300, Vec2(-x, 0))),
							EaseSineOut::create(MoveBy::create(x / 300, Vec2(0, -y))),
							NULL));
					else
						unitLabel->runAction(Sequence::create(
							EaseSineOut::create(MoveBy::create(0.5f - x / 300, Vec2(moveX - x, 0))),
							EaseSineOut::create(MoveBy::create(x / 300, Vec2(0, -y))),
							NULL));
					preLabels.push_back(unitLabel);
				}
			}
			preLabels.clear();
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(unitListener, target);
}