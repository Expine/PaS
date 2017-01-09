#include "Menu.h"
#include "Tile.h"
#include "Command.h"
#include "Stage.h"

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

constexpr int DURABILITY_SPLIT = 20;

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
	command::forMenu([this] (Command com, int i){
		auto item = Label::createWithSystemFont(command::getName(com), JP_FONT, MENU_SIZE);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		item->setColor(Color3B::BLACK);
		item->setPosition(_menu->getContentSize().width - 10, _menu->getContentSize().height - 50 - 60 * i);
		item->setTag(10 + (i - static_cast<int>(Command::MENU_START) - 1));
		_menu->addChild(item);
		_commands[com] = item;
	});

	// Set info frame
	_info = Sprite::create();
	_info->setTextureRect(Rect(0, 0, 350, 40));
	_info->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	_info->setPosition(winSize.width, 0);
	_info->setColor(Color3B(0, 0, 0));
	_info->setOpacity(100);
	this->addChild(_info);

	// Unit command
	command::forAllUnit([this] (Command com, int i)
	{
		_commands[com] = setCommand(command::getName(com), -80, _unit->getPosition().y - _unit->getContentSize().height, 80, 40); 
	});

	// City command
	command::forAllCity([this](Command com, int i) 
	{
		_commands[com] = setCommand(command::getName(com), -80, _map->getContentSize().height, 80, 40);
		_commands[com]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	});

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

	command::forMenu([this] (Command com, int i)
	{
		setMenuListener(_commands[com], [this, com] {if (_functions[com]) _functions[com](); }, false);
	});

	command::forAll([this](Command com, int i) 
	{
		setMenuListener(_commands[com], [this, com] {if (_functions[com]) _functions[com](); });
	});

	return true;
}

/*
 * Set tile information
 */
void MenuLayer::setTile(std::vector<StageTile*> tiles, Entity* unit)
{
	// Remove pre-information
	if (_map->getChildByTag(0))
	{
		_map->removeChildByTag(0);
		_map->removeChildByTag(1);
	}
	if (_map->getChildByTag(2))
		_map->removeChildByTag(2);

	// Get last tile
	StageTile* tile = nullptr;
	for (auto t : tiles)
		if (t && t->getId() != 0)
			tile = t;

	// Check null
	if (!tile)
	{
		hideCityCommand();
		return;
	}

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

	// Set durability
	if (util::instanceof<City>(tile))
	{
		auto node = Node::create();
		node->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		node->setContentSize(Size(140, INFO_SIZE));
		node->setPosition(145, 90);
		node->setTag(2);
		_map->addChild(node);
		for (int i = 0; i < DURABILITY_SPLIT; i++)
		{
			auto sq = Sprite::create();
			sq->setTextureRect(Rect(0, 0, 5, INFO_SIZE));
			sq->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			sq->setPosition(7 * i, 0);
			sq->setColor(util::instance<City>(tile)->getDurability() > i * util::instance<City>(tile)->getMaxDurability() / DURABILITY_SPLIT ? Color3B::WHITE : Color3B::GRAY);
			node->addChild(sq);
		}
	}

	// Set unit to tile information
	setUnitToTile(tiles, unit);

	// Show city command
	if (util::instanceof<City>(tile) && util::instance<City>(tile)->getOwner() == Owner::player)
	{
		checkCityCommand(unit, tiles, util::instance<City>(tile));
		moveCityCommand();
	}
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

	// Check null
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

	// Set unit to tile information
	setUnitToTile(tiles, unit);

	// If unit frame, check command
	if (target == _unit)
	{
		checkUnitCommand(unit, tiles);
		moveUnitCommand();
	}
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
	if (_map->getChildByTag(3))
	{
		_map->removeChildByTag(3);
		_map->removeChildByTag(4);
	}

	// Check empty and null
	if (tiles.empty() || !unit)
		return;

	// Calculate cost and effect
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
	cost->setTag(3);
	_map->addChild(cost);

	// Set effect
	auto effect = Label::createWithSystemFont(StringUtils::format(u8"地形効果  : %d %%", effect_value), JP_FONT, INFO_SIZE);
	effect->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	effect->setPosition(90, 15);
	effect->setColor(Color3B::BLACK);
	effect->setTag(4);
	_map->addChild(effect);
}

