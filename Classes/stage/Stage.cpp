#include "Stage.h"
#include "Tile.h"

#include "entity/Entity.h"
#include "entity/EntityToTile.h"
#include "util/Util.h"
#include "util/MultiListener.h"

USING_NS_CC;

/*
 * Set tile data
 * And return this tile data
 */
StageTile* StageLayer::setTile(int x, int y, int id)
{
	auto tile = StageTile::create(id, x, y, _batch, dynamic_cast<Stage*>(getParent()));
	_batch->addChild(tile);
	return tile;
}

/*
* Get tile data
*/
StageTile * StageLayer::getTile(int x, int y)
{
	return dynamic_cast<StageTile*>(getChildByTag(0)->getChildByTag(x * _mapSize.y + y));
};

/*
 * Remove tile data
 */
StageTile * StageLayer::removeTile(int x, int y)
{
	auto remove = dynamic_cast<StageTile*>(getChildByTag(0)->getChildByTag(x * _mapSize.y + y));
	getChildByTag(0)->removeChildByTag(x * _mapSize.y + y);
	return remove;
};

/*********************************************************/

/*
 * Set unit data
 * And return this unit data
 */
Entity* UnitLayer::setUnit(int x, int y, EntityType type)
{
	auto entity = Entity::create(type, x, y, _batch, dynamic_cast<Stage*>(getParent()));
	_batch->addChild(entity);
	return entity;
}

/*
 * Get unit data
 */
Entity * UnitLayer::getUnit(int x, int y)
{
	return dynamic_cast<Entity*>(getChildByTag(0)->getChildByTag(x * _mapSize.y + y));
}

/*********************************************************/

/*
 * Static Method
 * Parse file to stage.
 * Set stage base data and layer, chip data
 */
Stage * Stage::parseStage(const std::string file)
{
	auto stage = Stage::create();
	auto lines = util::splitFile(file);
	auto fLine = util::splitString(lines[0], ',');
	auto sLine = util::splitString(lines[1], ',');
	
	//Set stage data
	stage->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	stage->setMapSize(Vec2(std::atoi(fLine[0].c_str()), std::atoi(fLine[1].c_str())));
	stage->setChipSize(Vec2(std::atoi(fLine[2].c_str()), std::atoi(fLine[3].c_str())));
	stage->setGap(std::atoi(fLine[4].c_str()));
	stage->setBackGround(fLine[5]);
	stage->setBGM(fLine[6]);
	stage->setTileFile(fLine[7]);

	//Set Layerdata
	for (int i = 0; i < 3; i++)
	{
		auto layer = StageLayer::create();
		layer->setTag(i);
		layer->setMapSize(stage->getMapSize());
		stage->addChild(layer);
	}

	// Set shadow layer
	auto shadow = StageLayer::create();
	shadow->setTag(4);
	shadow->setMapSize(stage->getMapSize());
	auto batch = SpriteBatchNode::create("TileSet/" + stage->getTileFile());
	batch->setTag(0);
	shadow->setBatch(batch);
	shadow->addChild(batch);
	stage->addChild(shadow);

	// Set unit layer
	auto layer = UnitLayer::create();
	layer->setTag(3);
	layer->setMapSize(stage->getMapSize());
	batch = SpriteBatchNode::create("image/unit.png");
	batch->setTag(0);
	layer->setBatch(batch);
	layer->addChild(batch);
	stage->addChild(layer);

	//Set chip data
	int count = 0;
	for (int l = 0; l < 3; l++)
	{
		auto batch = SpriteBatchNode::create("TileSet/" + stage->getTileFile());
		batch->setTag(0);
		auto layer = stage->getStageLayer(l);
		layer->setBatch(batch);
		layer->addChild(batch);
		for (int x = 0; x < stage->getMapSize().x; x++)
		{
			for (int y = 0; y < stage->getMapSize().y; y++)
			{
				if (sLine.size() == count)
					break;
				if (std::atoi(sLine[count].c_str()) == 0)
					continue;

				auto tile = layer->setTile(x, y, std::atoi(sLine[count++].c_str()));
				// Add city
				if (util::instanceof<City>(tile))
					stage->_cities[util::instance<City>(tile)->getOwner()].push_back(util::instance<City>(tile));
			}
		}
	}

	// for debug 
	Owner names[] = { Owner::player, Owner::enemy };
	std::vector<Vec2> poses;
	bool check = true;
	for (int i = 0; i < 100; i++)
	{
		auto pos = Vec2(std::rand() % (int)(stage->getMapSize().x), std::rand() % (int)(stage->getMapSize().y));
		auto type = stage->getTile(0, pos.x, pos.y)->getTerrainType();
		if (type == TerrainType::ocean)
			continue;
		if (type == TerrainType::river)
			continue;
		for (auto p : poses)
			if (p.x == pos.x && p.y == pos.y)
			{
				check = false;
			}
		if (check)
		{
			stage->setUnit(pos.x, pos.y, static_cast<EntityType>(std::rand() % static_cast<int>(EntityType::COUNT)), names[i % 2]);
			if (i % 2)
				stage->getUnit(pos.x, pos.y)->setColor(Color3B::RED);
			poses.push_back(pos);
		}
		check = true;
	}

	return stage;
}

