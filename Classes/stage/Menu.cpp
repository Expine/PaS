#include "Menu.h"
#include "Tile.h"
#include "Command.h"

#include "entity/EntityToTile.h"
#include "entity/Entity.h"
#include "entity/Weapon.h"
#include "util/MultiListener.h"
#include "util/Util.h"

#define EN_FONT "Arial"
#define JP_FONT "MS ゴシック"
#define FRAME "image/window_2.png"
#define COMMAND_FRAME "image/window_1.png"
#define INFO_SIZE 20
#define MENU_SIZE 30
#define MODIFY 40

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
	_unit = util::createCutSkin(FRAME, 300, 200, util::CUT_MASK_LEFT, 200);
	_unit->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	_unit->setPosition(0, winSize.height);
	this->addChild(_unit);


	// Set unit label
	for (std::string name : { "U", "N", "I", "T" })
	{
		auto unitLabel = Label::createWithSystemFont(name, EN_FONT, INFO_SIZE);
		unitLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		unitLabel->setColor(Color3B::BLACK);
		if (_unitLabels.size() == 0)
			unitLabel->setPosition(5, _unit->getContentSize().height - 5);
		else
			unitLabel->setPosition(_unitLabels.back()->getPosition().x + _unitLabels.back()->getContentSize().width, _unit->getContentSize().height - 5);
		unitLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		_unit->addChild(unitLabel);
		_unitLabels.push_back(unitLabel);
	}

	// Set map frame
	_map = util::createCutSkin(FRAME, 300, 110, util::CUT_MASK_LEFT, 200);
	_map->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_map->setPosition(0, 0);
	this->addChild(_map);

	// Set map label
	for (std::string name : { "M", "A", "P" })
	{
		auto mapLabel = Label::createWithSystemFont(name, EN_FONT, INFO_SIZE);
		mapLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		mapLabel->setColor(Color3B::BLACK);
		if (_mapLabels.size() == 0)
			mapLabel->setPosition(5, _map->getContentSize().height - 5);
		else
			mapLabel->setPosition(_mapLabels.back()->getPosition().x + _mapLabels.back()->getContentSize().width, _map->getContentSize().height - 5);
		mapLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		_map->addChild(mapLabel);
		_mapLabels.push_back(mapLabel);
	}

	// Set menu frame
	_menu = Sprite::create();
	_menu = util::createCutSkin(FRAME, 220, 460, util::CUT_MASK_RIGHT, 200);
	_menu->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	_menu->setPosition(winSize.width, winSize.height);
	this->addChild(_menu);

	// Set menu label
	for (std::string name : { "M", "E", "N", "U" })
	{
		auto menuLabel = Label::createWithSystemFont(name, EN_FONT, INFO_SIZE);
		menuLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
		menuLabel->setColor(Color3B::BLACK);
		if (_menuLabels.size() == 0)
			menuLabel->setPosition(5, _menu->getContentSize().height - 5);
		else
			menuLabel->setPosition(_menuLabels.back()->getPosition().x + _menuLabels.back()->getContentSize().width, _menu->getContentSize().height - 5);
		menuLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		_menu->addChild(menuLabel);
		_menuLabels.push_back(menuLabel);
	}

	// Set menu item
	for (auto name : { u8"フェイズ終了", u8"次の都市へ", u8"次の部隊へ", u8"参謀と話す", u8"セーブ", u8"ロード", u8"オプション" })
	{
		static Label* preItem = nullptr;
		auto item = Label::createWithSystemFont(name, JP_FONT, MENU_SIZE);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		item->setColor(Color3B::BLACK);
		item->setPosition(_menu->getContentSize().width - 10, preItem ? preItem->getPosition().y - 60 : _menu->getContentSize().height - 50);
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

	// Unit command
	forUnit(i)
	{
		auto name = command::getName(castUnit(i));
		auto command = setCommand(name, -80, _unit->getPosition().y - _unit->getContentSize().height, 80, 40);
		_unit_command[castUnit(i)] = command;
	}

	// City command
	forCity(i)
	{
		auto name = command::getName(castCity(i));
		auto command = setCommand(name, -80, _map->getContentSize().height, 80, 40);
		command->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		_city_command[castCity(i)] = command;
	}

	// Set Move command
	forMove(i)
	{
		auto name = command::getName(castMove(i));
		auto command = setCommand(name, -80, _unit->getPosition().y - _unit->getContentSize().height, 80, 40);
		_move_command[castMove(i)] = command;
	}
	forMove2(i)
	{
		auto name = command::getName(castMove(i));
		auto command = setCommand(name, -80, _unit->getPosition().y - _unit->getContentSize().height, 80, 40);
		_move_command[castMove(i)] = command;
	}

	// Set enemy unit frame
	_enemy_unit = Sprite::create();
	_enemy_unit = util::createCutSkin(FRAME, 300, 200, util::CUT_MASK_RIGHT, 255);
	_enemy_unit->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	_enemy_unit->setPosition(winSize.width + _enemy_unit->getContentSize().width, winSize.height);
	this->addChild(_enemy_unit);

	// Set enemy unit label
	auto enemy_label = Label::createWithSystemFont("ENEMY", EN_FONT, INFO_SIZE);
	enemy_label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	enemy_label->setPosition(0, _enemy_unit->getContentSize().height);
	enemy_label->setColor(Color3B::RED);
	_enemy_unit->addChild(enemy_label);

	// Set listener
	setFrameListener(_unit, _unitLabels, FrameType::unit, 300 - MODIFY);
	setFrameListener(_map, _mapLabels, FrameType::map, 300 - MODIFY);
	setFrameListener(_menu, _menuLabels, FrameType::menu, -220 + MODIFY);

	setMenuListener(_menu->getChildByTag(10), [this] { if (endPhase)  endPhase(); }, false);
	setMenuListener(_menu->getChildByTag(11), [this] { if (nextCity)  nextCity(); }, false);
	setMenuListener(_menu->getChildByTag(12), [this] { if (nextUnit)  nextUnit(); }, false);
	setMenuListener(_menu->getChildByTag(13), [this] { if (talkStaff) talkStaff(); }, false);
	setMenuListener(_menu->getChildByTag(14), [this] { if (save)	  save(); }, false);
	setMenuListener(_menu->getChildByTag(15), [this] { if (load)	  load(); }, false);
	setMenuListener(_menu->getChildByTag(16), [this] { if (option)	  option(); }, false);

	forUnit(i)
		setMenuListener(_unit_command[castUnit(i)], [this, i] {if (_unit_function[castUnit(i)]) _unit_function[castUnit(i)](); });

	forCity(i)
		setMenuListener(_city_command[castCity(i)], [this, i] {if (_city_function[castCity(i)]) _city_function[castCity(i)](); });

	forMove(i)
		setMenuListener(_move_command[castMove(i)], [this, i] {if (_move_function[castMove(i)]) _move_function[castMove(i)](); });

	forMove2(i)
		setMenuListener(_move_command[castMove(i)], [this, i] {if (_move_function[castMove(i)]) _move_function[castMove(i)](); });

	return true;
}

