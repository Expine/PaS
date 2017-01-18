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
 * Constructor
 */
MenuLayer::MenuLayer()
	: _mode(MenuMode::none)
	, _unit_frame(nullptr), _enemy_unit_frame(nullptr), _map_frame(nullptr), _menu_frame(nullptr), _info_frame(nullptr)
	, _is_showed_city_command(false), _is_showed_unit_command(false)
	, _stage(nullptr)
	, _select_weapon(0), _select_deployer(nullptr)
	, attack_decision(nullptr), attack_cancel(nullptr)
{
}

/*
 * Destructor
 */
MenuLayer::~MenuLayer()
{
	_unit_frame = _enemy_unit_frame = _map_frame = _menu_frame = nullptr;
	_info_frame = nullptr;
	_is_showed_city_command = _is_showed_unit_command = false;
	_stage = nullptr;
	_select_weapon = 0;
	_select_deployer = nullptr;
	attack_decision = attack_cancel = nullptr;
}


/*
 * Initialize
 * Set sprite and listener
 */
bool MenuLayer::init()
{
	if (!Node::init())
		return false;

	auto winSize = Director::getInstance()->getWinSize();

	// Set each frame
	_unit_frame = setEachFrame(Vec2::ANCHOR_TOP_LEFT, 0, winSize.height, 300, 200, util::CUT_MASK_LEFT, { "U", "N", "I", "T" }, _unit_labels);
	_map_frame = setEachFrame(Vec2::ANCHOR_BOTTOM_LEFT, 0, 0, 300, 110, util::CUT_MASK_LEFT, { "M", "A", "P" }, _map_labels);
	_menu_frame = setEachFrame(Vec2::ANCHOR_TOP_RIGHT, winSize.width, winSize.height, 220, 460, util::CUT_MASK_RIGHT, { "M", "E", "N", "U" }, _menu_labels);

	// Set info frame
	_info_frame = Sprite::create();
	_info_frame->setTextureRect(Rect(0, 0, 350, 40));
	_info_frame->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	_info_frame->setPosition(winSize.width, 0);
	_info_frame->setColor(Color3B(0, 0, 0));
	_info_frame->setOpacity(100);
	this->addChild(_info_frame);

	// Set enemy unit frame
	_enemy_unit_frame = Sprite::create();
	_enemy_unit_frame = util::createCutSkin(FRAME, 300, 200, util::CUT_MASK_RIGHT, 255);
	_enemy_unit_frame->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	_enemy_unit_frame->setPosition(winSize.width + _enemy_unit_frame->getContentSize().width, winSize.height);
	this->addChild(_enemy_unit_frame);

	// Set menu item
	command::forMenu([this] (Command com, int i){
		auto item = Label::createWithSystemFont(command::getName(com), JP_FONT, MENU_SIZE);
		item->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		item->setColor(Color3B::BLACK);
		item->setPosition(_menu_frame->getContentSize().width - 10, _menu_frame->getContentSize().height - 50 - 60 * i);
		item->setTag(10 + (i - static_cast<int>(Command::MENU_START) - 1));
		_menu_frame->addChild(item);
		_command_frames[com] = item;
	});

	// Set Unit command
	command::forAllUnit([this] (Command com, int i)
	{
		_command_frames[com] = setCommand(command::getName(com), -80, _unit_frame->getPosition().y - _unit_frame->getContentSize().height, 80, 40); 
	});

	// Set City command
	command::forAllCity([this](Command com, int i) 
	{
		_command_frames[com] = setCommand(command::getName(com), -80, _map_frame->getContentSize().height, 80, 40);
		_command_frames[com]->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	});

	// Set enemy unit label
	auto enemy_label = Label::createWithSystemFont("ENEMY", EN_FONT, INFO_SIZE);
	enemy_label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	enemy_label->setPosition(0, _enemy_unit_frame->getContentSize().height);
	enemy_label->setColor(Color3B::RED);
	_enemy_unit_frame->addChild(enemy_label);

	// Set listener
	setFrameListener(_unit_frame, _unit_labels, FrameType::unit, 300 - MODIFY);
	setFrameListener(_map_frame, _map_labels, FrameType::map, 300 - MODIFY);
	setFrameListener(_menu_frame, _menu_labels, FrameType::menu, -220 + MODIFY);

	command::forMenu([this] (Command com, int i)
	{
		setCommandListener(_command_frames[com], _command_functions[com], false);
	});

	command::forAll([this](Command com, int i) 
	{
		setCommandListener(_command_frames[com], _command_functions[com]);
	});

	return true;
}

