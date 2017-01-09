#include "GameScene.h"

#include "ai/Owner.h"
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
 * Initialize
 */
bool Game::init(Stage* stage)
{
    if ( !Layer::init() )
        return false;

	// Set stage
	this->addChild(stage);

	// Set menu
	auto menu = MenuLayer::create();
	menu->setStage(stage);
	this->addChild(menu, 10);

	// Set stgae listener
	stage->onTap = [this, menu, stage](Vec2 v, std::vector<StageTile*> tiles)
	{
		// If running menu action, do nothing
		if (menu->isRunningAction())
			return;

		setCursol(stage, menu, v);
	};
	stage->onLongTapBegan = [this, stage, menu](Vec2 v, std::vector<StageTile*> tiles)
	{
		auto unit = stage->getUnit(v);
		if (unit)
		{
			stage->onTap(v, tiles);
			auto preUnit = _selectUnit;
			_selectUnit = unit;
			menu->getFunction(Command::spec)();
			_selectUnit = preUnit;
		}
	};
	stage->onDoubleTap = [this, stage, menu](Vec2 v, std::vector<StageTile*> tiles)
	{
		auto unit = stage->getUnit(v);
		bool isSameUnit = _selectUnit == unit;

		stage->onTap(v, tiles);

		switch (menu->getMenuMode())
		{
		case MenuMode::none:
		{
			// If double tap on unit, move process
			if (unit && unit->getAffiliation() == Owner::player)
				switch (unit->getState())
				{
				case EntityState::none:
					if (command::isEnable(Command::city_supply, _selectUnit, _selectTiles))
					{
						menu->getFunction(Command::city_supply)();
						break;
					}
				case EntityState::supplied:
					if (command::isEnable(Command::move, _selectUnit, _selectTiles))
						menu->getFunction(Command::move)();
					break;
				case EntityState::moved:
					if (command::isEnable(Command::attack, _selectUnit, _selectTiles))
						menu->getFunction(Command::attack)();
					else if (command::isEnable(Command::occupation, _selectUnit, _selectTiles))
						menu->getFunction(Command::occupation)();
					else if (command::isEnable(Command::wait, _selectUnit, _selectTiles))
						menu->getFunction(Command::wait)();
					break;
				case EntityState::acted:
					if(command::isEnable(Command::deployment,_selectUnit, _selectTiles))
						menu->getFunction(Command::deployment)();
					else
						menu->getFunction(Command::nextUnit)();
					break;
				}
			// If double tap on enemy unit
			else if (unit && unit->getOpacity() != 0 && OwnerInformation::getInstance()->isSameGroup(unit->getAffiliation(), Owner::player))
			{
				for (auto tile : stage->startRecursiveTileSearch(v, 1, EntityType::counter))
				{
					auto around_unit = stage->getUnit(tile->getTileCoordinate());
					if (around_unit && around_unit->getAffiliation() == Owner::player && command::isEnable(Command::attack, around_unit, stage->getTiles(tile->getTileCoordinate())))
					{
						setSelectUnit(stage, menu, around_unit);
						menu->getFunction(Command::attack)();
					}
				}
			}
			else if (!unit)
			{
				City* city = nullptr;
				for (auto tile : tiles)
					if (tile && tile->getId() && util::instanceof<City>(tile))
						city = util::instance<City>(tile);
				if (city && command::isEnable(Command::dispatch, _selectUnit, _selectTiles))
					menu->getFunction(Command::dispatch)();
			}
			break;
		}
		case MenuMode::move:
			if (isSameUnit)
			{
				menu->getFunction(Command::move_end)();
				if (command::isEnable(Command::attack, _selectUnit, _selectTiles))
					menu->getFunction(Command::attack)();
				else if (command::isEnable(Command::occupation, _selectUnit, _selectTiles))
					menu->getFunction(Command::occupation)();
				else if (command::isEnable(Command::wait, _selectUnit, _selectTiles))
					menu->getFunction(Command::wait)();
				break;
			}
			// If double tap on movable tile, start process
			else if (util::find(_selectArea, tiles.back()))
				menu->getFunction(Command::move_start)();
			else
				menu->getFunction(Command::move_end)();
			break;
		case MenuMode::moving:
			// If double tap on end tile, end process
			if (util::find(tiles, _moveRoot.back()))
				menu->getFunction(Command::move_decision)();
			// If double tap on movable tile, restart process
			else if (util::find(_selectArea, tiles.back()))
			{
				menu->getFunction(Command::move_cancel)();
				menu->getFunction(Command::move_start)();
			}
			break;
		case MenuMode::attack:
			if (isSameUnit)
			{
				menu->getFunction(Command::attack_end)();
				if (command::isEnable(Command::occupation, _selectUnit, _selectTiles))
					menu->getFunction(Command::occupation)();
				else if (command::isEnable(Command::wait, _selectUnit, _selectTiles))
					menu->getFunction(Command::wait)();
				break;
			}
			else if (unit && util::find<StageTile*, Entity*>(_selectArea, unit, [](StageTile* tile, Entity *unit) { return tile->getTileCoordinate() == unit->getTileCoordinate(); }))
				menu->getFunction(Command::attack_target)();
			else if (!util::find(_selectArea, tiles.back()))
				menu->getFunction(Command::attack_end)();
			break;
		case MenuMode::attacking:
			if (unit && util::find<StageTile*, Entity*>(_selectArea, unit, [](StageTile* tile, Entity *unit) { return tile->getTileCoordinate() == unit->getTileCoordinate(); }))
				menu->getFunction(Command::attack_start)();
			else if (!util::find(_selectArea, tiles.back()))
				menu->getFunction(Command::attack_cancel)();
			break;
		case MenuMode::occupy:
			if (isSameUnit)
				menu->getFunction(Command::occupation_start)();
			else
				menu->getFunction(Command::occupation_end)();
			break;
		case MenuMode::wait:
			if (isSameUnit)
				menu->getFunction(Command::wait_start)();
			else
				menu->getFunction(Command::wait_end)();
			break;
		case MenuMode::city_supply:
			if (isSameUnit)
				menu->getFunction(Command::city_supply_start)();
			else
				menu->getFunction(Command::city_supply_end)();
			break;
		case MenuMode::deploy:
			if (isSameUnit)
				menu->getFunction(Command::deploy_start)();
			else
				menu->getFunction(Command::deploy_end)();
			break;
		}
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

	//Set menu listener
	//End phase
	menu->setFunction(Command::endPhase, [this] 
	{
		_endFunction();
	});
	//Next city
	menu->setFunction(Command::nextCity, [this, stage, menu]
	{
		StageTile* tile = nullptr;
		//Get top tile
		for (auto t : _selectTiles)
			if (t->getId())
				tile = t;
		setCursol(stage, menu, stage->nextCity(Owner::player, tile));
	});
	//Next unit
	menu->setFunction(Command::nextUnit, [this, stage, menu]
	{
		if (menu->getMenuMode() != MenuMode::none)
		{
			stage->movePosition(_selectUnit);
			return;
		}

		auto pos = stage->nextUnit(Owner::player, _selectUnit);
		if(pos != Vec2(0, 0))
			setCursol(stage, menu, pos);
	});

	//Move function
	menu->setFunction(Command::move, [this, stage, menu] 
	{
		_selectArea = stage->moveCheck(_selectUnit);
		stage->movePosition(_selectUnit);
		for (auto tile : _selectArea)
			stage->blinkTile(tile, Color3B::WHITE);
		menu->setMenuMode(MenuMode::move, _selectUnit, _selectTiles);
		menu->checkUnitCommand(_selectUnit, _selectTiles, util::find(_selectArea, _selectTiles.back()));
	});
	menu->setFunction(Command::move_start, [this, stage, menu] 
	{
		_moveRoot = stage->provisionalMoveUnit(_selectUnit, _selectTiles.back());
		menu->setMenuMode(MenuMode::moving, _selectUnit, _selectTiles);
		menu->checkUnitCommand(_selectUnit, _selectTiles);
	});
	menu->setFunction(Command::move_end, [this, stage, menu] 
	{
		for (auto tile : _selectArea)
			stage->blinkOffTile(tile);
		_selectArea.clear();
		_moveRoot.clear();
		menu->setMenuMode(MenuMode::none, _selectUnit, _selectTiles);
		setCursol(stage, menu, _selectUnit->getTileCoordinate());
	});
	menu->setFunction(Command::move_decision, [this, stage, menu] 
	{
		stage->moveUnit(_selectUnit, _moveRoot);
		menu->getFunction(Command::move_end)();
	});
	menu->setFunction(Command::move_cancel, [this, stage, menu] 
	{
		stage->provisionalMoveCancel(_selectUnit);
		menu->setMenuMode(MenuMode::move, _selectUnit, _selectTiles);
		menu->checkUnitCommand(_selectUnit, _selectTiles, util::find(_selectArea, _selectTiles.back()));
	});

	//Attack function
	menu->setFunction(Command::attack, [this, stage, menu] 
	{
		menu->showWeaponFrame(_selectUnit);
		stage->movePosition(_selectUnit);
	});
	menu->attack_decision = [this, stage, menu](WeaponData* data)
	{
		_weapon = data;
		menu->attack_cancel(data);
		menu->setMenuMode(MenuMode::attack, _selectUnit, _selectTiles);
		_selectArea.clear();
		// Liner check
		if (data->getRange().directionRange == DirectionRange::liner)
			_selectArea = stage->startRecursiveTileSearchForLiner(_selectUnit->getTileCoordinate(), data->getRange().FiringRange);
		// If full, should not select enemy
		else if (data->getRange().directionRange == DirectionRange::full)
		{
			for (auto tile : stage->startRecursiveTileSearch(_selectUnit->getTileCoordinate(), data->getRange().FiringRange, EntityType::counter))
			{
				auto target = stage->getUnit(tile->getTileCoordinate());
				if (target && target->getOpacity() != 0 && !OwnerInformation::getInstance()->isSameGroup(target->getAffiliation(), Owner::player))
				{
					_selectEnemy = target;
					menu->showEnemyUnit(_selectEnemy);
					break;
				}
			}
			menu->getFunction(Command::attack_target)();
			return;
		}
		else
			_selectArea = stage->startRecursiveTileSearch(_selectUnit->getTileCoordinate(), data->getRange().FiringRange, EntityType::counter);
		// Blink tile
		for (auto tile : _selectArea)
			stage->blinkTile(tile, Color3B::YELLOW);
	};
	menu->attack_cancel = [this, menu] (WeaponData* data)
	{
		menu->hideEnemyUnit();
		menu->hideWeaponFrame();
	};
	menu->setFunction(Command::attack_target, [this, stage, menu] 
	{
		// Remove blink
		for (auto tile : _selectArea)
			stage->blinkOffTile(tile);

		menu->setMenuMode(MenuMode::attacking, _selectUnit, _selectTiles);
		// Show attack range
		_selectArea = stage->startRecursiveTileSearchForWeapon(_selectUnit, _selectEnemy, _weapon);
		for (auto tile : _selectArea)
			stage->blinkTile(tile, Color3B::RED);
	});
	menu->setFunction(Command::attack_change, [this, stage, menu] 
	{
		menu->getFunction(Command::attack_end)();
		menu->getFunction(Command::attack)();
	});
	menu->setFunction(Command::attack_end, [this, stage, menu] 
	{
		menu->setMenuMode(MenuMode::none, _selectUnit, _selectTiles);
		menu->hideEnemyUnit();
		// Remove blink
		for (auto tile : _selectArea)
			stage->blinkOffTile(tile);
		if (_selectEnemy)
			stage->blinkOffUnit(_selectEnemy);
		_selectEnemy = nullptr;
		setCursol(stage, menu, _selectUnit->getTileCoordinate());
	});
	menu->setFunction(Command::attack_start, [this, stage, menu] 
	{
		for (auto tile : _selectArea)
		{
			auto unit = stage->getUnit(tile->getTileCoordinate());
			if (unit && unit->getOpacity() != 0 && !OwnerInformation::getInstance()->isSameGroup(unit->getAffiliation(), Owner::player))
				_selectUnit->attack(unit, _weapon);
		}
		_selectUnit->setState(EntityState::acted);
		menu->getFunction(Command::attack_end)();
		menu->checkUnitCommand(_selectUnit, _selectTiles);
	});	
	menu->setFunction(Command::attack_cancel, [this, stage, menu] 
	{
		if (_weapon->getRange().directionRange == DirectionRange::full)
			menu->getFunction(Command::attack_end)();
		else
		{
			menu->getFunction(Command::attack_end)();
			menu->attack_decision(_weapon);
		}
	});

	// Occupation function
	menu->setFunction(Command::occupation, [this, stage, menu] 
	{
		menu->setMenuMode(MenuMode::occupy, _selectUnit, _selectTiles);
		menu->checkUnitCommand(_selectUnit, _selectTiles, false);
	});
	menu->setFunction(Command::occupation_start, [this, stage, menu]
	{
		_selectUnit->setState(EntityState::acted);
		_selectUnit->occupy(util::instance<City>(_selectTiles.back()));
		menu->getFunction(Command::occupation_end)();
	});
	menu->setFunction(Command::occupation_end, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::none, _selectUnit, _selectTiles);
		menu->checkUnitCommand(_selectUnit, _selectTiles, false);
		setCursol(stage, menu, _selectUnit->getTileCoordinate());
	});

	// Spec function
	menu->setFunction(Command::spec, [this, stage, menu] 
	{
		menu->showSpecFrame(_selectUnit);
	});

	// Wait function
	menu->setFunction(Command::wait, [this, stage, menu] 
	{
		menu->setMenuMode(MenuMode::wait, _selectUnit, _selectTiles);
		menu->checkUnitCommand(_selectUnit, _selectTiles, false);
	});
	menu->setFunction(Command::wait_start, [this, stage, menu]
	{
		_selectUnit->setState(EntityState::acted);
		menu->getFunction(Command::wait_end)();
	});
	menu->setFunction(Command::wait_end, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::none, _selectUnit, _selectTiles);
		menu->checkUnitCommand(_selectUnit, _selectTiles, false);
		setCursol(stage, menu, _selectUnit->getTileCoordinate());
	});

	// City supply function
	menu->setFunction(Command::city_supply, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::city_supply, _selectUnit, _selectTiles);
		menu->checkCityCommand(_selectUnit, _selectTiles, util::instance<City>(_selectTiles.back()));
	});
	menu->setFunction(Command::city_supply_start, [this, stage, menu]
	{
		util::instance<City>(_selectTiles.back())->supply(_selectUnit);
		_selectUnit->setState(EntityState::supplied);
		menu->getFunction(Command::city_supply_end)();
	});
	menu->setFunction(Command::city_supply_end, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::none, _selectUnit, _selectTiles);
		menu->checkCityCommand(_selectUnit, _selectTiles, util::instance<City>(_selectTiles.back()));
		setCursol(stage, menu, _selectTiles.back()->getTileCoordinate());
	});

	// Deploy function
	menu->setFunction(Command::deployment, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::deploy, _selectUnit, _selectTiles);
		menu->checkCityCommand(_selectUnit, _selectTiles, util::instance<City>(_selectTiles.back()));
		menu->deploy(util::instance<City>(_selectTiles.back()));
	});
	menu->setFunction(Command::deploy_start, [this, stage, menu]
	{
		util::instance<City>(_selectTiles.back())->addDeoloyer(_selectUnit);
		stage->removeUnit(_selectUnit);
		_selectUnit = nullptr;
		menu->getFunction(Command::deploy_end)();
	});
	menu->setFunction(Command::deploy_end, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::none, _selectUnit, _selectTiles);
		menu->checkCityCommand(_selectUnit, _selectTiles, util::instance<City>(_selectTiles.back()));
		menu->hideDeployers();
		setCursol(stage, menu, _selectTiles.back()->getTileCoordinate());
	});

	// Dispatch function
	menu->setFunction(Command::dispatch, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::dispatch, _selectUnit, _selectTiles);
		menu->checkCityCommand(_selectUnit, _selectTiles, util::instance<City>(_selectTiles.back()));
		menu->dispatch(util::instance<City>(_selectTiles.back()));
	});
	menu->setFunction(Command::dispatch_start, [this, stage, menu]
	{
		auto city = util::instance<City>(_selectTiles.back());
		auto unit = city->getDeploter(menu->getSelectDeployer());
		stage->setUnit(city->getTileCoordinate(), unit);
		city->removeDeployer(unit);
		menu->getFunction(Command::dispatch_cancel)();
	});
	menu->setFunction(Command::dispatch_cancel, [this, stage, menu]
	{
		menu->setMenuMode(MenuMode::none, _selectUnit, _selectTiles);
		menu->checkCityCommand(_selectUnit, _selectTiles, util::instance<City>(_selectTiles.back()));
		menu->hideDeployers();
		setCursol(stage, menu, _selectTiles.back()->getTileCoordinate());
	});


	stage->initTileSearched(Owner::player);

    return true;
}