/*
 * Set tile information
 */
void MenuLayer::setTile(std::vector<StageTile*> tiles, Entity* unit)
{
	// Remove pre-information
	if (_map->getChildByTag(0) != NULL)
	{
		_map->removeChildByTag(0);
		_map->removeChildByTag(1);
	}

	// Get last tile
	StageTile* tile = nullptr;
	for (auto t : tiles)
		if (t->getId() != 0)
			tile = t;

	if (!tile)
		return;

	// Set tile image
	auto id = tile->getId();
	auto tileImage = Sprite::create("image/tileExp.png", Rect((id % 8) * 18, id / 8 * 18, 18, 18));
	tileImage->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	tileImage->setPosition(10, 10);
	tileImage->setScale(3.5f);
	tileImage->setTag(0);
	_map->addChild(tileImage);

	// Set name
	auto name= Label::createWithSystemFont(TileInformation::getInstance()->getName(tile->getTerrainType()), JP_FONT, INFO_SIZE);
	name->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	name->setPosition(80, 90);
	name->setColor(Color3B::BLACK);
	name->setTag(1);
	_map->addChild(name);

	setUnitToTile(tiles, unit);

	if (util::instanceof<City>(tile) && util::instance<City>(tile)->getOwner() == Owner::player)
		showCityCommand(util::instance<City>(tile));
	else
		hideCityCommand();
}