/*
 * Set each frame and words
 */
Node* MenuLayer::setEachFrame(const Vec2 anchor, const int x, const int y, const int w, const int h, const int cut_mask, std::vector<std::string> words, std::vector<Label*> &labels)
{
	// Set frame
	auto frame = util::createCutSkin(FRAME, w, h, cut_mask, 200);
	frame->setAnchorPoint(anchor);
	frame->setPosition(x, y);
	this->addChild(frame);

	// Set words
	for (auto name : words)
	{
		auto label = Label::createWithSystemFont(name, EN_FONT, INFO_SIZE);
		label->setColor(Color3B::BLACK);
		label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		label->setPosition(labels.size() == 0 ? 5 : labels.back()->getPosition().x + labels.back()->getContentSize().width, frame->getContentSize().height - 5);
		frame->addChild(label);
		labels.push_back(label);
	}

	return frame;
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
 * Move frame and words
 */
void MenuLayer::setFrameListener(Node *target, const std::vector<Label*>& labels, FrameType type, int moveX)
{
	auto frame_listener = SingleTouchListener::create();
	frame_listener->onTouchBeganChecking = [this, type](Touch* touch, Event* event)
	{
		//Set on frame
		return _on_frame[type] = util::isTouchInEvent(touch, event);
	};
	frame_listener->onTap = [this, target, labels, type, moveX](Touch* touch, Event* event)
	{
		//Unless on frame, do nothing
		if (!_on_frame[type])
			return;

		// Show frame
		showFrame(target, labels, type, moveX);
	};
	//long tap began equals tap
	frame_listener->onLongTapBegan = [frame_listener, type](Vec2 v)
	{
		frame_listener->onTap(nullptr, nullptr);
	};
	// Swipe process
	frame_listener->onSwipe = [this, target, labels, type, moveX, frame_listener](Vec2 v, Vec2 diff, float time)
	{
		//Unless on frame, do nothing
		if (!_on_frame[type])
			return;

		// if action excuting, do nothing
		if (target->getNumberOfRunningActions() != 0)
			return;

		//If showable swipe, show frame
		if (isShowableBySwipe(type, diff) && _is_hided[type])
			showFrame(target, labels, type, moveX);
		//If hidable swipe, hide frame
		else if (isHidableBySwipe(type, diff) && !_is_hided[type])
			hideFrame(target, labels, type, moveX);
	};
	frame_listener->onTouchEndedChecking = [this, type](Touch* touch, Event* event)
	{
//		_on_frame[type] = false;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(frame_listener, target);
}

/*
 * Show frame when frame is hided
 * Move frame and words, commands
 */
void MenuLayer::showFrame(cocos2d::Node * target, const std::vector<cocos2d::Label*>& labels, FrameType type, int moveX)
{
	// if not hided, do nothing
	if (!_is_hided[type])
		return;

	// Move frame
	target->runAction(EaseSineIn::create(MoveBy::create(0.5f, Vec2(moveX, 0))));
	_is_hided[type] = false;

	// Move words
	auto x = 0.0f;
	auto y = 0.0f;
	for (auto label : labels)
	{
		label->runAction(Sequence::create(
			EaseSineIn::create(MoveBy::create(x / 300, Vec2(0, y))),
			EaseSineIn::create(MoveBy::create(0.5f - x / 300, Vec2(x + (type == FrameType::menu ? 0 : -moveX), 0))),
			NULL));
		x += label->getContentSize().width;
		y += label->getContentSize().height;
	}

	// If map frame and showing it, also move command
	if (type == FrameType::map && _is_showed_city_command)
	{
		_is_showed_city_command = false;
		moveCityCommand();
	}

	// If unit frame and showing it, also move command
	if (type == FrameType::unit && _is_showed_unit_command)
	{
		_is_showed_unit_command = false;
		moveUnitCommand();
	}
}

/*
 * Hide framw when fram is showed
 * Move frame and words, commands
 */
void MenuLayer::hideFrame(cocos2d::Node * target, const std::vector<cocos2d::Label*>& labels, FrameType type, int moveX)
{
	// if hided, do nothing
	if (_is_hided[type])
		return;

	// Move frame
	target->runAction(EaseSineOut::create(MoveBy::create(0.5f, Vec2(-moveX, 0))));
	_is_hided[type] = true;

	// Move words
	auto x = 0.0f;
	auto y = 0.0f;
	for (auto label : labels)
	{
		label->runAction(Sequence::create(
			EaseSineOut::create(MoveBy::create(0.5f - x / 300, Vec2((type == FrameType::menu ? 0 : moveX) - x, 0))),
			EaseSineOut::create(MoveBy::create(x / 300, Vec2(0, -y))),
			NULL));
		x += label->getContentSize().width;
		y += label->getContentSize().height;
	}

	// If map frame and showing it, also move command
	if (type == FrameType::map && _is_showed_city_command)
	{
		_is_showed_city_command = false;
		moveCityCommand();
	}

	// If unit frame and showing it, also move command
	if (type == FrameType::unit && _is_showed_unit_command)
	{
		_is_showed_unit_command = false;
		moveUnitCommand();
	}
}

/*
 * Set each command listener
 * if tap on target, call func
 */
void MenuLayer::setCommandListener(cocos2d::Node * target, std::function<void()> &func, bool isWhiteEnable)
{
	auto listener = SingleTouchListener::create();
	listener->setSwallowTouches(true);
	listener->onTouchBeganChecking = [](Touch* touch, Event* event)
	{
		return util::isTouchInEvent(touch, event);
	};
	listener->onTap = [target, &func, isWhiteEnable](Touch* touch, Event* event)
	{
		if (!isWhiteEnable || target->getColor() == Color3B::WHITE && func)
			func();
	};
	listener->onDoubleTap = [listener](Touch* touch, Event* event)
	{
		listener->onTap(touch, event);
		listener->onTap(touch, event);
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, target);
}

/*
* Check swipe makes frame hided
*/
bool MenuLayer::isHidableBySwipe(FrameType type, cocos2d::Vec2 diff)
{
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
bool MenuLayer::isShowableBySwipe(FrameType type, cocos2d::Vec2 diff)
{
	switch (type)
	{
	case FrameType::unit:	return diff.x > 0;
	case FrameType::map:	return diff.x > 0;
	case FrameType::menu:	return diff.x < 0;
	case FrameType::info:	return diff.x < 0;
	}
	return false;
}

/********************************************************************************/

/*
 * Set tile information
 */
void MenuLayer::setTileInformation(std::vector<StageTile*> tiles, Entity* unit)
{
	// Remove pre-information
	if (_map_frame->getChildByTag(0))
	{
		_map_frame->removeChildByTag(0);
		_map_frame->removeChildByTag(1);
	}
	if (_map_frame->getChildByTag(2))
		_map_frame->removeChildByTag(2);

	// Get last tile
	StageTile* tile = tiles.back();

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
	_map_frame->addChild(tileImage);

	// Set name
	auto name= Label::createWithSystemFont(TileInformation::getInstance()->getName(tile->getTerrainType()), JP_FONT, INFO_SIZE);
	name->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	name->setPosition(80, 90);
	name->setColor(Color3B::BLACK);
	name->setTag(1);
	_map_frame->addChild(name);

	// Set durability
	if (util::instanceof<City>(tile))
		setCityDurability(util::instance<City>(tile));

	// Set unit to tile information
	setUnitToTileInformation(tiles, unit);

	// Show city command
	if (util::instanceof<City>(tile) && util::instance<City>(tile)->getOwner() == Owner::player)
		moveCityCommand();
	else
		hideCityCommand();
}

/*
 * Set city durability on map frame
 */
void MenuLayer::setCityDurability(City * city)
{
	auto node = Node::create();
	node->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	node->setContentSize(Size(140, INFO_SIZE));
	node->setPosition(145, 90);
	node->setTag(2);
	_map_frame->addChild(node);
	for (int i = 0; i < DURABILITY_SPLIT; i++)
	{
		auto sq = Sprite::create();
		sq->setTextureRect(Rect(0, 0, 5, INFO_SIZE));
		sq->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		sq->setPosition(7 * i, 0);
		sq->setColor(city->getDurability() > i * city->getMaxDurability() / DURABILITY_SPLIT ? Color3B::WHITE : Color3B::GRAY);
		node->addChild(sq);
	}
}

/*
 * Set unit information on target
 */
void MenuLayer::setUnitInformation(std::vector<StageTile*> tiles, Entity * unit, Node * target)
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
	setUnitAbility(unit, target);

	// Set unit to tile information
	setUnitToTileInformation(tiles, unit);

	// If unit frame, check command
	if (target == _unit_frame)
		moveUnitCommand();
}

/*
 * Set unit ability
 */
void MenuLayer::setUnitAbility(Entity * unit, Node* target)
{
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
}

/*
 * Set tile coordinate information
 */
void MenuLayer::setCoordinateInformation(Vec2 cor)
{
	// Remove pre-information
	if (_info_frame->getChildByTag(0) != NULL)
	{
		_info_frame->removeChildByTag(0);
		_info_frame->removeChildByTag(1);
	}

	//Set x coordinate
	auto xLabel = Label::createWithSystemFont(StringUtils::format("X = %d", (int)cor.x), EN_FONT, INFO_SIZE);
	xLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	xLabel->setPosition(5, _info_frame->getTextureRect().getMidY());
	xLabel->setTag(0);
	_info_frame->addChild(xLabel);

	//Set y coordinate
	auto yLabel = Label::createWithSystemFont(StringUtils::format("Y = %d", (int)cor.y), EN_FONT, INFO_SIZE);
	yLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	yLabel->setPosition(80, _info_frame->getTextureRect().getMidY());
	yLabel->setTag(1);
	_info_frame->addChild(yLabel);
}

/*
 * Set unit to tile data
 */
void MenuLayer::setUnitToTileInformation(std::vector<StageTile*> tiles, Entity *unit)
{
	// Remove pre-information
	if (_map_frame->getChildByTag(3))
	{
		_map_frame->removeChildByTag(3);
		_map_frame->removeChildByTag(4);
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
	_map_frame->addChild(cost);

	// Set effect
	auto effect = Label::createWithSystemFont(StringUtils::format(u8"地形効果  : %d %%", effect_value), JP_FONT, INFO_SIZE);
	effect->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	effect->setPosition(90, 15);
	effect->setColor(Color3B::BLACK);
	effect->setTag(4);
	_map_frame->addChild(effect);
}

/********************************************************************************/

/*
 * Check unit command enable
 */
void MenuLayer::checkUnitCommand(Entity *entity, std::vector<StageTile*> tiles, Entity* enemy, WeaponData* weapon, std::vector<StageTile*> area)
{
	std::function<void(std::function<void(Command, int)>)> func = nullptr;
	switch (_mode)
	{
	case MenuMode::none:
		func = command::forUnit;
		break;
	case MenuMode::move:
		_command_frames[Command::move]->setColor(Color3B::GRAY);
		func = command::forMove;
		break;
	case MenuMode::moving:
	{
		_command_frames[Command::move]->setColor(Color3B::GRAY);
		// TODO Attack command when moving
		_command_frames[Command::attack]->setColor(Color3B::GRAY);
		func = command::forMove2;
		break;
	}
	case MenuMode::attack:
		_command_frames[Command::attack]->setColor(Color3B::GRAY);
		func = command::forAttack;
		break;
	case MenuMode::attacking:
		_command_frames[Command::attack]->setColor(Color3B::GRAY);
		func = command::forAttack2;
		break;
	case MenuMode::occupy:
		_command_frames[Command::occupation]->setColor(Color3B::GRAY);
		func = command::forOccupation;
		break;
	case MenuMode::wait:
		_command_frames[Command::wait]->setColor(Color3B::GRAY);
		func = command::forWait;
		break;
	default:
		break;
	}

	// Check command enable
	if(func)
		func([this, entity, tiles, enemy, weapon, area](Command com, int i)
		{
			// Whether this command is enable
			if (command::isEnable(com, entity, tiles, enemy, weapon, area))
				_command_frames[com]->setColor(Color3B::WHITE);
			else
				_command_frames[com]->setColor(Color3B::GRAY);
		});
}

/*
 * Move unit command
 * Only move
 */
void MenuLayer::moveUnitCommand()
{
	// Already unit mode, do nothing
	if (!isUnitMenuModeAble())
	{
		hideUnitCommand();
		return;
	}

	// Already show command, do nothing
	if (_is_showed_unit_command)
		return;

	// Set showed
	_is_showed_unit_command = true;

	// Move command
	switch (_mode)
	{
	case MenuMode::none:
		command::forUnit([this](Command com, int i) { showUnitCommandByOne(i % 3, i / 3, com); });
		break;
	case MenuMode::move:
		showUnitCommandByOne(0, 0, Command::move);
		command::forMove([this](Command com, int i) { showUnitCommandByOne(0 + i, 1, com); });
		break;
	case MenuMode::moving:
		showUnitCommandByOne(0, 0, Command::move);
		showUnitCommandByOne(0, 1, Command::attack);
		command::forMove2([this](Command com, int i) { showUnitCommandByOne(i + 1, 1, com); });
		break;
	case MenuMode::attack:
		showUnitCommandByOne(0, 0, Command::attack);
		command::forAttack([this](Command com, int i) { showUnitCommandByOne(i, 1, com); });
		break;
	case MenuMode::attacking:
		showUnitCommandByOne(0, 0, Command::attack);
		showUnitCommandByOne(2, 1, Command::attack_end);
		command::forAttack2([this](Command com, int i) { showUnitCommandByOne(i, 1, com); });
		break;
	case MenuMode::occupy:
		showUnitCommandByOne(0, 0, Command::occupation);
		command::forOccupation([this](Command com, int i) { showUnitCommandByOne(i, 1, com); });
		break;
	case MenuMode::wait:
		showUnitCommandByOne(0, 0, Command::wait);
		command::forWait([this](Command com, int i) { showUnitCommandByOne(i, 1, com); });
		break;
	}
}

/*
 * Showing unit command one by one
 */
void MenuLayer::showUnitCommandByOne(int x, int y, Command com)
{
	auto command = _command_frames[com];
	command->stopAllActions();
	if (_is_hided[FrameType::unit])
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit_frame->getPosition().y - _unit_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(MODIFY + 10 + y * (command->getContentSize().width + 20), -20 + _unit_frame->getPosition().y - x * (command->getContentSize().height + 15)))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit_frame->getPosition().y - _unit_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(0, _unit_frame->getPosition().y) + Vec2(10 + x * (command->getContentSize().width + 20), -y * (command->getContentSize().height + 15) - _unit_frame->getContentSize().height))),
			NULL));
}