/*
 * Set cursol position
 */
void Game::setCursol(Stage * stage, MenuLayer * menu, cocos2d::Vec2 tileCoordinate)
{
	// Select tile
	stage->selectTile(tileCoordinate);

	//Set unit and tile information
	setSelectUnit(stage, menu, stage->getUnit(tileCoordinate));
	setSelectTiles(stage, menu, stage->getTiles(tileCoordinate));

	//Set base information
	menu->setInfo(tileCoordinate);

	// Check unit command
	switch (menu->getMenuMode())
	{
	case MenuMode::none:
		menu->checkUnitCommand(_selectUnit, _selectTiles, false);
		break;
	case MenuMode::move:
	case MenuMode::moving:
		menu->checkUnitCommand(_selectUnit, _selectTiles, util::find(_selectArea, _selectTiles.back()));
		break;
	case MenuMode::attack:
		menu->checkUnitCommand(_selectUnit, _selectTiles, util::find<StageTile*, Entity*>(_selectArea, _selectEnemy, [stage](StageTile* tile, Entity *enemy) 
		{
			return enemy && tile->getTag() == enemy->getTag();
		}));
		break;
	case MenuMode::attacking:
		break;
	case MenuMode::occupy:
		break;
	case MenuMode::wait:
		break;
	}
}