/*
 * Set unit information
 */
void MenuLayer::setUnit(Node * target, std::vector<StageTile*> tiles, Entity * unit)
{
	// Remove pre-information
	if (target->getChildByTag(0) != NULL)
	{
		target->removeChildByTag(0);
		target->removeChildByTag(1);
		target->removeChildByTag(2);
		target->removeChildByTag(3);
		target->removeChildByTag(4);
		target->removeChildByTag(5);
		target->removeChildByTag(6);
	}

	if (!unit)
	{
		hideUnitCommand();
		return;
	}

	// Set unit image
	auto unitImage = Sprite::create("image/unit.png", Rect(0, static_cast<int>(unit->getType()) * 32, 32, 32));
	unitImage->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	unitImage->setPosition(10, target->getContentSize().height / 2);
	unitImage->setScale(2.5f);
	unitImage->setTag(0);
	target->addChild(unitImage);

	// Set unit color
	auto color = Sprite::create();
	color->setTextureRect(Rect(0, 0, 30, 30));
	color->setColor(OwnerInformation::getInstance()->getColor(unit->getAffiliation()));
	color->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	color->setPosition(75, 180);
	color->setTag(1);
	target->addChild(color);

	// Set unit name
	auto name = Label::createWithSystemFont(unit->getName(), JP_FONT, INFO_SIZE);
	name->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	name->setPosition(110, 165);
	name->setColor(Color3B::BLACK);
	name->setTag(2);
	target->addChild(name);


	// Set unit ability
	auto count = 0;
	for (auto i : {
		StringUtils::format(u8"残存兵力 %d / %d", unit->getDurability(), unit->getMaxDurability()),
		StringUtils::format(u8"残存物資 %d / %d", unit->getMaterial(), unit->getMaxMaterial()),
		StringUtils::format(u8"索敵能力 %d", unit->getSearchingAbility()),
		StringUtils::format(u8"移動能力 %d", unit->getMobility())
	})
	{
		auto item = Label::createWithSystemFont(i, JP_FONT, INFO_SIZE);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		item->setPosition(90, 140 - count * (INFO_SIZE + 10));
		item->setColor(Color3B::BLACK);
		item->setTag(count + 3);
		target->addChild(item);
		count++;
	}

	if(!tiles.empty())
		setUnitToTile(tiles, unit);

	if (unit->getAffiliation() == Owner::player)
		showUnitCommand(unit, tiles);
	else
		hideUnitCommand();
}

/*
 * Set info data
 */