/*
 * Hide unit command
 * If already hided, do nothing
 */
void MenuLayer::hideUnitCommand()
{
	// Not show command, do nothing
	if (!_is_showed_unit_command)
		return;

	// Set hided
	_is_showed_unit_command = false;

	std::function<void(std::function<void(Command, int)>)> func;

	switch (_mode)
	{
	case MenuMode::none:
		func = command::forUnit;
		break;
	case MenuMode::move:
		hideUnitCommandByOne(Command::move);
		hideUnitCommandByOne(Command::attack);
		func = command::forMove;
		break;
	case MenuMode::moving:
		hideUnitCommandByOne(Command::move);
		hideUnitCommandByOne(Command::attack);
		func = command::forMove2;
		break;
	case MenuMode::attack:
		hideUnitCommandByOne(Command::attack);
		func = command::forAttack;
		break;
	case MenuMode::attacking:
		hideUnitCommandByOne(Command::attack);
		hideUnitCommandByOne(Command::attack_end);
		func = command::forAttack2;
		break;
	case MenuMode::occupy:
		hideUnitCommandByOne(Command::occupation);
		func = command::forOccupation;
		break;
	case MenuMode::wait:
		hideUnitCommandByOne(Command::wait);
		func = command::forWait;
		break;
	}

	// Hide supecify commands
	func([this](Command com, int i) { hideUnitCommandByOne(com); });
}