/*
 * Check unit command enable
 */
void MenuLayer::checkUnitCommand(Entity *entity, std::vector<StageTile*> tiles, bool able)
{
	std::function<void(std::function<void(Command, int)>)> func = nullptr;
	switch (_mode)
	{
	case MenuMode::none:
		func = command::forUnit;
		break;
	case MenuMode::move:
		_commands[Command::move]->setColor(Color3B::GRAY);
		command::forMove([this, entity, tiles, able] (Command com, int i)
		{
			// If move start, check movable
			if (com == Command::move_start && !able)
				_commands[com]->setColor(Color3B::GRAY);
			// Whether this command is enable
			else if (command::isEnable(com, entity, tiles))
				_commands[com]->setColor(Color3B::WHITE);
			else
				_commands[com]->setColor(Color3B::GRAY);
		});
		break;
	case MenuMode::moving:
	{
		if (entity->isAttakable())
			_commands[Command::attack]->setColor(Color3B::WHITE);
		else
			_commands[Command::attack]->setColor(Color3B::GRAY);
		break;
	}
	case MenuMode::attack:
		_commands[Command::attack]->setColor(Color3B::GRAY);
		command::forAttack([this, entity, tiles, able] (Command com, int i)
		{
			// check targetable
			if (com == Command::attack_target && !able)
				_commands[com]->setColor(Color3B::GRAY);
			// Whether this command is enable
			else if (command::isEnable(com, entity, tiles))
				_commands[com]->setColor(Color3B::WHITE);
			else
				_commands[com]->setColor(Color3B::GRAY);
		});
		break;
	case MenuMode::attacking:
		break;
	case MenuMode::occupy:
		_commands[Command::occupation]->setColor(Color3B::GRAY);
		func = command::forOccupation;
		break;
	case MenuMode::wait:
		_commands[Command::wait]->setColor(Color3B::GRAY);
		func = command::forWait;
		break;
	default:
		break;
	}
	if(func)
		func([this, entity, tiles](Command com, int i)
		{
			// Whether this command is enable
			if (command::isEnable(com, entity, tiles))
				_commands[com]->setColor(Color3B::WHITE);
			else
				_commands[com]->setColor(Color3B::GRAY);
		});
}

/*
 * Move unit command
 * Only move
 */
void MenuLayer::moveUnitCommand()
{
	if (!isUnitMenuModeAble())
	{
		hideUnitCommand();
		return;
	}

	if (_isShowedUnitCommand)
		return;

	switch (_mode)
	{
	case MenuMode::none:
		command::forUnit([this] (Command com, int i)
		{
			showUnitCommandByOne(i % 3, i / 3, _commands[com]);
		});
		break;
	case MenuMode::move:
		showUnitCommandByOne(0, 0, _commands[Command::move]);
		command::forMove([this](Command com, int i) 
		{
			showUnitCommandByOne(0 + i, 1, _commands[com]);
		});
		break;
	case MenuMode::moving:
		showUnitCommandByOne(0, 0, _commands[Command::move]);
		showUnitCommandByOne(0, 1, _commands[Command::attack]);
		command::forMove2([this](Command com, int i) 
		{
			showUnitCommandByOne(i + 1, 1, _commands[com]);
		});
		break;
	case MenuMode::attack:
		showUnitCommandByOne(0, 0, _commands[Command::attack]);
		command::forAttack([this](Command com, int i) 
		{
			showUnitCommandByOne(i, 1, _commands[com]);
		});
		break;
	case MenuMode::attacking:
		showUnitCommandByOne(0, 0, _commands[Command::attack]);
		showUnitCommandByOne(2, 1, _commands[Command::attack_end]);
		command::forAttack2([this] (Command com, int i)
		{
			showUnitCommandByOne(i, 1, _commands[com]);
		});
		break;
	case MenuMode::occupy:
		showUnitCommandByOne(0, 0, _commands[Command::occupation]);
		command::forOccupation([this](Command com, int i)
		{
			showUnitCommandByOne(i, 1, _commands[com]);
		});
		break;
	case MenuMode::wait:
		showUnitCommandByOne(0, 0, _commands[Command::wait]);
		command::forWait([this](Command com, int i)
		{
			showUnitCommandByOne(i, 1, _commands[com]);
		});
		break;
	}

	_isShowedUnitCommand = true;
}