void MenuLayer::setInfo(int x, int y)
{
	// Remove pre-information
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
 * Set unit to tile data
 */
void MenuLayer::setUnitToTile(std::vector<StageTile*> tiles, Entity *unit)
{
	// Remove pre-information
	if (_map->getChildByTag(2) != NULL)
	{
		_map->removeChildByTag(2);
		_map->removeChildByTag(3);
	}

	if (tiles.size() == 0)
		return;

	if (!unit)
		return;

	auto search_cost = 0;
	auto effect_value = 0;
	for (auto tile : tiles)
	{
		search_cost += EntityToTile::getInstance()->getSearchCost(tile->getTerrainType(), unit->getType());
		effect_value += EntityToTile::getInstance()->getEffect(tile->getTerrainType(), unit->getType());
	}

	// Set move cost
	auto cost = Label::createWithSystemFont(StringUtils::format(u8"移動コスト : %2d", search_cost), JP_FONT, INFO_SIZE);
	cost->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	cost->setPosition(90, 45);
	cost->setColor(Color3B::BLACK);
	cost->setTag(2);
	_map->addChild(cost);

	// Set effect
	auto effect = Label::createWithSystemFont(StringUtils::format(u8"地形効果  : %d %%", effect_value), JP_FONT, INFO_SIZE);
	effect->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	effect->setPosition(90, 15);
	effect->setColor(Color3B::BLACK);
	effect->setTag(3);
	_map->addChild(effect);
}

/*
 * Show unit command
 * If already showed, do nothing
 */
void MenuLayer::showUnitCommand(Entity* entity, std::vector<StageTile*> tiles, bool movable)
{
	if (_mode == MenuMode::unit)
	{
		forUnit(i)
			if (!EntityInformation::getInstance()->getCommand(entity->getType(), castUnit(i)))
				_unit_command[castUnit(i)]->setColor(Color3B::GRAY);
			else if(command::isEnable(castUnit(i), entity, tiles))
				_unit_command[castUnit(i)]->setColor(Color3B::WHITE);
			else
				_unit_command[castUnit(i)]->setColor(Color3B::GRAY);
	}
	else if (_mode == MenuMode::move)
	{
		_unit_command[UnitCommand::move]->setColor(Color3B::GRAY);
		forMove(i)
			if (castMove(i) == MoveCommand::start && !movable)
				_move_command[castMove(i)]->setColor(Color3B::GRAY);
			else if(command::isEnable(castMove(i), entity, tiles))
				_move_command[castMove(i)]->setColor(Color3B::WHITE);
			else
				_move_command[castMove(i)]->setColor(Color3B::GRAY);
	}
	else if (_mode == MenuMode::moving)
	{
	}

	if (_isShowedUnitCommand)
		return;

	moveUnitCommand();

	_isShowedUnitCommand = true;
}

/*
 * Move unit command
 * Only move
 */
void MenuLayer::moveUnitCommand()
{
	if (_mode == MenuMode::unit)
	{
		forUnit(i)
			showUnitCommandByOne(i % 3, i / 3, _unit_command[castUnit(i)]);
	}
	else if (_mode == MenuMode::move)
	{
		showUnitCommandByOne(0, 0, _unit_command[UnitCommand::move]);
		forMove(i)
			showUnitCommandByOne(0 + i, 1, _move_command[castMove(i)]);
	}
	else if (_mode == MenuMode::moving)
	{
		showUnitCommandByOne(0, 0, _unit_command[UnitCommand::move]);
		showUnitCommandByOne(0, 1, _unit_command[UnitCommand::attack]);
		forMove2(i)
			showUnitCommandByOne(1 + i - static_cast<int>(MoveCommand::decision), 1, _move_command[castMove(i)]);
	}
}

/*
 * Hide unit command
 * If already hided, do nothing
 */
void MenuLayer::hideUnitCommand()
{
	if (!_isShowedUnitCommand)
		return;

	if (_mode == MenuMode::unit)
	{
		forUnit(i)
			hideUnitCommandByOne(_unit_command[castUnit(i)]);
	}
	else if (_mode == MenuMode::move)
	{
		hideUnitCommandByOne(_unit_command[UnitCommand::move]);
		hideUnitCommandByOne(_unit_command[UnitCommand::attack]);
		forMove(i)
			hideUnitCommandByOne(_move_command[castMove(i)]);
	}
	else if (_mode == MenuMode::moving)
	{
		hideUnitCommandByOne(_unit_command[UnitCommand::move]);
		hideUnitCommandByOne(_unit_command[UnitCommand::attack]);
		forMove2(i)
			hideUnitCommandByOne(_move_command[castMove(i)]);
	}
	_isShowedUnitCommand = false;
}

/*
 * Showing unit command one by one
 */
void MenuLayer::showUnitCommandByOne(int x, int y, Node* command)
{
	command->stopAllActions();
	if (isHided(FrameType::unit))
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit->getPosition().y - _unit->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(MODIFY + 10 + y * (command->getContentSize().width + 20), -20 + _unit->getPosition().y - x * (command->getContentSize().height + 15)))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit->getPosition().y - _unit->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(0, _unit->getPosition().y) + Vec2(10 + x * (command->getContentSize().width + 20), -y * (command->getContentSize().height + 15) - _unit->getContentSize().height))),
			NULL));
}