/*
 * Initialize information.
 * Mainly set listener.
 */
bool Stage::init()
{
	if (!Node::init())
		return false;

	//Set listener
	auto listener = MultiTouchListener::create();
	//Stop all action and adjust position. 
	listener->onTouchBegan = [this](Vec2 v)
	{
		stopAllActions();
		setPosition(adjustArea(getPosition()));
	};
	//When tapping, call onTap
	listener->onTap = [this](Vec2 v)
	{
		if (onTap)
		{
			auto cor = getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (auto tile : getTiles(cor.x, cor.y))
				if (tile->getId() != 0)
					tiles.push_back(tile);
			onTap(cor, tiles);
		}
	};
	//if has on-Check, call it
	listener->onSwipeCheck = [this](Vec2 v, Vec2 diff, float time)	{ return !onSwipeCheck || onSwipeCheck(v, diff, time); };
	listener->onFlickCheck = [this](Vec2 v, Vec2 diff, float time) { return !onFlickCheck || onFlickCheck(v, diff, time); };
	// When swiping, move position
	listener->onSwipe = [this](Vec2 v, Vec2 diff, float time) 
	{ 
		setPosition(adjustArea(getPosition() + diff)); 
	};
	// When flicking, move position by inertia
	listener->onFlick = [this](Vec2 v, Vec2 diff, float time)
	{
		runAction(Spawn::create(
			Sequence::create(
				EaseSineOut::create(MoveBy::create(0.5f, diff / time / 3)),
				CallFunc::create([this] {
					stopAllActions();
				}),
			NULL),
			Repeat::create(Sequence::create(
				CallFunc::create([this] {
					setPosition(adjustArea(getPosition()));
				}),
				DelayTime::create(0.005f),
			NULL), -1),
		NULL));
	};
	//When onLongtapBegan, call onLongTapBegan
	listener->onLongTapBegan = [this](Vec2 v)
	{
		if (onLongTapBegan)
		{
			auto cor = getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			onLongTapBegan(cor, tiles);
		}
	};
	//When onLongtapEnd, call onLongTapEnd
	listener->onLongTapEnd = [this](Vec2 v)
	{
		if (onLongTapEnd)
		{
			auto cor = getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			onLongTapEnd(cor, tiles);
		}
	};
	// Pinch in equals zoom in
	listener->pinchIn = [this](Vec2 v, float ratio)
	{
		ratio = ratio / (1 + ratio) / STAGE_RATIO_RATIO;
		auto diff_ratio = getScale();
		setScale(adjustRatio(getScale() * ratio));
		diff_ratio = getScale() / diff_ratio;
		auto diff_x = v.x * (1 - diff_ratio);
		auto diff_y = v.y * (1 - diff_ratio);
		auto pos = (getPosition() + Vec2(diff_x, diff_y));
		setPosition(adjustArea(getPosition()  * diff_ratio + Vec2(diff_x, diff_y)));
	};
	// Pinch out equals zoom out
	listener->pinchOut = [this](Vec2 v, float ratio)
	{
		ratio = ratio / (1 + ratio) / STAGE_RATIO_RATIO;
		auto diff_ratio = getScale();
		setScale(adjustRatio(getScale() * ratio));
		diff_ratio = getScale() / diff_ratio;
		auto diff_x = v.x * (1 - diff_ratio);
		auto diff_y = v.y * (1 - diff_ratio);
		auto pos = (getPosition() + Vec2(diff_x, diff_y));
		setPosition(adjustArea(getPosition()  * diff_ratio + Vec2(diff_x, diff_y)));
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	// Set mouse listener(for Windows)
	auto mouseListener = MultiMouseListener::create(listener);
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);

	return true;
}