/*
 * Hiding unit command one by one
 */
void MenuLayer::hideUnitCommandByOne(Command com)
{
	auto command = _command_frames[com];
	command->stopAllActions();
	if (_is_hided[FrameType::unit])
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit_frame->getPosition().y - _unit_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _unit_frame->getPosition().y - _unit_frame->getContentSize().height))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _unit_frame->getPosition().y - _unit_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _unit_frame->getPosition().y - _unit_frame->getContentSize().height))),
			NULL));
}

/*
 * Check city command enable
 */
void MenuLayer::checkCityCommand(Entity* entity, std::vector<StageTile*> tiles, Entity* enemy, WeaponData* weapon, std::vector<StageTile*> area)
{
	std::function<void(std::function<void(Command, int)>)> func = nullptr;

	switch (_mode)
	{
	case MenuMode::city_supply:
		_command_frames[Command::city_supply]->setColor(Color3B::GRAY);
		func = command::forCitySupply;
		break;
	case MenuMode::deploy:
		_command_frames[Command::deployment]->setColor(Color3B::GRAY);
		func = command::forDeploy;
		break;
	case MenuMode::dispatch:
		_command_frames[Command::dispatch]->setColor(Color3B::GRAY);
		func = command::forDispatch;
		break;
	default:
		func = command::forCity;
		break;
	}

	func([this, entity, tiles, enemy, weapon, area](Command com, int i)
	{
		if (command::isEnable(com, entity, tiles, enemy, weapon, area))
			_command_frames[com]->setColor(Color3B::WHITE);
		else
			_command_frames[com]->setColor(Color3B::GRAY);
	});

}