/*
* Hiding unit command one by one
*/
void MenuLayer::hideUnitCommandByOne(Node* command)
{
	command->stopAllActions();
	if (isHided(FrameType::unit))
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit->getPosition().y - _unit->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _unit->getPosition().y - _unit->getContentSize().height))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit->getPosition().y - _unit->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _unit->getPosition().y - _unit->getContentSize().height))),
			NULL));
}

/*
 * Show city command
 * If already showed, do nothing
 */
void MenuLayer::showCityCommand(City* city)
{
	forCity(i)
	{
		if(TileInformation::getInstance()->getCommand(city->getTerrainType(), castCity(i)))
			_city_command[castCity(i)]->setColor(Color3B(255, 255, 255));
		else
			_city_command[castCity(i)]->setColor(Color3B(128, 128, 128));
	}

	if (_isShowedCityCommand)
		return;

	moveCityCommand();

	_isShowedCityCommand = true;
}

/*
 * Move city command
 * Only move
 */
void MenuLayer::moveCityCommand()
{
	forCity(i)
	{
		auto com = _city_command[castCity(i)];
		com->stopAllActions();
		if (isHided(FrameType::map))
			com->runAction(Sequence::create(
				EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
				EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(MODIFY + 10 + (i / 2) * (com->getContentSize().width + 20), _map->getPosition().y + _map->getContentSize().height - (com->getContentSize().height + 10) - (i % 2) * (com->getContentSize().height + 15)))),
			NULL));
		else
			com->runAction(Sequence::create(
				EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
				EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(0, _map->getPosition().y) + Vec2(10 + (i % 3) * (com->getContentSize().width + 20), i / 3 * (com->getContentSize().height + 15) + _map->getContentSize().height))),
			NULL));
	}
}

/*
 * Hide city command
 * If already hided, do nothing
 */
void MenuLayer::hideCityCommand()
{
	if (!_isShowedCityCommand)
		return;

	forCity(i)
	{
		auto com = _city_command[castCity(i)];
		com->stopAllActions();
		if(isHided(FrameType::map))
			com->runAction(Sequence::create(
				EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
				EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-com->getContentSize().width, _map->getContentSize().height))),
			NULL));
		else
			com->runAction(Sequence::create(
				EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
				EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-com->getContentSize().width, _map->getContentSize().height))),
			NULL));
	}
	_isShowedCityCommand = false;
}

/*
 * Set command frame and command
 */
Node* MenuLayer::setCommand(const std::string & name, const int x, const int y, const int width, const int height)
{
	auto command_frame = util::createCutSkin(COMMAND_FRAME, width, height, 0);
	command_frame->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	command_frame->setPosition(Vec2(x, y));
	command_frame->setCascadeColorEnabled(true);
	command_frame->setCascadeOpacityEnabled(true);
	this->addChild(command_frame);

	auto command = Label::createWithSystemFont(name, JP_FONT, MENU_SIZE);
	command->setPosition(command_frame->getContentSize() / 2);
	command_frame->addChild(command);

	return command_frame;
}

/*
 * Set frame listener
 */
void MenuLayer::setFrameListener(Node *target, const std::vector<Label*>& targets, FrameType type, int moveX)
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
			if (type == FrameType::map && _isShowedCityCommand)
				moveCityCommand();
			if (type == FrameType::unit &&_isShowedUnitCommand)
				moveUnitCommand();
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
			if (type == FrameType::map && _isShowedCityCommand)
				moveCityCommand();
			if (type == FrameType::unit &&_isShowedUnitCommand)
				moveUnitCommand();
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(unitListener, target);
}

/*
 * Set menu item listener
 * if tap on target, call func
 */