/*
 * Get coordinate on tile
 */
Vec2 Stage::getTileCoordinate(Vec2 cor)
{
	auto fix = (cor - getPosition()) / getScale();
	fix.y = getHeight() - fix.y;
	auto even = (int)fix.x % (int)(_chipSize.x + _gap) <= (_chipSize.x + _gap) / 2;
	auto x = (int)(fix.x / (_chipSize.x + _gap));
	auto y = (int)((fix.y - (even ? 0 : _chipSize.y / 2)) / _chipSize.y) * 2 + (even ? 0 : 1);
	return Vec2(x, y);
}

/*
* Get coordinate by tile coordinate
*/
Vec2 Stage::getCoordinateByTile(int x, int y)
{
	return Vec2(x * (_chipSize.x + _gap) + (y % 2) * (_chipSize.x + _gap) / 2, (_mapSize.y - 1 - y) * _chipSize.y / 2);
}

/*
 * Move for centering specify tile
 */
void Stage::movePosition(int x, int y)
{
	// Stop pre move
	if (this->getActionByTag(0))
		this->stopActionByTag(0);

	auto winSize = Director::getInstance()->getWinSize();
	auto action = MoveTo::create(0.1f, adjustArea(Vec2(winSize.width / 2 - (getChipSize().x + getGap()) / 2, winSize.height / 2 - getChipSize().y / 2) - Vec2(x, y) * getScale()));
	action->setTag(0);
	this->runAction(action);
}

/*
 * Initialize searched property
 */