/*
 * Move city command
 * Only move
 */
void MenuLayer::moveCityCommand()
{
	// Already city mode, do nothing
	if (!isCityMenuModeAble())
	{
		hideCityCommand();
		return;
	}

	// Already show command, do nothing
	if (_is_showed_city_command)
		return;

	//  Set showed
	_is_showed_city_command = true;

	std::function<void(std::function<void(Command, int)>)> func;

	// Move command
	switch (_mode)
	{
	case MenuMode::none:
		func = command::forCity;
		break;
	case MenuMode::city_supply:
		showCityCommandByOne(0, 1, Command::city_supply);
		func = command::forCitySupply;
		break;
	case MenuMode::deploy:
		showCityCommandByOne(0, 1, Command::deployment);
		func = command::forDeploy;
		break;
	case MenuMode::dispatch:
		showCityCommandByOne(0, 1, Command::dispatch);
		func = command::forDispatch;
		break;
	}

	// Move command
	if (func)
		func([this](Command com, int i) { showCityCommandByOne(i % 3, i / 3, com); });
}

/*
 * Showing city command one by one
 */
void MenuLayer::showCityCommandByOne(int x, int y, Command com)
{
	auto command = _command_frames[com];
	command->stopAllActions();
	if (_is_hided[FrameType::map])
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map_frame->getPosition().y + _map_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(MODIFY + 10 + y * (command->getContentSize().width + 20), _map_frame->getPosition().y + _map_frame->getContentSize().height - x * (command->getContentSize().height + 15)))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map_frame->getPosition().y + _map_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(0, _map_frame->getPosition().y) + Vec2(10 + x * (command->getContentSize().width + 20), y * (command->getContentSize().height + 15) + _map_frame->getContentSize().height))),
			NULL));
}

