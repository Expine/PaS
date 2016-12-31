﻿#include "GameScene.h"

#include "ai/Owner.h"
#include "stage/Menu.h"
#include "stage/Stage.h"
#include "stage/Tile.h"
#include "stage/Command.h"
#include "entity/Entity.h"
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

		// move mode
		if(_mode == GameMode::move)
			// enable move
			if (util::find(_moveTiles, tiles.back()))
				menu->setMenuMode(MenuMode::move, _preUnit, tiles ,true);
			// unenable move
			else
				menu->setMenuMode(MenuMode::move, _preUnit, tiles, false);

		//Set unit information
		auto unit = stage->getUnit(v.x, v.y);
		if (unit)
			setPreUnit(stage, menu, unit);

		//Set tile information
		setPreTiles(stage, menu, tiles);

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
		if (_mode == GameMode::move)
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
			_moveTiles = stage->moveCheck(_preUnit);
			for (auto tile : _moveTiles)
				stage->blinkTile(tile, Color3B::BLUE);
			_mode = GameMode::move;
			menu->setMenuMode(MenuMode::move, _preUnit, _preTiles, util::find(_moveTiles, _preTiles.back()));
		}
	});

	menu->setMoveFunction(MoveCommand::start, [this, stage, menu] {
		stage->moveUnit(_preUnit, _preTiles.back());
		_preUnit->setState(EntityState::moved);
		menu->getMoveFunction(MoveCommand::end)();
	});
	menu->setMoveFunction(MoveCommand::end, [this, stage, menu] {
		for (auto tile : _moveTiles)
			stage->blinkOffTile(tile);
		if (!_preTiles.empty())
			stage->blinkTile(_preTiles.back());
		_moveTiles.clear();
		_mode = GameMode::normal;
		menu->setMenuMode(MenuMode::unit, _preUnit, _preTiles, false);
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
	//Reset tile information
	if (_preTiles.size() > 0 && !util::find(_moveTiles, _preTiles.back()))
		stage->blinkOffTile(_preTiles.back());
	else if (!_preTiles.empty())
		stage->blinkChange(_preTiles.back(), Color3B::BLUE);

	_preTiles.clear();
	//Set tile information
	menu->setTile(tiles, _preUnit);
	if (!util::find(_moveTiles, tiles.back()))
		stage->blinkTile(tiles.back());
	else
		stage->blinkChange(tiles.back(), Color3B::WHITE);
	for (auto tile : tiles)
		_preTiles.push_back(tile);
}

void Game::setPreUnit(Stage * stage, MenuLayer * menu, Entity * unit)
{
	if (_mode == GameMode::move)
		return;

	// Remove pre-unit's blink
	if (_preUnit)
		stage->blinkOffUnit(_preUnit);

	// If tap same unit, only remove
	if (unit == _preUnit || unit->getOpacity() == 0)
		unit = nullptr;
	// else, create blink
	else
		stage->blinkUnit(unit);
	menu->setUnit(_preTiles, unit);
	_preUnit = unit;
}