void MenuLayer::setMenuListener(cocos2d::Node * target, std::function<void()> func, bool isWhiteEnable)
{
	auto listener = SingleTouchListener::create();
	listener->setSwallowTouches(true);
	listener->onTouchBeganChecking = [](Touch* touch, Event* event)
	{
		return util::isTouchInEvent(touch, event);
	};
	listener->onTap = [target, func, isWhiteEnable](Touch* touch, Event* event)
	{
		if(!isWhiteEnable || target->getColor() == Color3B::WHITE)
			if(func)
				func();
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, target);
}

/*
 * Check whether action unlock onFrame
 */
bool MenuLayer::checkAction(Vec2 diff, FrameType type, Node *target, bool *onTarget, bool isMenu)
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
	case FrameType::unit:	return (_unit->getPosition().x < 0) ^ (_unit->getNumberOfRunningActions() != 0);
	case FrameType::map:	return (_map->getPosition().x < 0) ^ (_map->getNumberOfRunningActions() != 0);
	case FrameType::menu:	return (_menu->getPosition().x > cocos2d::Director::getInstance()->getWinSize().width) ^ (_menu->getNumberOfRunningActions() != 0);
	case FrameType::info:	return (_info->getPosition().x > cocos2d::Director::getInstance()->getWinSize().width) ^ (_info->getNumberOfRunningActions() != 0);
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

/*
 * Set menu mode
 */
void MenuLayer::setMenuMode(MenuMode mode, Entity *unit, std::vector<StageTile*> tiles, bool movable)
{
	if (_mode != mode)
	{
		hideUnitCommand();
		_mode = mode;
		showUnitCommand(unit, tiles, movable);
		_isShowedUnitCommand = true;
	}
	else
	{
		showUnitCommand(unit, tiles, movable);
	}
}

/*
 * Show enemy unit
 */
void MenuLayer::showEnemyUnit(Entity * enemy)
{
	auto winSize = Director::getInstance()->getWinSize();
	setUnit(_enemy_unit, std::vector<StageTile*>(), enemy);
	_enemy_unit->runAction(EaseSineIn::create(MoveTo::create(0.5f, Vec2(winSize.width, winSize.height))));
}

/*
 * Hide enemy unit
 */
void MenuLayer::hideEnemyUnit()
{
	auto winSize = Director::getInstance()->getWinSize();
	_enemy_unit->runAction(EaseSineOut::create(MoveTo::create(0.5f, Vec2(winSize.width + _enemy_unit->getContentSize().width, winSize.height))));
}

/*
 * Show weapon frame
 */