/*
 * Hide unit command
 * If already hided, do nothing
 */
void MenuLayer::hideUnitCommand()
{
	if (!_isShowedUnitCommand)
		return;

	std::function<void(std::function<void(Command, int)>)> func;

	switch (_mode)
	{
	case MenuMode::none:
		func = command::forUnit;
		break;
	case MenuMode::move:
		hideUnitCommandByOne(_commands[Command::move]);
		hideUnitCommandByOne(_commands[Command::attack]);
		func = command::forMove;
		break;
	case MenuMode::moving:
		hideUnitCommandByOne(_commands[Command::move]);
		hideUnitCommandByOne(_commands[Command::attack]);
		func = command::forMove2;
		break;
	case MenuMode::attack:
		hideUnitCommandByOne(_commands[Command::attack]);
		func = command::forAttack;
		break;
	case MenuMode::attacking:
		hideUnitCommandByOne(_commands[Command::attack]);
		hideUnitCommandByOne(_commands[Command::attack_end]);
		func = command::forAttack2;
		break;
	case MenuMode::occupy:
		hideUnitCommandByOne(_commands[Command::occupation]);
		func = command::forOccupation;
		break;
	case MenuMode::wait:
		hideUnitCommandByOne(_commands[Command::wait]);
		func = command::forWait;
		break;
	}

	// Hide supecify commands
	func([this](Command com, int i)
	{
		hideUnitCommandByOne(_commands[com]);
	});

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
 * Check city command enable
 */
void MenuLayer::checkCityCommand(Entity* entity, std::vector<StageTile*> tiles, City* city)
{
	switch (_mode)
	{
	case MenuMode::city_supply:
		_commands[Command::city_supply]->setColor(Color3B::GRAY);
		command::forCitySupply([this, entity, tiles](Command com, int i)
		{
			if (!command::isEnable(com, entity, tiles))
				_commands[com]->setColor(Color3B::GRAY);
			else
				_commands[com]->setColor(Color3B::WHITE);
		});
		break;
	case MenuMode::deploy:
		_commands[Command::deployment]->setColor(Color3B::GRAY);
		command::forDeploy([this, entity, tiles](Command com, int i)
		{
			if (!command::isEnable(com, entity, tiles))
				_commands[com]->setColor(Color3B::GRAY);
			else
				_commands[com]->setColor(Color3B::WHITE);
		});
		break;
	case MenuMode::dispatch:
		_commands[Command::dispatch]->setColor(Color3B::GRAY);
		command::forDispatch([this, entity, tiles](Command com, int i)
		{
			if (!command::isEnable(com, entity, tiles))
				_commands[com]->setColor(Color3B::GRAY);
			else
				_commands[com]->setColor(Color3B::WHITE);
		});
		break;
	default:
		command::forCity([this, city, entity, tiles](Command com, int i)
		{
			if (!TileInformation::getInstance()->getCommand(city->getTerrainType(), com))
				_commands[com]->setColor(Color3B::GRAY);
			else if (!command::isEnable(com, entity, tiles))
				_commands[com]->setColor(Color3B::GRAY);
			else
				_commands[com]->setColor(Color3B::WHITE);
		});
		break;
	}
}

/*
 * Move city command
 * Only move
 */
void MenuLayer::moveCityCommand()
{
	if (!isCityMenuModeAble())
	{
		hideCityCommand();
		return;
	}

	// Check showed
	if (_isShowedCityCommand)
		return;

	std::function<void(std::function<void(Command, int)>)> func;

	switch (_mode)
	{
	case MenuMode::none:
		func = command::forCity;
		break;
	case MenuMode::city_supply:
		showCityCommandByOne(0, 1, _commands[Command::city_supply]);
		func = command::forCitySupply;
		break;
	case MenuMode::deploy:
		showCityCommandByOne(0, 1, _commands[Command::deployment]);
		func = command::forDeploy;
		break;
	case MenuMode::dispatch:
		showCityCommandByOne(0, 1, _commands[Command::dispatch]);
		func = command::forDispatch;
		break;
	}

	// Move command
	if(func)
		func([this](Command com, int i)
		{
			showCityCommandByOne(i % 3, i / 3, _commands[com]);
		});

	_isShowedCityCommand = true;
}

/*
 * Hide city command
 * If already hided, do nothing
 */
void MenuLayer::hideCityCommand()
{
	if (!_isShowedCityCommand)
		return;

	std::function<void(std::function<void(Command, int)>)> func;

	switch (_mode)
	{
	case MenuMode::none:
		func = command::forCity;
		break;
	case MenuMode::city_supply:
		func = command::forCitySupply;
		hideCityCommandByOne(_commands[Command::city_supply]);
		break;
	case MenuMode::deploy:
		func = command::forDeploy;
		hideCityCommandByOne(_commands[Command::deployment]);
		break;
	case MenuMode::dispatch:
		func = command::forDispatch;
		hideCityCommandByOne(_commands[Command::dispatch]);
		break;

	}
	func([this](Command com, int i) 
	{
		hideCityCommandByOne(_commands[com]);
	});

	_isShowedCityCommand = false;
}

void MenuLayer::showCityCommandByOne(int x, int y, cocos2d::Node * command)
{
	command->stopAllActions();
	if (isHided(FrameType::map))
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(MODIFY + 10 + y * (command->getContentSize().width + 20), _map->getPosition().y + _map->getContentSize().height - x * (command->getContentSize().height + 15)))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(0, _map->getPosition().y) + Vec2(10 + x * (command->getContentSize().width + 20), y * (command->getContentSize().height + 15) + _map->getContentSize().height))),
			NULL));
}