/*
 * Hide city command
 * If already hided, do nothing
 */
void MenuLayer::hideCityCommand()
{
	// Not show command, do nothing
	if (!_is_showed_city_command)
		return;

	// Set hided
	_is_showed_city_command = false;

	std::function<void(std::function<void(Command, int)>)> func;

	switch (_mode)
	{
	case MenuMode::city_supply:
		hideCityCommandByOne(Command::city_supply);
		func = command::forCitySupply;
		break;
	case MenuMode::deploy:
		func = command::forDeploy;
		hideCityCommandByOne(Command::deployment);
		break;
	case MenuMode::dispatch:
		func = command::forDispatch;
		hideCityCommandByOne(Command::dispatch);
		break;
	case MenuMode::none:
	default:
		func = command::forCity;
		break;

	}

	func([this](Command com, int i) { hideCityCommandByOne(com); });
}

/*
 * Hiding city command one by one
 */
void MenuLayer::hideCityCommandByOne(Command com)
{
	auto command = _command_frames[com];
	command->stopAllActions();
	if (_is_hided[FrameType::map])
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map_frame->getPosition().y + _map_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _map_frame->getContentSize().height))),
			NULL));
	else
		command->runAction(Sequence::create(
			EaseExponentialOut::create(MoveTo::create(0.15f, Vec2(MODIFY + 10, _map_frame->getPosition().y + _map_frame->getContentSize().height))),
			EaseExponentialOut::create(MoveTo::create(0.5f, Vec2(-command->getContentSize().width, _map_frame->getContentSize().height))),
			NULL));

}