void MenuLayer::showWeaponFrame(Entity* unit)
{
	auto winSize = Director::getInstance()->getWinSize();
	auto h = unit->getWeaponsByRef().size() * 50 + 30 + 30;

	// Set weapon frame
	auto weapon = util::createCutSkin(FRAME, 700, h, 0, 255);
	weapon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	weapon->setPosition(winSize.width / 2, 0);
	weapon->setTag(1000);
	this->addChild(weapon);

	// Set info
	auto height = weapon->getContentSize().height - 10 - INFO_SIZE;
	auto name = Label::createWithSystemFont(u8"装備名", JP_FONT, INFO_SIZE);
	name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	name->setPosition(70, height);
	weapon->addChild(name);
	auto x = 160;
	for (auto item : { u8"対人", u8"対魔", u8"対炎", u8"対氷", u8"対雷", u8"対土", u8"命中", u8"範囲" })
	{
		if(item == u8"範囲")
			x += 30;
		auto label = Label::createWithSystemFont(item, JP_FONT, INFO_SIZE);
		label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		label->setPosition(x, height);
		weapon->addChild(label);
		x += 60;
	}

	// Set weapon data
	auto no = 0;
	for (auto weapon : unit->getWeaponsByRef())
		if(weapon)
			renderWeapon(weapon, no++);

	// Set frame
	auto frame = util::createCutSkinAndAnimation("image/frame_1.png", 680, 40, 6, 1, 0, 0.15f);
	frame->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	frame->setPosition(weapon->getContentSize().width / 2, weapon->getContentSize().height - 15 - MENU_SIZE - 30);
	weapon->addChild(frame);

	// Set decision frame
	auto decision = util::createCutSkin(COMMAND_FRAME, 50, h, 0, 200);
	decision->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	decision->setPosition(-25, 0);
	weapon->addChild(decision);
	auto decision_label = Label::createWithSystemFont(u8"確定", JP_FONT, MENU_SIZE);
	decision_label->setWidth(MENU_SIZE);
	decision_label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	decision_label->setPosition(decision->getContentSize() / 2);
	decision->addChild(decision_label);

	// Set cancel frame
	auto cancel = util::createCutSkin(COMMAND_FRAME, 50, h, 0, 200);
	cancel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	cancel->setPosition(weapon->getContentSize().width + 25, 0);
	weapon->addChild(cancel);
	auto cancel_label = Label::createWithSystemFont(u8"撤回", JP_FONT, MENU_SIZE);
	cancel_label->setWidth(MENU_SIZE);
	cancel_label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	cancel_label->setPosition(cancel->getContentSize() / 2);
	cancel->addChild(cancel_label);

	// Set listener
	auto lis = SingleTouchListener::create();
	lis->setSwallowTouches(true);
	lis->onTap = [this, weapon, frame, decision, cancel](Touch* touch, Event* event)
	{
		if (util::isTouchInEvent(touch->getLocation(), weapon))
		{
			auto pos = touch->getLocation() - (weapon->getPosition() - Vec2(weapon->getContentSize().width / 2, 0));
			int no = (weapon->getContentSize().height - pos.y - 15 - 30 - MENU_SIZE / 2) / 50;
			if (no >= 0)
			{
				frame->stopAllActions();
				frame->runAction(MoveTo::create(0.2f, Vec2(weapon->getContentSize().width / 2, weapon->getContentSize().height - 15 - MENU_SIZE - 30 - no * 50)));
			}
		}
		if (util::isTouchInEvent(touch->getLocation(), decision))
		{
			hideWeaponFrame();
			hideEnemyUnit();
		}
		if (util::isTouchInEvent(touch->getLocation(), cancel))
		{
			hideWeaponFrame();
			hideEnemyUnit();
		}
	};
	lis->onSwipe = [weapon](Vec2 v, Vec2 diff, float time)
	{
		auto winSize = Director::getInstance()->getWinSize();
		auto size = weapon->getContentSize();
		auto pos = weapon->getPosition() + diff;
		pos.x = (pos.x < size.width / 2 + 50) ? size.width / 2 + 50 : (pos.x > winSize.width - size.width / 2 - 50) ? winSize.width - size.width / 2 - 50 : pos.x;
		pos.y = (pos.y < 0) ? 0 : (pos.y > winSize.height - size.height) ? winSize.height - size.height : pos.y;
		weapon->setPosition(pos);
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, weapon);
}

/*
 * Render a weapon information
 */
void MenuLayer::renderWeapon(WeaponData * weapon, int no)
{
	auto frame = this->getChildByTag(1000);
	auto height = frame->getContentSize().height - 15 - 50 * no - MENU_SIZE - 30;

	// Set name
	auto name = Label::createWithSystemFont(weapon->getName(), JP_FONT, MENU_SIZE);
	name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	name->setPosition(70, height);
	frame->addChild(name);

	// Set info
	auto x = 160;
	for (auto item : { weapon->getAntiPersonnel(), weapon->getAntiWizard(), weapon->getAntiFire(), weapon->getAntiIce(), weapon->getAntiThunder(), weapon->getAntiGround(), weapon->getAccuracy() })
	{
		auto label = Label::createWithSystemFont(StringUtils::format("%d", item), EN_FONT, INFO_SIZE);
		label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		label->setPosition(x, height);
		frame->addChild(label);
		x += 60;
	}

	// Set range
	x += 30;
	auto range = Label::createWithSystemFont(WeaponInformation::getInstance()->getRangeName(weapon->getRange()), JP_FONT, INFO_SIZE);
	range->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	range->setPosition(x, height);
	frame->addChild(range);
}

void MenuLayer::hideWeaponFrame()
{
	this->getChildByTag(1000)->removeFromParentAndCleanup(true);
}
