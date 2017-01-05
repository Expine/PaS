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

		//Set unit information
		auto unit = stage->getUnit(v.x, v.y);
		setPreUnit(stage, menu, unit);

		//Set tile information
		setPreTiles(stage, menu, tiles);

		//Set base information
		menu->setInfo(v.x, v.y);
	};
	stage->onDoubleTap = [this, stage, menu](Vec2 v, std::vector<StageTile*> tiles)
	{
		stage->onTap(v, tiles);
		auto unit = stage->getUnit(v.x, v.y);
		if (_mode == GameMode::normal)
		{
			// If double tap on unit, move process
			if (unit && unit->getAffiliation() == Owner::player)
			{
				if (unit->getState() < EntityState::moved)
				{
					if (EntityInformation::getInstance()->getCommand(_preUnit->getType(), UnitCommand::move) && command::isEnable(UnitCommand::move, _preUnit, tiles))
						menu->getUnitFunction(UnitCommand::move)();
				}
				else if (unit->getState() == EntityState::moved)
				{
					if (EntityInformation::getInstance()->getCommand(_preUnit->getType(), UnitCommand::attack) && command::isEnable(UnitCommand::attack, _preUnit, tiles))
						menu->getUnitFunction(UnitCommand::attack)();
				}
			}
			else if (unit)
			{
				for (auto tile : stage->startRecursiveTileSearch(v, 1, EntityType::counter))
				{
					auto pos = tile->getTileCoordinate(stage->getMapSize().y);
					auto own = stage->getUnit(pos.x, pos.y);
					if (own && own->getAffiliation() == Owner::player)
						if (EntityInformation::getInstance()->getCommand(own->getType(), UnitCommand::attack) && command::isEnable(UnitCommand::attack, own, tiles))
						{
							setPreUnit(stage, menu, own);
							menu->getUnitFunction(UnitCommand::attack)();
						}
				}
			}
		}
		else if (_mode == GameMode::move)
		{
			// If double tap on movable tile, start process
			if (util::find(_moveTiles, tiles.back()))
				menu->getMoveFunction(MoveCommand::start)();
			else
				menu->getMoveFunction(MoveCommand::end)();
		}
		else if (_mode == GameMode::moving)
		{
			if (util::find(tiles, _moveRoot.back()))
				menu->getMoveFunction(MoveCommand::decision)();
			else if(util::find(_moveTiles, tiles.back()))
			{
				menu->getMoveFunction(MoveCommand::cancel)();
				menu->getMoveFunction(MoveCommand::start)();
			}
		}
		else if (_mode == GameMode::attack)
		{

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
	menu->endPhase = [this]
	{
		_endFunction();
	};
	//Next city
	menu->nextCity = [this, stage, menu]
	{
		StageTile* tile = nullptr;
		//Get top tile
		for (auto t : _preTiles)
			if (t->getId())
				tile = t;
		auto pos = stage->nextCity(Owner::player, tile);
		setPreTiles(stage, menu, stage->getTiles(pos.x, pos.y));
		menu->setInfo(pos.x, pos.y);
	};
	//Next unit
	menu->nextUnit = [this, stage, menu]
	{
		//if move mode, move to moving unit
		if (_mode != GameMode::normal)
		{
			stage->movePosition(_preUnit->getPosition().x, _preUnit->getPosition().y);
			return;
		}

		auto pos = stage->nextUnit(Owner::player, _preUnit);
		setPreUnit(stage, menu, stage->getUnit(pos.x, pos.y));
		menu->setInfo(pos.x, pos.y);
	};

	//Move function
	menu->setUnitFunction(UnitCommand::move, [this, stage, menu] {
		if (_mode == GameMode::normal)
		{
			auto pos = _preUnit->getPosition();
			_moveTiles = stage->moveCheck(_preUnit);
			stage->movePosition(pos.x, pos.y);
			for (auto tile : _moveTiles)
				stage->blinkTile(tile, Color3B::WHITE);
			_mode = GameMode::move;
			menu->setMenuMode(MenuMode::move, _preUnit, _preTiles, util::find(_moveTiles, _preTiles.back()));
		}
	});

	menu->setMoveFunction(MoveCommand::start, [this, stage, menu] {
		_moveRoot = stage->provisionalMoveUnit(_preUnit, _preTiles.back());
		_mode = GameMode::moving;
		menu->setMenuMode(MenuMode::moving, _preUnit, _preTiles, false);
	});
	menu->setMoveFunction(MoveCommand::end, [this, stage, menu] {
		for (auto tile : _moveTiles)
			stage->blinkOffTile(tile);
		_moveTiles.clear();
		_moveRoot.clear();
		_mode = GameMode::normal;
		menu->setMenuMode(MenuMode::unit, _preUnit, _preTiles, false);
	});
	menu->setMoveFunction(MoveCommand::decision, [this, stage, menu] {
		stage->moveUnit(_preUnit, _moveRoot);
		menu->getMoveFunction(MoveCommand::end)();
	});
	menu->setMoveFunction(MoveCommand::cancel, [this, stage, menu] {
		stage->provisionalMoveCancel(_preUnit);
		_mode = GameMode::move;
		menu->setMenuMode(MenuMode::move, _preUnit, _preTiles, true);
	});

	//Attack function
	menu->setUnitFunction(UnitCommand::attack, [this, stage, menu] {
		auto pos = _preUnit->getPosition();
		menu->showWeaponFrame(_preUnit);
		stage->movePosition(pos.x, pos.y);
	});
	menu->attack_decision = [this, stage, menu](WeaponData* data)
	{
		_mode = GameMode::attack;
		menu->setMenuMode(MenuMode::attack, _enemy, _preTiles, false);
		menu->attack_cancel(data);
//		_moveTiles = stage->startRecursiveTileSearchForWeapon(_preUnit, _enemy, data);
		if (data->getRange().directionRange == DirectionRange::liner)
			_moveTiles = stage->startRecursiveTileSearchForLiner(_preUnit->getTileCoordinate(stage->getMapSize().y), data->getRange().FiringRange);
		else
			_moveTiles = stage->startRecursiveTileSearch(_preUnit->getTileCoordinate(stage->getMapSize().y), data->getRange().FiringRange, EntityType::counter);
		for (auto tile : _moveTiles)
			stage->blinkTile(tile, Color3B::YELLOW);
	};
	menu->attack_cancel = [this, menu] (WeaponData* data)
	{
		menu->hideEnemyUnit();
		menu->hideWeaponFrame();
	};
	menu->setAttackFunction(AttackCommand::decision, [this, stage, menu] {
		_mode = GameMode::normal;
		menu->getAttackFunction(AttackCommand::cancel)();
	});
	menu->setAttackFunction(AttackCommand::cancel, [this, stage, menu] {
		if(_mode == GameMode::attack	)
			menu->showWeaponFrame(_preUnit);
		_mode = GameMode::normal;
		menu->setMenuMode(MenuMode::unit, _preUnit, _preTiles, false);
		menu->hideEnemyUnit();
		for (auto tile : _moveTiles)
			stage->blinkOffTile(tile);
		_moveTiles.clear();
		if (_enemy)
			stage->blinkOffUnit(_enemy);
		_enemy = nullptr;
	});

	stage->initTileSearched(Owner::player);

    return true;
}

/*
 * Set pretile data
 * And reset pretile data
 */
void Game::setPreTiles(Stage* stage, MenuLayer * menu, std::vector<StageTile*> tiles)
{
	// move mode
	if (_mode == GameMode::move)
		menu->setMenuMode(MenuMode::move, _preUnit, tiles, util::find(_moveTiles, tiles.back()));

	//Reset tile information
	/*
	if (_preTiles.size() > 0 && !util::find(_moveTiles, _preTiles.back()))
		stage->blinkOffTile(_preTiles.back());
	else if (!_preTiles.empty())
		stage->blinkChange(_preTiles.back(), Color3B::WHITE);
		*/

	_preTiles.clear();
	//Set tile information
	menu->setTile(tiles, _preUnit);
	auto pos = tiles.back()->getTileCoordinate(stage->getMapSize().y);
	stage->selectTile(pos.x, pos.y);
	/*
	if (!util::find(_moveTiles, tiles.back()))
//		stage->blinkTile(tiles.back());
	else
		stage->blinkChange(tiles.back(), Color3B::WHITE);
	*/
	for (auto tile : tiles)
		_preTiles.push_back(tile);
}

void Game::setPreUnit(Stage * stage, MenuLayer * menu, Entity * unit)
{
	if (_mode == GameMode::move || _mode == GameMode::moving)
		return;

	if (_mode == GameMode::normal)
	{
		// If tap same unit, only remove
		if (!unit || unit == _preUnit || unit->getOpacity() == 0)
			return;
		// else, create blink
		else
			stage->blinkUnit(unit);

		// Remove pre-unit's blink
		if (_preUnit)
			stage->blinkOffUnit(_preUnit);

		menu->setUnit(_preTiles, unit);
		_preUnit = unit;
	}
	else if (_mode == GameMode::attack)
	{
		// If tap same unit, only remove
		if (!unit || unit->getOpacity() == 0 || unit->getAffiliation() == Owner::player)
			unit = nullptr;
		// else, create blink
		else
			stage->blinkUnit(unit);

		// Remove pre-unit's blink
		if (_enemy)
			stage->blinkOffUnit(_enemy);
		if (unit)
			menu->showEnemyUnit(unit);
		else
			menu->hideEnemyUnit();
		_enemy = unit;
		menu->setMenuMode(MenuMode::attack, _enemy, _preTiles, false);
	}

}