void Stage::initTileSearched(Owner owner)
{
	// All black out
	// All enemy hided
	auto shadow = getShadowLayer();
	for (int x = 0; x < _mapSize.x; x++)
	{
		for (int y = 0; y < _mapSize.y; y++)
		{
			for (auto tile : getTiles(x, y))
				tile->setSearched(false);
			if (shadow->getTile(x, y) == nullptr)
				shadow->setTile(x, y, 22);
			auto unit = getUnit(x, y);
			if (unit && unit->getAffiliation() != owner)
				unit->setOpacity(0);
		}
	}


	for (auto city : _cities[owner])
	{
		for (auto tile : recursiveTileSearch(Vec2(0, 0), city->getTileCoordinate(_mapSize.y), 1, EntityType::sight))
		{
			auto cor = tile->getTileCoordinate(_mapSize.y);
			tile->setSearched(true);
			if (shadow->getTile(cor.x, cor.y) != nullptr)
				shadow->removeTile(cor.x, cor.y);
			auto unit = getUnit(cor.x, cor.y);
			if (unit && unit->getAffiliation() != owner)
				unit->setOpacity(255);
		}
	}

	for (auto unit : _units[owner])
	{
		for (auto tile : recursiveTileSearch(Vec2(0, 0), unit->getTileCoordinate(_mapSize.y), unit->getSearchingAbility(), EntityType::sight))
		{
			auto cor = tile->getTileCoordinate(_mapSize.y);
			tile->setSearched(true);
			if (shadow->getTile(cor.x, cor.y) != nullptr)
				shadow->removeTile(cor.x, cor.y);
			auto unit = getUnit(cor.x, cor.y);
			if (unit && unit->getAffiliation() != owner)
				unit->setOpacity(255);
		}
	}
}
/*
* Recursive search
*/
std::vector<StageTile*> Stage::recursiveTileSearch(Vec2 intrusion, Vec2 point, int remainCost, EntityType type)
{
	auto cost = 0;
	auto tiles = getTiles(point.x, point.y);
	for (auto tile : tiles)
		cost += EntityToTile::getInstance()->getSearchCost(tile->getTerrainType(), type);

	CCLOG("Cost %d", cost);

	// If consume all cost, process end
	if (intrusion != Vec2(0, 0) && (remainCost -= cost) < 0)
		return std::vector<StageTile*>();

	//Up
	if (intrusion.y >= 0)
		for (auto tile : recursiveTileSearch(Vec2(0, 1), point + Vec2(0, -2), remainCost, type))
			tiles.push_back(tile);
	//Up right
	if (intrusion.x == -1 || (intrusion.x == 0 && intrusion.y != -1))
		for (auto tile : recursiveTileSearch(Vec2(-1, 1), point + Vec2((int)(point.y) % 2, -1), remainCost, type))
			tiles.push_back(tile);
	//Up left
	if (intrusion.x == 1 || (intrusion.x == 0 && intrusion.y != -1))
		for (auto tile : recursiveTileSearch(Vec2(1, 1), point + Vec2(((int)(point.y) % 2) - 1, -1), remainCost, type))
			tiles.push_back(tile);
	//Down
	if (intrusion.y <= 0)
		for (auto tile : recursiveTileSearch(Vec2(0, -1), point + Vec2(0, 2), remainCost, type))
			tiles.push_back(tile);
	//Down right
	if (intrusion.x == -1 || (intrusion.x == 0 && intrusion.y != 1))
		for (auto tile : recursiveTileSearch(Vec2(-1, -1), point + Vec2((int)(point.y) % 2, 1), remainCost, type))
			tiles.push_back(tile);
	//Down left
	if (intrusion.x == 1 || (intrusion.x == 0 && intrusion.y != 1))
		for (auto tile : recursiveTileSearch(Vec2(1, -1), point + Vec2((int)(point.y) % 2 - 1, 1), remainCost, type))
			tiles.push_back(tile);

	return tiles;
}

/*
 * Get tiles by x, y
 */
std::vector<StageTile*> Stage::getTiles(int x, int y)
{
	std::vector<StageTile*> tiles;
	for (int i = 0; i < 3; i++)
	{
		auto tile = getTile(i, x, y);
		if (!tile)
			continue;
		tiles.push_back(tile);
	}
	return tiles;
}

/*
 * Blink tile
 */
void Stage::blinkTile(StageTile* tile)
{
	Sprite* white;
	auto cor = tile->getTileCoordinate(_mapSize.y);
	auto shadow = getShadowLayer();
	auto shadow_tile = shadow->getTile(cor.x, cor.y);
	if (shadow_tile)
	{
		auto tex = tile->getTexture();
		white = Sprite::createWithTexture(tex, Rect(tex->getContentSize().width - _chipSize.x, tex->getContentSize().height - _chipSize.y, _chipSize.x, _chipSize.y));
		shadow_tile->addChild(white);
	}
	else
	{
		white = shadow->setTile(cor.x, cor.y, 23);
	}

	white->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	white->setOpacity(0);
	white->runAction(RepeatForever::create(Sequence::create(
		EaseExponentialIn::create(FadeTo::create(0.5f, 200)),
		EaseExponentialOut::create(FadeTo::create(0.5f, 0)),
		NULL)));
}