/********************************************************************************/

/*
 * Check whether action unlock onFrame
 */
bool MenuLayer::checkAction(Vec2 diff, FrameType type, Node *target)
{
	// If showied and swipe not hidable, onFrame is unenable
//	if (!isHidableBySwipe(type, diff) && !_is_hided[type] && target->getNumberOfRunningActions() == 0)
//		_on_frame[type] = false;

	// If hided and swipe not showable, onFrame is unenable
//	if (!isShowableBySwipe(type, diff) && _is_hided[type] && target->getNumberOfRunningActions() == 0)
//		_on_frame[type] = false;
	return !_on_frame[type];
}

/********************************************************************************/

/*
 * Show enemy unit
 */
void MenuLayer::showEnemyUnit(Entity * enemy)
{
	auto winSize = Director::getInstance()->getWinSize();
	setUnitInformation(std::vector<StageTile*>(), enemy, _enemy_unit_frame);
	_enemy_unit_frame->stopAllActions();
	_enemy_unit_frame->runAction(EaseSineIn::create(MoveTo::create(0.5f, Vec2(winSize.width, winSize.height))));
}

/*
 * Hide enemy unit
 */
void MenuLayer::hideEnemyUnit()
{
	auto winSize = Director::getInstance()->getWinSize();
	_enemy_unit_frame->stopAllActions();
	_enemy_unit_frame->runAction(EaseSineOut::create(MoveTo::create(0.5f, Vec2(winSize.width + _enemy_unit_frame->getContentSize().width, winSize.height))));
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

	// Set weapon information
	showWeapon(weapon, unit, Color3B::BLACK);

	// Set initial position
	_select_weapon = 0;
	for (auto weapon : unit->getWeaponsByRef())
	{
		if (weapon && weapon->isUsable(unit))
			break;
		++_select_weapon;
	}

	// Set frame
	auto frame = util::createCutSkinAndAnimation("image/frame_1.png", 680, 40, 6, 1, 0, 0.15f);
	frame->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	frame->setPosition(weapon->getContentSize().width / 2, weapon->getContentSize().height - 15 - MENU_SIZE - 30 - _select_weapon * 50);
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
				_select_weapon = no;
			}
		}
		else if (util::isTouchInEvent(touch->getLocation(), decision))
			attack_decision(unit->getWeaponsByRef().at(_select_weapon));
		else if (util::isTouchInEvent(touch->getLocation(), cancel))
			attack_cancel(unit->getWeaponsByRef().at(_select_weapon));
	};
	lis->onDoubleTap = [this, unit, weapon, lis] (Touch* touch, Event* event)
	{
		lis->onTap(touch, event);
		if (util::isTouchInEvent(touch->getLocation(), weapon))
			attack_decision(unit->getWeaponsByRef().at(_select_weapon));
		else
			attack_cancel(unit->getWeaponsByRef().at(_select_weapon));
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

/*
 * Show weapon information on target
 */
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

/*
 * Hide weapon frame
 */
void MenuLayer::hideWeaponFrame()
{
	if(this->getChildByTag(1000))
		this->getChildByTag(1000)->removeFromParentAndCleanup(true);
}

/*
 * Show spec frame
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

/*
 * Show deployers frame
 */
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

/*
 * Render each deployer
 */
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

/*
 * Hide deployers frame
 */
void MenuLayer::hideDeployers()
{
	if (this->getChildByTag(1001))
		this->removeChildByTag(1001);
}

/*
 * Deploy event
 */
void MenuLayer::deploy(City * city)
{
	showDeployers(city);
}

void MenuLayer::dispatch(City * city)
{
	auto lis = showDeployers(city);
	auto node = this->getChildByTag(1001)->getChildByTag(0);

	// Add frame
	_select_deployer = city->getDeployersByRef().front();
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
			_select_deployer = city->getDeployersByRef().at(no);
			frame->stopAllActions();
			frame->runAction(MoveTo::create(0.1f, Vec2(10, node->getContentSize().height - 100 * (no + 1) - 5)));
		}
	};
	lis->onDoubleTap = [this, city, frame, lis](Touch *touch, Event *event)
	{
		lis->onTap(touch, event);
		getFunction(Command::dispatch_start)();
	};
}

