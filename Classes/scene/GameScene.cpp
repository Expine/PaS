#include "GameScene.h"

#include "ai/Owner.h"
#include "ai/Ai.h"
#include "stage/Menu.h"
#include "stage/Stage.h"
#include "stage/Tile.h"
#include "stage/Command.h"
#include "entity/Entity.h"
#include "entity/Weapon.h"
#include "util/Util.h"

USING_NS_CC;

/*
 * Create scene
 */
Scene* Game::createScene(Stage* stage)
{
	CCLOG("GameScene Create");
	auto scene = Scene::create();
    auto layer = Game::create(stage);
	layer->setTag(0);
    scene->addChild(layer);
    return scene;
}

/*
 * Constructor
 */
Game::Game()
	: _stage(nullptr), _menu(nullptr)
	, _select_unit(nullptr), _select_enemy(nullptr), _select_weapon(nullptr)
	, _end_function(nullptr)
{}

/*
 * Destructor
 */
Game::~Game()
{
	_stage = nullptr;
	_menu = nullptr;
	_select_unit = _select_enemy = nullptr;
	_select_weapon = nullptr;
	_end_function = nullptr;
}


/*
 * Initialize
 * Set stage and menu listener
 */
bool Game::init(Stage* stage)
{
    if ( !Layer::init() )
        return false;

	// Set stage
	this->addChild(stage);
	_stage = stage;

	// Set menu
	_menu = MenuLayer::create();
	_menu->setStage(stage);
	this->addChild(_menu, 10);

	// For debug
	auto ai = PlayerAI::create();
	ai->initialize(stage, Owner::player);
	ai->retain();


	// Set stgae listener
	stage->onTap = std::bind(&Game::onTap, this, std::placeholders::_1, std::placeholders::_2);
	stage->onLongTapBegan = std::bind(&Game::onLongTapBagan, this, std::placeholders::_1, std::placeholders::_2);
	stage->onDoubleTap = std::bind(&Game::onDoubleTap, this, std::placeholders::_1, std::placeholders::_2);
	// If menu action, swipe is disable
	stage->onSwipeCheck = [this](Vec2 v, Vec2 diff, float time) { return _menu->checkAllAction(diff); };
	// If menu action, swipe is disable
	stage->onFlickCheck = [this](Vec2 v, Vec2 diff, float time) { return _menu->checkAllAction(diff); };

	//Set menu listener
	_menu->setFunction(Command::endPhase, [this] { _end_function(); });
	_menu->setFunction(Command::nextCity, [this] { if(!_select_tiles.empty())	setCursor(_stage->nextCity(Owner::player, _select_tiles.front())); });
	// TODO IF MenuMode is not none, command change
	_menu->setFunction(Command::nextUnit, [this] { setCursor(_stage->nextUnit(Owner::player, _select_unit)); });
	// For debug
	_menu->setFunction(Command::talkStaff, [this, ai] { ai->evaluate(); });
	_menu->setFunction(Command::save, [this] {});
	_menu->setFunction(Command::load, [this] {});
	_menu->setFunction(Command::option, [this] {});

	//Move function
	setMoveFunction();

	//Attack function
	setAttackFunction();

	// Occupation function
	setOccupyFunction();

	// Spec function
	_menu->setFunction(Command::spec, [this] { _menu->showSpecFrame(_select_unit);});

	// Wait function
	_menu->setFunction(Command::wait, [this] { 
		if (_menu->getMode() != MenuMode::none)
			return;
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::wait, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area); 
	});
	_menu->setFunction(Command::wait_start, [this]
	{
		_select_unit->setState(EntityState::acted);
		_menu->getFunction(Command::wait_end)();
	});
	_menu->setFunction(Command::wait_end, [this]
	{
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::none, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		setCursor(_select_unit->getPositionAsTile());
	});

	// City supply function
	_menu->setFunction(Command::city_supply, [this] { 
		if (_menu->getMode() != MenuMode::none)
			return;
		_menu->setModeWithCheckAndMoveForCity(MenuMode::city_supply, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
	});
	_menu->setFunction(Command::city_supply_start, [this]
	{
		util::instance<City>(_select_tiles.back())->supply(_select_unit);
		_select_unit->setState(EntityState::supplied);
		_menu->getFunction(Command::city_supply_end)();
	});
	_menu->setFunction(Command::city_supply_end, [this]
	{
		_menu->setModeWithCheckAndMoveForCity(MenuMode::none, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		setCursor(_select_tiles.back()->getPositionAsTile());
	});

	// Deploy function
	_menu->setFunction(Command::deployment, [this]
	{
		if (_menu->getMode() != MenuMode::none)
			return;
		_menu->setModeWithCheckAndMoveForCity(MenuMode::deploy, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		_menu->deploy(util::instance<City>(_select_tiles.back()));
	});
	_menu->setFunction(Command::deploy_start, [this]
	{
		_stage->deployUnit(_select_unit, util::instance<City>(_select_tiles.back()));
		_select_unit = nullptr;
		_menu->getFunction(Command::deploy_end)();
	});
	_menu->setFunction(Command::deploy_end, [this]
	{
		_menu->setModeWithCheckAndMoveForCity(MenuMode::none, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		_menu->hideDeployers();
		setCursor(_select_tiles.back()->getPositionAsTile());
	});

	// Dispatch function
	_menu->setFunction(Command::dispatch, [this]
	{
		if (_menu->getMode() != MenuMode::none)
			return;
		_menu->setModeWithCheckAndMoveForCity(MenuMode::dispatch, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		_menu->dispatch(util::instance<City>(_select_tiles.back()));
	});
	_menu->setFunction(Command::dispatch_start, [this]
	{
		_stage->dispatchUnit(_menu->getSelectDeployer(), util::instance<City>(_select_tiles.back()));
		_menu->getFunction(Command::dispatch_cancel)();
	});
	_menu->setFunction(Command::dispatch_cancel, [this]
	{
		_menu->setModeWithCheckAndMoveForCity(MenuMode::none, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		_menu->hideDeployers();
		setCursor(_select_tiles.back()->getPositionAsTile());
	});

	stage->initStage(Owner::player);

    return true;
}


/*
 * Set move function
 */
void Game::setMoveFunction()
{
	_menu->setFunction(Command::move, [this]
	{
		if (_menu->getMode() != MenuMode::none)
			return;
		_select_area = _stage->moveCheck(_select_unit);
		_stage->moveView(_select_unit);
		for (auto tile : _select_area)
			_stage->blinkTile(tile, Color3B::WHITE);
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::move, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
	});
	_menu->setFunction(Command::move_start, [this]
	{
		_move_route = _stage->provisionalMoveUnit(_select_unit, _select_tiles.front());
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::moving, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
	});
	_menu->setFunction(Command::move_end, [this]
	{
		for (auto tile : _select_area)
			_stage->blinkOffTile(tile);
		_select_area.clear();
		_move_route.clear();
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::none, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		setCursor(_select_unit->getPositionAsTile());
	});
	_menu->setFunction(Command::move_decision, [this]
	{
		_stage->moveUnit(_select_unit, _move_route);
		_menu->getFunction(Command::move_end)();
	});
	_menu->setFunction(Command::move_cancel, [this]
	{
		_stage->provisionalMoveCancel(_select_unit);
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::move, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
	});
}

/*
 * Set attack function
 */
void Game::setAttackFunction()
{
	_menu->setFunction(Command::attack, [this]
	{
		if (_menu->getMode() != MenuMode::none)
			return;
		_menu->showWeaponFrame(_select_unit);
		_stage->moveView(_select_unit);
	});
	_menu->attack_decision = [this](WeaponData* data)
	{
		_select_weapon = data;
		_menu->attack_cancel(data);
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::attack, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		_select_area.clear();
		// Liner check
		if (data->getRange().direction_type == DirectionType::liner)
			_select_area = _stage->startRecursiveTileSearchForLiner(_select_unit->getPositionAsTile(), data->getRange().firing_range);
		// If full, should not select enemy
		else if (data->getRange().direction_type == DirectionType::full)
		{
			for (auto tile : _stage->startRecursiveTileSearch(_select_unit->getPositionAsTile(), data->getRange().firing_range, EntityType::counter))
			{
				auto target = _stage->getUnit(tile->getPositionAsTile());
				if (target && target->isSelectableEnemy(Owner::player))
				{
					_select_enemy = target;
					_menu->showEnemyUnit(_select_enemy);
					break;
				}
			}
			_menu->getFunction(Command::attack_target)();
			return;
		}
		else
			_select_area = _stage->startRecursiveTileSearch(_select_unit->getPositionAsTile(), data->getRange().firing_range, EntityType::counter);

		// Blink tile
		for (auto tile : _select_area)
			_stage->blinkTile(tile, Color3B::YELLOW);
	};
	_menu->attack_cancel = [this](WeaponData* data)
	{
		_menu->hideEnemyUnit();
		_menu->hideWeaponFrame();
	};
	_menu->setFunction(Command::attack_target, [this]
	{
		// Remove blink
		for (auto tile : _select_area)
			_stage->blinkOffTile(tile);

		_menu->setModeWithCheckAndMoveForUnit(MenuMode::attacking, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		// Show attack range
		_select_area = _stage->startRecursiveTileSearchForWeapon(_select_unit, _select_enemy, _select_weapon);
		for (auto tile : _select_area)
			_stage->blinkTile(tile, Color3B::RED);
	});
	_menu->setFunction(Command::attack_change, [this]
	{
		_menu->getFunction(Command::attack_end)();
		_menu->getFunction(Command::attack)();
	});
	_menu->setFunction(Command::attack_end, [this]
	{
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::none, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		_menu->hideEnemyUnit();
		// Remove blink
		for (auto tile : _select_area)
			_stage->blinkOffTile(tile);
		if (_select_enemy)
			_stage->blinkOffUnit(_select_enemy);
		_select_enemy = nullptr;
		setCursor(_select_unit->getPositionAsTile());
	});
	_menu->setFunction(Command::attack_start, [this]
	{
		for (auto tile : _select_area)
		{
			auto unit = _stage->getUnit(tile->getPositionAsTile());
			if (unit && unit->isSelectableEnemy(Owner::player))
				_select_unit->attack(unit, _select_weapon);
		}
		_select_unit->setState(EntityState::acted);
		_menu->getFunction(Command::attack_end)();
	});
	_menu->setFunction(Command::attack_cancel, [this]
	{
		if (_select_weapon->getRange().direction_type == DirectionType::full)
			_menu->getFunction(Command::attack_end)();
		else
		{
			_menu->getFunction(Command::attack_end)();
			_menu->attack_decision(_select_weapon);
		}
	});
}

/*
 * Set occupy function
 */
void Game::setOccupyFunction()
{
	_menu->setFunction(Command::occupation, [this]
	{
		if (_menu->getMode() != MenuMode::none)
			return;
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::occupy, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
	});
	_menu->setFunction(Command::occupation_start, [this]
	{
		_select_unit->setState(EntityState::acted);
		_select_unit->occupy(util::instance<City>(_select_tiles.back()));
		_menu->getFunction(Command::occupation_end)();
	});
	_menu->setFunction(Command::occupation_end, [this]
	{
		_menu->setModeWithCheckAndMoveForUnit(MenuMode::none, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
		setCursor(_select_unit->getPositionAsTile());
	});
}

/*
 * On tap function
 * Move cursor
 */
 void Game::onTap(cocos2d::Vec2 cor, std::vector<StageTile*> tiles)
{
	// If running menu action, do nothing
	if (_menu->isRunningAction())
		return;

	setCursor(cor);
}

/*
 * On long tap beganfunction
 * Show spec
 */
 void Game::onLongTapBagan(Vec2 cor, std::vector<StageTile*> tiles)
{
	auto unit = _stage->getUnit(cor);
	if (unit)
	{
		auto pre_unit = _select_unit;
		_select_unit = unit;
		callCommand(Command::spec);
		_select_unit = pre_unit;
	}
}

/*
 * On double tap function
 */
 void Game::onDoubleTap(Vec2 cor, std::vector<StageTile*> tiles)
{
	// Check tap same unit
	 auto target = _stage->getUnit(cor);
	bool isSameUnit = _select_unit && _select_unit == target;

	// Call on tap function, set unit and tile
	_stage->onTap(cor, tiles);


	switch (_menu->getMode())
	{
	case MenuMode::none: onDoubleTapByNone();				break;
	case MenuMode::move: onDoubleTapByMove(isSameUnit);		break;
	case MenuMode::moving:	onDoubleTapByMoving();			break;
	case MenuMode::attack:	onDoubleTapByAttack(isSameUnit);break;
	case MenuMode::attacking:	onDoubleTapByAttacking(target);	break;
	case MenuMode::occupy:
		callCommand(isSameUnit ? Command::occupation_start : Command::occupation_end);
		break;
	case MenuMode::wait:
		callCommand(isSameUnit ? Command::wait_start : Command::wait_end);
		break;
	case MenuMode::city_supply:
		callCommand(isSameUnit ? Command::city_supply_start : Command::city_supply_end);
		break;
	case MenuMode::deploy:
		callCommand(isSameUnit ? Command::deploy_start : Command::deploy_end);
		break;
	}
}

/*
 * On double tap function when menu mode is none
 */
void Game::onDoubleTapByNone()
{
	// When not select unit and select city, dispatch
	if (!_select_unit && util::findElement<StageTile*>(_select_tiles, [](StageTile* tile){return util::instanceof<City>(tile);}))
		callCommand(Command::dispatch);

	// When select ally unit, change process
	if(_select_unit && _select_unit->getAffiliation() == Owner::player)
		switch (_select_unit->getState())
		{
		case EntityState::none:		if (callCommand(Command::city_supply))		break;
		case EntityState::supplied:	if (callCommand(Command::move))				break;
		case EntityState::moved:	if (callCommand(Command::attack))			break;
									else if (callCommand(Command::occupation))	break;
									else if (callCommand(Command::wait))		break;
		case EntityState::acted:	if (callCommand(Command::deployment))		break;
									else if (callCommand(Command::nextUnit))	break;
		}

	// When select enemy unit, attack process
	// TODO double tap attack process is implement
	if (_select_unit && _select_unit->isSelectableEnemy(Owner::player))
	{
	}
}

/*
 * On double tap function when menu mode is move
 */
void Game::onDoubleTapByMove(bool isSameUnit)
{
	// When tap on same unit, change process
	if (isSameUnit)
	{
		
		callCommand(Command::move_end);
		if (callCommand(Command::attack))			return;
		else if (callCommand(Command::occupation))	return;
		else if (callCommand(Command::wait))		return;
		return;
	}

	// When tap on movable are, start to move. If not, stop to move.
	if (util::find(_select_area, _select_tiles.front()))
		callCommand(Command::move_start);
	else
		callCommand(Command::move_end);
}

/*
 * On double tap function when menu mode is moving
 */
void Game::onDoubleTapByMoving()
{
	// If double tap on end tile, end process
	if (util::find(_select_tiles, _move_route.back()))
		callCommand(Command::move_decision);
	// If double tap on movable tile, restart process
	else if (util::find(_select_area, _select_tiles.front()))
	{
		callCommand(Command::move_cancel);
		callCommand(Command::move_start);
	}
}

/*
 * On double tap function when menu mode is attack
 */
void Game::onDoubleTapByAttack(bool isSameUnit)
{
	// When tap on same unit, change process
	if (isSameUnit)
	{
		callCommand(Command::attack_end);
		if (callCommand(Command::occupation))	return;
		else if (callCommand(Command::wait))	return;
		return;
	}

	//When tap on targetable unit, targeting. If tap on out of range, end process
	if (_select_enemy && util::find<StageTile*, Entity*>(_select_area, _select_enemy, [](StageTile* tile, Entity *unit) { return tile->getPositionAsTile() == unit->getPositionAsTile(); }))
		callCommand(Command::attack_target);
	else if (!util::find(_select_area, _select_tiles.front()))
		callCommand(Command::attack_end);
}

/*
 * On double tap function when menu mode is attacking
 */
void Game::onDoubleTapByAttacking(Entity* target)
{
	if (target && util::find<StageTile*, Entity*>(_select_area, target, [](StageTile* tile, Entity *unit) { return tile->getPositionAsTile() == unit->getPositionAsTile(); }))
		callCommand(Command::attack_start);
	else if (!util::find(_select_area, _select_tiles.front()))
		callCommand(Command::attack_cancel);
}

/*
 * Call command function with check
 */
bool Game::callCommand(Command com)
{
	if (!command::isEnable(com, _select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area))
		return false;

	_menu->getFunction(com)();
	return true;
}

/*
 * Set cursol position
 */
void Game::setCursor(Vec2 cor)
{
	// Check whether out of range
	if (cor.x < 0 || cor.y < 0 || cor.x > _stage->getMapSize().x - 1 || cor.y > _stage->getMapSize().y - 1)
		return;

	// Select tile
	_stage->selectTile(cor);

	//Set unit and tile information
	setSelectUnit(_stage->getUnit(cor));
	setSelectTiles(_stage->getTiles(cor));

	//Set base information
	_menu->setCoordinateInformation(cor);

	// Check unit command
	_menu->checkUnitCommand(_select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
	_menu->checkCityCommand(_select_unit, _select_tiles, _select_enemy, _select_weapon, _select_area);
}

/*
 * Set select tile
 * And set tile information
 */
void Game::setSelectTiles(std::vector<StageTile*> tiles)
{
	switch (_menu->getMode())
	{
	case MenuMode::none:
	case MenuMode::move:
	case MenuMode::moving:
	case MenuMode::attack:
	case MenuMode::attacking:
	case MenuMode::wait:
		_select_tiles = tiles;
		_menu->setTileInformation(_select_tiles, _select_unit);
		break;
	case MenuMode::occupy:
	case MenuMode::city_supply:
	case MenuMode::deploy:
	case MenuMode::dispatch:
		break;
	}
}

/*
 * Set select unit
 * And set unit information
 */
//TODO BLINCK PROCESS
void Game::setSelectUnit(Entity * unit)
{
	// If same unit, do nothing
	if (unit && unit == _select_unit)
		return;

	// If invisible unit, not select
	if (unit && unit->getOpacity() == 0)
		unit = nullptr;

	switch (_menu->getMode())
	{
	case MenuMode::none:
		_select_unit = unit;
		_menu->setUnitInformation(_select_tiles, _select_unit);
		break;
	case MenuMode::move:
	case MenuMode::moving:
		break;
	case MenuMode::attack:
		// If player side, not select
		if (unit && OwnerInformation::getInstance()->isSameGroup(unit->getAffiliation(), Owner::player))
			unit = nullptr;

		if (unit)
			_menu->showEnemyUnit(unit);
		else
			_menu->hideEnemyUnit();

		_select_enemy = unit;
		break;
	case MenuMode::attacking:
	case MenuMode::occupy:
	case MenuMode::wait:
	case MenuMode::city_supply:
	case MenuMode::deploy:
	case MenuMode::dispatch:
		break;
	}
}