/*
 * Stop to blink tile
 */
void Stage::blinkOffTile(StageTile* tile)
{
	auto cor = tile->getTileCoordinate(_mapSize.y);
	auto shadow = getShadowLayer();
	auto shadow_tile = shadow->getTile(cor.x, cor.y);
	if (shadow_tile)
		if (shadow_tile->getId() == 23)
			shadow_tile->removeFromParent();
		else
			shadow_tile->removeAllChildren();
}

/*
 * Set unit
 */
void Stage::setUnit(int x, int y, EntityType type, const Owner owner)
{
	_units[owner].push_back(getUnitLayer()->setUnit(x, y, type)->setAffiliationRetThis(owner));
}

/*
 * Blink unit
 */
void Stage::blinkUnit(Entity * unit)
{
	auto inner = Sprite::createWithTexture(unit->getTexture(), Rect(32, static_cast<int>(unit->getType()) * 32, 32, 32));
	inner->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	inner->setOpacity(0);
	inner->runAction(RepeatForever::create(Sequence::create(
		EaseExponentialIn::create(FadeTo::create(0.5f, 100)),
		EaseExponentialOut::create(FadeTo::create(0.5f, 0)),
		NULL)));
	unit->addChild(inner);
}

/*
 * Stop to blink unit
 */
void Stage::blinkOffUnit(Entity * unit)
{
	unit->removeAllChildren();
}


/*
 * Adjust position
 */
cocos2d::Vec2 Stage::adjustArea(cocos2d::Vec2 v)
{
	auto maxWidth = getWidth() * getScale() - cocos2d::Director::getInstance()->getWinSize().width;
	auto maxHeight = getHeight() * getScale() - cocos2d::Director::getInstance()->getWinSize().height;
	return cocos2d::Vec2(
		(maxWidth < 0) ? 0 : max(-maxWidth, min(v.x, 0)),
		(maxHeight < 0) ? 0 : max(-maxHeight, min(v.y, 0)));
};

/*
 * Move next city
 */
Vec2 Stage::nextCity(Owner owner, StageTile* nowTile)
{
	auto cities = _cities[owner];
	if (!nowTile || !util::instanceof<City>(nowTile))
		nowTile = cities.back();
	bool discovered = false;
	for (auto city : cities)
	{
		if (discovered)
		{
			movePosition(city->getPosition().x, city->getPosition().y);
			return city->getTileCoordinate(getMapSize().y);
		}
		if (city == nowTile)
			discovered = true;
	}
	movePosition(cities.front()->getPosition().x, cities.front()->getPosition().y);
	return cities.front()->getTileCoordinate(getMapSize().y);
}

/*
 * Move next unit
 */
Vec2 Stage::nextUnit(Owner owner, Entity* nowUnit)
{
	auto units = _units[owner];
	if (!nowUnit)
		nowUnit = units.back();
	bool discovered = false;
	for (auto unit : units)
	{
		if (discovered)
		{
			movePosition(unit->getPosition().x, unit->getPosition().y);
			return Vec2(unit->getTag() / (int)(getMapSize().y), unit->getTag() % (int)(getMapSize().y));
		}
		if (unit == nowUnit)
			discovered = true;
	}
	movePosition(units.front()->getPosition().x, units.front()->getPosition().y);
	return Vec2(units.front()->getTag() / (int)(getMapSize().y), units.front()->getTag() % (int)(getMapSize().y));
}

void Stage::moveCheck(Entity * entity)
{
	CCLOG("MoveCheck");
	auto cor = entity->getTileCoordinate(_mapSize.y);
	movePosition(cor.x, cor.y);

	auto tiles = recursiveTileSearch(Vec2(0, 0), cor, entity->getMobility(), entity->getType());
	for (auto tile : tiles)
	{
		if (tile->getId() == 0)
			continue;
		blinkTile(tile);
	}
}
