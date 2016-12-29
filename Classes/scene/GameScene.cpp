#include "GameScene.h"

#include "ai/Owner.h"
#include "stage/Menu.h"
#include "stage/Stage.h"
#include "stage/Tile.h"
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
		if (menu->isRunningAction())
			return;
		//Set unit information
		auto unit = stage->getUnit(v.x, v.y);
		if (unit)
			setPreUnit(stage, menu, unit);

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
	menu->endPhase = [this]
	{
		_endFunction();
	};
	menu->nextCity = [this, stage, menu]
	{
		StageTile* tile = nullptr;
		for (auto t : _preTiles)
			if (t->getId())
				tile = t;
		auto pos = stage->nextCity(Owner::player, tile);
		setPreTiles(stage, menu, stage->getTiles(pos.x, pos.y));
		menu->setInfo(pos.x, pos.y);
	};
	menu->nextUnit = [this, stage, menu]
	{
		auto pos = stage->nextUnit(Owner::player, _preUnit);
		setPreUnit(stage, menu, stage->getUnit(pos.x, pos.y));
		menu->setInfo(pos.x, pos.y);
	};

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
	if(_preTiles.size() > 0)
		stage->blinkOffTile(_preTiles.back());

	_preTiles.clear();
	//Set tile information
	menu->setTile(tiles, _preUnit);
	stage->blinkTile(tiles.back());
	for (auto tile : tiles)
		_preTiles.push_back(tile);
}

void Game::setPreUnit(Stage * stage, MenuLayer * menu, Entity * unit)
{
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