/*
 * Set pretile data
 * And reset pretile data
 */
void Game::setSelectTiles(Stage* stage, MenuLayer * menu, std::vector<StageTile*> tiles)
{
	switch (menu->getMenuMode())
	{
	case MenuMode::none:
	case MenuMode::move:
	case MenuMode::moving:
	case MenuMode::attack:
	case MenuMode::attacking:
	case MenuMode::wait:
		_selectTiles = tiles;
		menu->setTile(_selectTiles, _selectUnit);
		break;
	case MenuMode::occupy:
	case MenuMode::city_supply:
	case MenuMode::deploy:
	case MenuMode::dispatch:
		break;
	}
}

void Game::setSelectUnit(Stage * stage, MenuLayer * menu, Entity * unit)
{
	// If same unit, do nothing
	if (unit && unit == _selectUnit)
		return;
	// If invisible unit, not select
	if (unit && unit->getOpacity() == 0)
		unit = nullptr;

	switch (menu->getMenuMode())
	{
	case MenuMode::none:
		// Remove pre-unit's blink
		if (_selectUnit)
			stage->blinkOffUnit(_selectUnit);
		// Blink
		if(unit)
			stage->blinkUnit(unit);

		menu->setUnit(_selectTiles, unit);
		_selectUnit = unit;
		break;
	case MenuMode::move:
		break;
	case MenuMode::moving:
		break;
	case MenuMode::attack:
		// If player side, not select
		if (unit && OwnerInformation::getInstance()->isSameGroup(unit->getAffiliation(), Owner::player))
			unit = nullptr;

		// Remove pre-unit's blink
		if (_selectEnemy)
			stage->blinkOffUnit(_selectEnemy);
		// Blink
		if (unit)
		{
			stage->blinkUnit(unit);
			menu->showEnemyUnit(unit);
		}
		else
		{
			menu->hideEnemyUnit();
		}

		_selectEnemy = unit;
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