void MenuLayer::hideCityCommandByOne(cocos2d::Node * command)
{
	command->stopAllActions();
	if (isHided(FrameType::map))
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _map->getContentSize().height))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map->getPosition().y + _map->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _map->getContentSize().height))),
			NULL));

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
			{
				_isShowedCityCommand = false;
				moveCityCommand();
			}
			if (type == FrameType::unit && _isShowedUnitCommand)
			{
				_isShowedUnitCommand = false;
				moveUnitCommand();
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
			if (type == FrameType::map && _isShowedCityCommand)
			{
				_isShowedCityCommand = false;
				moveCityCommand();
			}
			if (type == FrameType::unit && _isShowedUnitCommand)
			{
				_isShowedUnitCommand = false;
				moveUnitCommand();
			}
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
void MenuLayer::setMenuMode(MenuMode mode, Entity *unit, std::vector<StageTile*> tiles)
{
	if (_mode != mode)
	{
		hideUnitCommand();
		hideCityCommand();
		_mode = mode;
		checkUnitCommand(unit, tiles);
		moveUnitCommand();
		if(util::instanceof<City>(tiles.back()))
			checkCityCommand(unit, tiles, util::instance<City>(tiles.back()));
		moveCityCommand();
	}
	else
	{
		checkUnitCommand(unit, tiles);
		if (util::instanceof<City>(tiles.back()))
			checkCityCommand(unit, tiles, util::instance<City>(tiles.back()));
	}
}

/*
 * Show enemy unit
 */
void MenuLayer::showEnemyUnit(Entity * enemy)
{
	auto winSize = Director::getInstance()->getWinSize();
	setUnit(_enemy_unit, std::vector<StageTile*>(), enemy);
	_enemy_unit->stopAllActions();
	_enemy_unit->runAction(EaseSineIn::create(MoveTo::create(0.5f, Vec2(winSize.width, winSize.height))));
}

/*
 * Hide enemy unit
 */
void MenuLayer::hideEnemyUnit()
{
	auto winSize = Director::getInstance()->getWinSize();
	_enemy_unit->stopAllActions();
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

	showWeapon(weapon, unit, Color3B::BLACK);

	// Set frame
	auto no = 0;
	for (auto weapon : unit->getWeaponsByRef())
	{
		if (weapon && weapon->isUsable(unit))
			break;
		no++;
	}

	auto frame = util::createCutSkinAndAnimation("image/frame_1.png", 680, 40, 6, 1, 0, 0.15f);
	frame->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	frame->setPosition(weapon->getContentSize().width / 2, weapon->getContentSize().height - 15 - MENU_SIZE - 30 - no * 50);
	weapon->addChild(frame);
	_selectWeapon = no;

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
	lis->onTap = [this, unit, weapon, frame, decision, cancel](Touch* touch, Event* event)
	{
		if (util::isTouchInEvent(touch->getLocation(), weapon))
		{
			auto pos = touch->getLocation() - (weapon->getPosition() - Vec2(weapon->getContentSize().width / 2, 0));
			int no = (weapon->getContentSize().height - pos.y - 15 - 30 - MENU_SIZE / 2) / 50;
			if (no >= 0 && weapon->getChildByTag(no)->getColor() == Color3B::BLACK)
			{
				frame->stopAllActions();
				frame->runAction(MoveTo::create(0.2f, Vec2(weapon->getContentSize().width / 2, weapon->getContentSize().height - 15 - MENU_SIZE - 30 - no * 50)));
				_selectWeapon = no;
			}
		}
		else if (util::isTouchInEvent(touch->getLocation(), decision))
			attack_decision(unit->getWeaponsByRef().at(_selectWeapon));
		else if (util::isTouchInEvent(touch->getLocation(), cancel))
			attack_cancel(unit->getWeaponsByRef().at(_selectWeapon));
	};
	lis->onDoubleTap = [this, unit, weapon, lis] (Touch* touch, Event* event)
	{
		lis->onTap(touch, event);
		if (util::isTouchInEvent(touch->getLocation(), weapon))
			attack_decision(unit->getWeaponsByRef().at(_selectWeapon));

	};
	lis->onSwipe = [weapon](Vec2 v, Vec2 diff, float time)
	{
		if (util::isTouchInEvent(v, weapon))
		{
			auto winSize = Director::getInstance()->getWinSize();
			auto size = weapon->getContentSize();
			auto pos = weapon->getPosition() + diff;
			pos.x = (pos.x < size.width / 2 + 50) ? size.width / 2 + 50 : (pos.x > winSize.width - size.width / 2 - 50) ? winSize.width - size.width / 2 - 50 : pos.x;
			pos.y = (pos.y < 0) ? 0 : (pos.y > winSize.height - size.height) ? winSize.height - size.height : pos.y;
			weapon->setPosition(pos);
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, weapon);
}

void MenuLayer::showWeapon(Node * target, Entity * unit, Color3B color, bool checkUsable)
{
	// Set info
	auto height = target->getContentSize().height - 10 - INFO_SIZE;
	auto name = Label::createWithSystemFont(u8"装備名", JP_FONT, INFO_SIZE);
	name->setColor(color);
	name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	name->setPosition(70, height);
	target->addChild(name);
	auto x = 160;
	for (auto item : { u8"対人", u8"対魔", u8"対炎", u8"対氷", u8"対雷", u8"対土", u8"命中", u8"範囲" })
	{
		if (item == u8"範囲")
			x += 30;
		auto label = Label::createWithSystemFont(item, JP_FONT, INFO_SIZE);
		label->setColor(color);
		label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		label->setPosition(x, height);
		target->addChild(label);
		x += 60;
	}

	// Set weapon data
	auto no = 0;
	for (auto weapon : unit->getWeaponsByRef())
		if (weapon)
			renderWeapon(target, unit, weapon, no++, color, checkUsable);
}

/*
 * Render a weapon information
 */
void MenuLayer::renderWeapon(Node* target, Entity *unit, WeaponData * weapon, int no, Color3B color, bool checkUsable)
{
	auto height = target->getContentSize().height - 15 - 50 * no - MENU_SIZE - 30;
	auto usable = !checkUsable || weapon->isUsable(unit);

	// Set name
	auto name = Label::createWithSystemFont(weapon->getName(), JP_FONT, MENU_SIZE);
	name->setColor(usable ? color : Color3B::GRAY);
	name->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	name->setPosition(70, height);
	name->setTag(no);
	target->addChild(name);

	// Set info
	auto x = 160;
	for (auto item : { weapon->getAntiPersonnel(), weapon->getAntiWizard(), weapon->getAntiFire(), weapon->getAntiIce(), weapon->getAntiThunder(), weapon->getAntiGround(), weapon->getAccuracy() })
	{
		auto label = Label::createWithSystemFont(StringUtils::format("%d", item), EN_FONT, INFO_SIZE);
		label->setColor(usable ? color : Color3B::GRAY);
		label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		label->setPosition(x, height);
		target->addChild(label);
		x += 60;
	}

	// Set range
	x += 30;
	auto range = Label::createWithSystemFont(WeaponInformation::getInstance()->getRangeName(weapon->getRange()), JP_FONT, INFO_SIZE);
	range->setColor(usable ? color : Color3B::GRAY);
	range->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	range->setPosition(x, height);
	target->addChild(range);
}

void MenuLayer::hideWeaponFrame()
{
	if(this->getChildByTag(1000))
		this->getChildByTag(1000)->removeFromParentAndCleanup(true);
}

/*
 * Show specframe
 */
void MenuLayer::showSpecFrame(Entity * unit)
{
	auto winSize = Director::getInstance()->getWinSize();

	// Set main frame
	auto frame = util::createCutSkin(FRAME, 800, 600, 0);
	frame->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	frame->setPosition(winSize / 2);
	frame->setTag(1000);
	this->addChild(frame);

	// Set margin
	auto right = Sprite::create("image/window_side_right_2.png");
	right->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	right->setPosition(frame->getContentSize().width, 0);
	frame->addChild(right);
	auto left = Sprite::create("image/window_side_left_2.png");
	left->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	left->setPosition(0, 0);
	frame->addChild(left);

	// Set information
	// Set unit image
	auto unitImage = Sprite::create("image/unit.png", Rect(0, static_cast<int>(unit->getType()) * 32, 32, 32));
	unitImage->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	unitImage->setPosition(20, 580);
	unitImage->setScale(2.5f);
	frame->addChild(unitImage);

	// Set unit color
	auto color = Sprite::create();
	color->setTextureRect(Rect(0, 0, 30, 30));
	color->setColor(OwnerInformation::getInstance()->getColor(unit->getAffiliation()));
	color->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	color->setPosition(110, 580);
	frame->addChild(color);

	// Set unit name
	auto name = Label::createWithSystemFont(unit->getName(), JP_FONT, INFO_SIZE);
	name->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	name->setPosition(150, 575);
	name->setColor(Color3B::BLACK);
	frame->addChild(name);

	// Set unit ability
	auto count = 0;
	for (auto i : {
		StringUtils::format(u8"残存兵力 %d / %d", unit->getDurability(), unit->getMaxDurability()),
		StringUtils::format(u8"残存物資 %d / %d", unit->getMaterial(), unit->getMaxMaterial()),
		StringUtils::format(u8"索敵能力 %d", unit->getSearchingAbility()),
		StringUtils::format(u8"移動能力 %d", unit->getMobility()), 
		StringUtils::format(u8"防護装甲 %d", unit->getDefence())
	})
	{
		auto item = Label::createWithSystemFont(i, JP_FONT, INFO_SIZE);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		item->setPosition(130, 540 - count * (INFO_SIZE + 10));
		item->setColor(Color3B::BLACK);
		frame->addChild(item);
		count++;
	}

	// Set weapon frame
	auto h = unit->getWeaponsByRef().size() * 50 + 30 + 30;
	auto weapon = util::createCutSkin(COMMAND_FRAME, 750, h, 0);
	weapon->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	weapon->setPosition(400, 10);
	frame->addChild(weapon);

	showWeapon(weapon, unit, Color3B::WHITE, false);

	// Set listener
	auto lis = SingleTouchListener::create();
	lis->setSwallowTouches(true);
	lis->onTap = [this, frame] (Touch* touch, Event* event)
	{
		frame->removeFromParentAndCleanup(true);
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, frame);
}

void MenuLayer::deploy(City * city)
{
	showDeployers(city);
}

void MenuLayer::dispatch(City * city)
{
	auto lis = showDeployers(city);
	auto node = this->getChildByTag(1001)->getChildByTag(0);

	// Add frame
	_selectDeployer = 0;
	auto frame = util::createCutSkinAndAnimation("image/frame_1.png", 380, 100, 6, 1, 0, 0.15f);
	frame->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	frame->setPosition(10, node->getContentSize().height - 105);
	node->addChild(frame);

	// Set listener
	lis->onTap = [this, city, frame](Touch *touch, Event *event)
	{
		auto node = event->getCurrentTarget();
		auto pos = 600 - (touch->getLocation().y - node->getPosition().y + (600 - node->getContentSize().height));
		int no = pos / 100;
		if (no < city->getDeployersByRef().size())
		{
			_selectDeployer = no;
			frame->stopAllActions();
			frame->runAction(MoveTo::create(0.1f, Vec2(10, node->getContentSize().height - 100 * (no+1) - 5)));
		}
	};
	lis->onDoubleTap = [this, city, frame, lis](Touch *touch, Event *event)
	{
		lis->onTap(touch, event);
		getFunction(Command::dispatch_start)();
	};

}

SingleTouchListener* MenuLayer::showDeployers(City * city)
{
	auto winSize = Director::getInstance()->getWinSize();

	// Set frame
	auto frame = util::createCutSkin(FRAME, 400, 600, util::CUT_MASK_UP | util::CUT_MASK_DOWN);
	frame->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	frame->setPosition(winSize.width, 0);
	frame->setTag(1001);
	this->addChild(frame);

	// Set node
	auto node = Node::create();
	node->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	node->setContentSize(Size(400, 100 * city->getDeployersByRef().size() + 20));
	node->setPosition(0, 600 - node->getContentSize().height);
	node->setTag(0);
	frame->addChild(node);

	// Set unit data
	auto count = 0;
	for (auto unit : city->getDeployersByRef())
		renderDeployer(node, unit, node->getContentSize().height - count++ * 100);


	// Set listener
	auto lis = SingleTouchListener::create();
	lis->setSwallowTouches(true);
	lis->onTouchBeganChecking = [frame](Touch *touch, Event *event) {return util::isTouchInEvent(touch->getLocation(), frame); };
	lis->onSwipe = [node](Vec2 v, Vec2 diff, float time)
	{
		if (600 < node->getContentSize().height)
		{
			auto pos = node->getPosition() + Vec2(0, diff.y);
			pos.y = (pos.y < 600 - node->getContentSize().height) ? 600 - node->getContentSize().height : (pos.y > 0) ? 0 : pos.y;
			node->setPosition(pos);
		}
	};
	lis->onFlick = [node](Vec2 v, Vec2 diff, float time)
	{
		if (600 < node->getContentSize().height)
		{
			node->runAction(Spawn::create(
				Sequence::create(
					EaseSineOut::create(MoveBy::create(0.5f, Vec2(0, diff.y) / time / 3)),
					CallFunc::create([node] {
						node->stopAllActions();
					}),
				NULL),
				Repeat::create(Sequence::create(
					CallFunc::create([node] {
						auto pos = node->getPosition();
						pos.y = (pos.y < 600 - node->getContentSize().height) ? 600 - node->getContentSize().height : (pos.y > 0) ? 0 : pos.y;
						node->setPosition(pos);
					}),
					DelayTime::create(0.005f),
				NULL), -1),
			NULL));
		}
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, node);

	return lis;
}

void MenuLayer::renderDeployer(Node* target, Entity * unit, int y)
{
	y -= 10;

	// Set unit image
	auto unitImage = Sprite::create("image/unit.png", Rect(0, static_cast<int>(unit->getType()) * 32, 32, 32));
	unitImage->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	unitImage->setPosition(20, y - 10);
	unitImage->setScale(2.5f);
	target->addChild(unitImage);

	// Set unit color
	auto color = Sprite::create();
	color->setTextureRect(Rect(0, 0, 30, 30));
	color->setColor(OwnerInformation::getInstance()->getColor(unit->getAffiliation()));
	color->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	color->setPosition(110, y);
	target->addChild(color);

	// Set unit name
	auto name = Label::createWithSystemFont(unit->getName(), JP_FONT, INFO_SIZE);
	name->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	name->setPosition(150, y - 5);
	name->setColor(Color3B::BLACK);
	target->addChild(name);

	// Set unit ability
	auto count = 0;
	for (auto i : {
		StringUtils::format(u8"残存兵力 %d / %d", unit->getDurability(), unit->getMaxDurability()),
		StringUtils::format(u8"残存物資 %d / %d", unit->getMaterial(), unit->getMaxMaterial()),
	})
	{
		auto item = Label::createWithSystemFont(i, JP_FONT, INFO_SIZE);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		item->setPosition(130, y - 40 - count * (INFO_SIZE + 10));
		item->setColor(Color3B::BLACK);
		target->addChild(item);
		count++;
	}
}

void MenuLayer::hideDeployers()
{
	if (this->getChildByTag(1001))
		this->removeChildByTag(1001);
}
