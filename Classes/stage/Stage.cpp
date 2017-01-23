#include "Stage.h"
#include "Tile.h"

#include "entity/Entity.h"
#include "entity/EntityToTile.h"
#include "entity/Weapon.h"
#include "util/Util.h"
#include "util/MultiListener.h"

USING_NS_CC;

/*
 * Constructor of stage layer
 */
StageLayer::StageLayer()
	: _batch(nullptr)
{}

/*
 * Destructor of stage layer
 */
StageLayer::~StageLayer()
{
	_batch = nullptr;
}

/*
 * Get stage instance by get parent
 */
Stage * StageLayer::getStage()
{
	return dynamic_cast<Stage*>(getParent());
}

/*
 * Set tile data
 * And return this tile data
 */
StageTile* StageLayer::setTile(Vec2 cor, int id)
{
	auto tile = StageTile::create(id, cor, getStage());
	_batch->addChild(tile);
	return tile;
}

/*
* Get tile data
*/
StageTile * StageLayer::getTile(int x, int y)
{
	return util::instance<StageTile>(getChildByTag(0)->getChildByTag(x * _map_size.y + y));
}

/*
 * Remove tile data
 */
StageTile * StageLayer::removeTile(int x, int y)
{
	auto remove = util::instance<StageTile>(getChildByTag(0)->getChildByTag(x * _map_size.y + y));
	getChildByTag(0)->removeChild(remove);
	return remove;
};

/***********************************************************************************************/

/*
 * Constructor of unit layer
 */
UnitLayer::UnitLayer()
	: _batch(nullptr)
{}

/*
 * Destructor of unit layer
 */
UnitLayer::~UnitLayer()
{
	_batch = nullptr;
}

/*
 * Get stage instance by getParent
 */
Stage * UnitLayer::getStage()
{
	return dynamic_cast<Stage*>(getParent());
}

/*
 * Set unit data
 * And return this unit data
 */
Entity* UnitLayer::setUnit(Vec2 cor, EntityType type)
{
	auto entity = Entity::create(type, cor, getStage());
	_batch->addChild(entity);
	return entity;
}

/*
 * Set unit data
 */
void UnitLayer::setUnit(Vec2 cor, Entity * unit)
{
	auto stage = getStage();
	unit->setPosition(stage->getPositionAsTile(cor) + Vec2(stage->getChipSize().x / 2, 0));
	unit->setTag(cor.x * _map_size.y + cor.y);
	_batch->addChild(unit);
}

/*
 * Get unit data
 */
Entity * UnitLayer::getUnit(int x, int y)
{
	return util::instance<Entity>(getChildByTag(0)->getChildByTag(x * _map_size.y + y));
}

/*
 * Remove unit data
 */
Entity * UnitLayer::removeUnit(int x, int y)
{
	auto remove = util::instance<Entity>(getChildByTag(0)->getChildByTag(x * _map_size.y + y));
	getChildByTag(0)->removeChild(remove);
	return remove;
}

/********************************************************************************/

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
	stage->setTileBatch(SpriteBatchNode::create("TileSet/" + stage->getTileFile()));
	stage->setUnitBatch(SpriteBatchNode::create("image/unit.png"));

	//Set Layerdata
	for (int i = 0; i < 5; i++)
	{
		if (i == 3) continue;
		auto layer = StageLayer::create();
		layer->setTag(i);
		layer->setMapSize(stage->getMapSize());
		layer->setBatch(util::cloneBatch(stage->getTileBatch(), 0));
		layer->addChild(layer->getBatch());
		stage->addChild(layer);
	}

	// Set unit layer
	auto layer = UnitLayer::create();
	layer->setTag(3);
	layer->setMapSize(stage->getMapSize());
	layer->setBatch(util::cloneBatch(stage->getUnitBatch(), 0));
	layer->addChild(layer->getBatch());
	stage->addChild(layer);

	//Set chip data
	int count = 0;
	for (int l = 0; l < 3; l++)
	{
		auto layer = stage->getStageLayer(l);
		for (int x = 0; x < stage->getMapSize().x; x++)
		{
			for (int y = 0; y < stage->getMapSize().y; y++)
			{
				// If consume all element, break
				if (sLine.size() == count)
					break;

				auto id = std::atoi(sLine[count++].c_str());
				// id == 0 is no tile
				if (id == 0)
					continue;

				auto tile = layer->setTile(x, y, id);
				// Add city
				if (util::instanceof<City>(tile))
					stage->_cities[util::instance<City>(tile)->getOwner()].push_back(util::instance<City>(tile));
			}
		}
	}

	// Add selector
	auto wnum = stage->getHorizontalChipNumber();
	stage->selector = Sprite::createWithTexture(stage->getTileBatch()->getTexture(), Rect((STAGE_TILE_FRAME % wnum) * stage->getChipSize().x, (STAGE_TILE_FRAME / wnum) * stage->getChipSize().y, stage->getChipSize().x, stage->getChipSize().y));
	stage->selector->setOpacity(0);
	stage->selector->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	// Add animation
	auto animation = Animation::create();
	animation->addSpriteFrame(SpriteFrame::createWithTexture(stage->getTileBatch()->getTexture(), Rect((STAGE_TILE_FRAME % wnum) * stage->getChipSize().x, (STAGE_TILE_FRAME / wnum) * stage->getChipSize().y, stage->getChipSize().x, stage->getChipSize().y)));
	animation->addSpriteFrame(SpriteFrame::createWithTexture(stage->getTileBatch()->getTexture(), Rect(((STAGE_TILE_FRAME + 1) % wnum) * stage->getChipSize().x, ((STAGE_TILE_FRAME + 1) / wnum) * stage->getChipSize().y, stage->getChipSize().x, stage->getChipSize().y)));
	animation->setDelayPerUnit(0.1f);
	animation->setRestoreOriginalFrame(true);
	stage->selector->runAction(RepeatForever::create(Animate::create(animation)));
	stage->addChild(stage->selector, 100);

	stage->addUnitForDebug();

	return stage;
}

/*
 * Add unti for debug
 */
void Stage::addUnitForDebug()
{
	// for debug 
	Owner names[] = { Owner::player, Owner::enemy };
	std::vector<Vec2> poses;
	for (int i = 0; i < 100; i++)
	{
		auto pos = Vec2(std::rand() % (int)(getMapSize().x), std::rand() % (int)(getMapSize().y));
		auto type = getTile(0, pos.x, pos.y)->getTerrainType();
		if (type == TerrainType::ocean)
			continue;
		if (type == TerrainType::river)
			continue;
		if (!util::find(poses, pos))
		{
//			setUnit(pos.x, pos.y, static_cast<EntityType>(std::rand() % static_cast<int>(EntityType::COUNT)), names[i % 2]);
			if (i % 2)
//				getUnit(pos.x, pos.y)->setColor(Color3B::RED);
			poses.push_back(pos);
		}
	}
	for(auto pair : _cities)
		for(auto city : pair.second)
			for (int i = 0; i < city->getMaxDeployer() - 10; i++)
			{
				auto unit = setUnit(city->getPositionAsTile(), static_cast<EntityType>(std::rand() % static_cast<int>(EntityType::COUNT)), city->getOwner());
				if (!OwnerInformation::getInstance()->isSameGroup(Owner::player, unit->getAffiliation()))
					unit->setColor(Color3B::RED);
				deployUnit(unit, city);
			}
}

/*
 * Constructor of stage
 */
Stage::Stage()
	: _gap(0), _tile_batch(nullptr), _unit_batch(nullptr)
	, onTap(nullptr), onDoubleTap(nullptr), onLongTapBegan(nullptr), onLongTapEnd(nullptr), onSwipeCheck(nullptr), onFlickCheck(nullptr)
	, selector(nullptr)
{}

/*
 * Destructor of stage
 */
Stage::~Stage()
{
	_gap = 0;
	onTap = onDoubleTap = onLongTapBegan = onLongTapEnd = nullptr;
	onSwipeCheck = onFlickCheck = nullptr;
	selector = nullptr;
	CC_SAFE_RELEASE_NULL(_tile_batch);
	CC_SAFE_RELEASE_NULL(_unit_batch);
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
			auto cor = getPositionAsTile(v);
			auto tiles = util::collect<StageTile*>(getTiles(cor), [](StageTile* tile) {return tile->getId(); });
			onTap(cor, tiles);
		}
	};
	listener->onDoubleTap = [this](Vec2 v)
	{
		if (onDoubleTap)
		{
			auto cor = getPositionAsTile(v);
			auto tiles = util::collect<StageTile*>(getTiles(cor), [](StageTile* tile) {return tile->getId(); });
			onDoubleTap(cor, tiles);
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
		// Spawn move and adjust position
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
			auto cor = getPositionAsTile(v);
			auto tiles = util::collect<StageTile*>(getTiles(cor), [](StageTile* tile) {return tile->getId(); });
			onLongTapBegan(cor, tiles);
		}
	};
	//When onLongtapEnd, call onLongTapEnd
	listener->onLongTapEnd = [this](Vec2 v)
	{
		if (onLongTapEnd)
		{
			auto cor = getPositionAsTile(v);
			auto tiles = util::collect<StageTile*>(getTiles(cor), [](StageTile* tile) {return tile->getId(); });
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

/********************************************************************************/

/*
 * Adjust position from min to max
 */
cocos2d::Vec2 Stage::adjustArea(cocos2d::Vec2 v)
{
	auto maxWidth = getActualMapWidth() * getScale() - cocos2d::Director::getInstance()->getWinSize().width;
	auto maxHeight = getActualMapHeight() * getScale() - cocos2d::Director::getInstance()->getWinSize().height;
	return cocos2d::Vec2(
		(maxWidth < 0) ? 0 : std::max(-maxWidth, std::min(v.x, 0.0f)),
		(maxHeight < 0) ? 0 : std::max(-maxHeight, std::min(v.y, 0.0f)));
};

/*
 * Get position on tile
 */
Vec2 Stage::getPositionAsTile(Vec2 pos)
{
	auto fix = (pos - getPosition()) / getScale();
	fix.y = getActualMapHeight() - fix.y;
	auto even = (int)fix.x % (int)(_chip_size.x + _gap) <= (_chip_size.x + _gap) / 2;
	auto x = (int)(fix.x / (_chip_size.x + _gap));
	auto y = (int)((fix.y - (even ? 0 : _chip_size.y / 2)) / _chip_size.y) * 2 + (even ? 0 : 1);
	return Vec2(x, y);
}

/*
* Get coordinate by tile coordinate
*/
Vec2 Stage::getPositionByTile(int x, int y)
{
	return Vec2(x * (_chip_size.x + _gap) + (y % 2) * (_chip_size.x + _gap) / 2, (_map_size.y - 1 - y) * _chip_size.y / 2);
}


/*
 * Get all unit number
 */
int Stage::getUnitNumber()
{
	auto size = 0;
	for (auto pair : _units)
		size += pair.second.size();
	return size;
}

/*
 * Cut route by unit mobility
 */
void Stage::cutRouteByMobility(Entity * unit, std::vector<StageTile*> &route)
{
	auto size = 0;
	auto cost = 0;
	auto it = route.rbegin();
	for(auto it : route)
	{
		for (auto tile : getTiles(it->getPositionAsTile()))
			cost += EntityToTile::getInstance()->getSearchCost(tile->getTerrainType(), unit->getType());
		if (getUnit(it->getPositionAsTile()))
			break;
		if (cost > unit->getMobility())
			break;
		size++;
	}
	while (route.size() > size)
		route.pop_back();
}

/********************************************************************************/

/*
 * Get tiles by coordinate
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
 * Show selector
 * If minus coordinate, remove selector
 */
void Stage::selectTile(int x, int y)
{
	// Remove selector
	if (x < 0 || y < 0)
	{
		selector->setOpacity(0);
		return;
	}

	selector->setOpacity(255);
	selector->setPosition(getPositionByTile(x, y));
}

/*
 * Blink tile
 * Implement by adding shadow layer
 */
void Stage::blinkTile(StageTile* tile, Color3B color)
{
	Sprite* white;
	auto cor = tile->getPositionAsTile();
	auto shadow = getShadowLayer();
	auto shadow_tile = shadow->getTile(cor);
	// For unsearch area
	if (shadow_tile && shadow_tile->getId() == STAGE_TILE_DARK)
	{
		//Already blinking, resert
		shadow_tile->removeAllChildren();
		// Make new sprite
		auto tex = tile->getTexture();
		white = Sprite::createWithTexture(tex, Rect(tex->getContentSize().width - _chip_size.x, tex->getContentSize().height - _chip_size.y, _chip_size.x, _chip_size.y));
		shadow_tile->addChild(white);
	}
	else
	{
		//Already blinking, resert
		if (shadow_tile)
			shadow_tile->removeFromParent();

		white = shadow->setTile(cor, STAGE_TILE_WHITE);
	}

	white->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	white->setOpacity(150);
	white->setColor(color);
}

/*
 * Change blink color
 */
void Stage::blinkChange(StageTile* tile, cocos2d::Color3B color)
{
	auto shadow_tile = getShadowLayer()->getTile(tile->getPositionAsTile());
	if (shadow_tile)
		if (shadow_tile->getId() == STAGE_TILE_WHITE)
			shadow_tile->setColor(color);
		else if (shadow_tile->getChildrenCount() == 1)
			shadow_tile->getChildren().at(0)->setColor(color);
}

/*
* Stop to blink tile
*/
void Stage::blinkOffTile(StageTile* tile)
{
	auto shadow_tile = getShadowLayer()->getTile(tile->getPositionAsTile());
	if (shadow_tile)
		if (shadow_tile->getId() == STAGE_TILE_WHITE)
			shadow_tile->removeFromParent();
		else
			shadow_tile->removeAllChildren();
}

/********************************************************************************/

/*
 * Set unit by creating instance
 */
Entity* Stage::setUnit(int x, int y, EntityType type, const Owner owner)
{
	auto unit = getUnitLayer()->setUnit(x, y, type);
	unit->setAffiliation(owner);
	_units[owner].push_back(unit);
	return unit;
}

/*
 * Set unit position with setting tag
 * If only set tag, do not move position
 */
void Stage::moveUnitPositionAsTile(Vec2 cor, Entity * unit, bool isOnlySetTag)
{
	if(!isOnlySetTag)
		unit->setPosition(getPositionByTile(cor) + Vec2(getChipSize().x / 2, 0));
	unit->setTag(cor.x * getMapSize().y + cor.y);
}

/*
 * Remove unit
 */
void Stage::removeUnit(Entity * unit)
{
	auto units = _units[unit->getAffiliation()];
	units.erase(std::remove(units.begin(), units.end(), unit), units.end());
	unit->removeFromParent();
}

/*
 * Blink unit
 */
void Stage::blinkUnit(Entity * unit)
{
	auto inner = Sprite::createWithTexture(unit->getTexture(), Rect(32, static_cast<int>(unit->getType()) * 32, 32, 32));
	inner->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	inner->setOpacity(0);
	inner->setTag(0);
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
	if (unit->getChildByTag(0))
		unit->removeChildByTag(0);
}

/********************************************************************************/

/*
 * Move for centering specify coordinate
 */
void Stage::moveView(int x, int y)
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
 * Move for centering specify unit
 */
void Stage::moveView(Entity * unit)
{
	moveView(getPositionByTile(unit->getPositionAsTile()));
}

/*
 * Move for centering specify tile
 */
void Stage::moveView(StageTile * tile)
{
	moveView(getPositionByTile(tile->getPositionAsTile()));
}


/*
 * Initialize searched property
 */
void Stage::initStage(Owner owner)
{
	auto shadow = getShadowLayer();
	shadow->getChildByTag(0)->removeAllChildren();
	for (int x = 0; x < _map_size.x; x++)
	{
		for (int y = 0; y < _map_size.y; y++)
		{
			for (auto tile : getTiles(x, y))
				tile->setSearched(false);

			// All black out
			shadow->setTile(x, y, STAGE_TILE_DARK);

			// All enemy hided
			auto unit = getUnit(x, y);
			if (unit)
			{
				if (unit->getAffiliation() == owner)
					unit->setOpacity(255);
				else
					unit->setOpacity(0);
				if(unit->getTag() > DEPLOY_CONST)
					unit->setState(EntityState::supplied);
				else
					unit->setState(EntityState::none);

			}
		}
	}
	for(auto pair : _units)
		for (auto unit : pair.second)
		{
			if (unit->getAffiliation() == owner)
				unit->setOpacity(255);
			else
				unit->setOpacity(0);
			if (unit->getTag() > DEPLOY_CONST)
				unit->setState(EntityState::supplied);
			else
				unit->setState(EntityState::none);
		}


	for (auto city : _cities[owner])
	{
		discoverTile(city);
		for (auto tile : startRecursiveTileSearch(city->getPositionAsTile(), 1, EntityType::sight))
			discoverTile(tile);
	}

	for (auto unit : _units[owner])
	{
		discoverTile(getTile(0, unit->getPositionAsTile()));
		for (auto tile : startRecursiveTileSearch(unit->getPositionAsTile(), unit->getSearchingAbility(), EntityType::sight))
			discoverTile(tile);
	}
}

/*
 * Set searched and open insight, enemy
 */
void Stage::discoverTile(StageTile * tile)
{
	auto shadow = getShadowLayer();
	auto cor = tile->getPositionAsTile();
	tile->setSearched(true);
	if (shadow->getTile(cor))
		shadow->removeTile(cor);
	auto unit = getUnit(cor);
	if (unit && unit->getOpacity() == 0)
		unit->setOpacity(255);
}

/********************************************************************************/

/*
 * Recursive search Start
 * isBlockByUnit is true, do not move throuhgh tile on unit
 * isContainUnit is false, do not contains visible unit. And visible unit tile is not moved
 * Tile of lower layer is listing.
 */
std::vector<StageTile*> Stage::startRecursiveTileSearch(Vec2 point, int remainCost, EntityType type, bool isBlockByUnit, bool isContainTileOnUnit)
{
	std::vector<StageTile*> result;
	std::queue<std::function<StageTile*(EntityType, bool, bool)>> queue;
	auto ori = recursiveTileSearch(queue, Vec2(0, 0), point, remainCost, type, isBlockByUnit, isContainTileOnUnit);
	while (!queue.empty())
	{
		auto tile = queue.front()(type, isBlockByUnit, isContainTileOnUnit);
		if(tile)
			result.push_back(tile);
		queue.pop();
	}
	for (auto tile : result)
		tile->setActualCost(0);
	if(ori)
		ori->setActualCost(0);
	return result;
}

/*
 * Recursive search
 * Intrusion
 * go ↑: (0, 1)
 * go ↓: (0, -1)
 * go →↑: (-1, 1)
 * go →↓: (-1, -1)
 * go ←↑: (1, 1)
 * go ←↓: (1, -1)
 *
 * isBlockByUnit is true, do not move throuhgh tile on unit 
 * isContainUnit is false, do not contains visible unit. And visible unit tile is not moved
 */
StageTile* Stage::recursiveTileSearch(std::queue<std::function<StageTile*(EntityType, bool, bool)>> &queue, Vec2 intrusion, Vec2 point, int remainCost, EntityType type, bool isBlockByUnit, bool isContainTileOnUnit)
{
	// Out of range
	if (point.x < 0 || point.y < 0 || point.x > _map_size.x - 1 || point.y > _map_size.y - 1)
		return nullptr;

	auto cost = 0;
	auto tiles = getTiles(point);
	auto result = tiles.front();
	for (auto tile : tiles)
		cost += EntityToTile::getInstance()->getSearchCost(tile->getTerrainType(), type);

	// If consume all cost, process end
	if (intrusion != Vec2(0, 0) && (remainCost -= cost) < 0)
		return nullptr;

	// Already exist unit
	if (intrusion != Vec2(0, 0) && isBlockByUnit && type != EntityType::sight && type != EntityType::counter)
	{
		auto unit = getUnit(point);
		if (unit && unit->getOpacity() != 0)
			if (isContainTileOnUnit)
				return result;
			else
				return nullptr;
	}

	// Check cost 
	if (result->getActualCost() > remainCost)
		return nullptr;
	else if (result->getActualCost() == 0)
		result->setActualCost(remainCost);
	else
	{
		result->setActualCost(remainCost);
		result = nullptr;
	}

	//Up
	if (intrusion.y >= 0)
		queue.push(std::bind(&Stage::recursiveTileSearch, this, std::ref(queue), Vec2(0, 1), util::getHexUp(point), remainCost, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//Up right
	if (intrusion.x == -1 || (intrusion.x == 0 && intrusion.y != -1))
		queue.push(std::bind(&Stage::recursiveTileSearch, this, std::ref(queue), Vec2(-1, 1), util::getHexUpRight(point), remainCost, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//Up left
	if (intrusion.x == 1 || (intrusion.x == 0 && intrusion.y != -1))
		queue.push(std::bind(&Stage::recursiveTileSearch, this, std::ref(queue), Vec2(1, 1), util::getHexUpLeft(point), remainCost, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//Down
	if (intrusion.y <= 0)
		queue.push(std::bind(&Stage::recursiveTileSearch, this, std::ref(queue), Vec2(0, -1), util::getHexDown(point), remainCost, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//Down right
	if (intrusion.x == -1 || (intrusion.x == 0 && intrusion.y != 1))
		queue.push(std::bind(&Stage::recursiveTileSearch, this, std::ref(queue), Vec2(-1, -1), util::getHexDownRight(point), remainCost, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	//Down left
	if (intrusion.x == 1 || (intrusion.x == 0 && intrusion.y != 1))
		queue.push(std::bind(&Stage::recursiveTileSearch, this, std::ref(queue), Vec2(1, -1), util::getHexDownLeft(point), remainCost, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	return result;
}

/*
 * Start recursive liner search for all directon
 */
std::vector<StageTile*> Stage::startRecursiveTileSearchForLiner(cocos2d::Vec2 point, int remainCost)
{
	std::vector<StageTile*> tiles;
	//Up
	for (auto tile : recursiveTileSearchForLiner(Vec2(0, 1), util::getHexUp(point), remainCost))
		tiles.push_back(tile);
	//Up right
	for (auto tile : recursiveTileSearchForLiner(Vec2(-1, 1), util::getHexUpRight(point), remainCost))
		tiles.push_back(tile);
	//Up left
	for (auto tile : recursiveTileSearchForLiner(Vec2(1, 1), util::getHexUpLeft(point), remainCost))
		tiles.push_back(tile);
	//Down
	for (auto tile : recursiveTileSearchForLiner(Vec2(0, -1), util::getHexDown(point), remainCost))
		tiles.push_back(tile);
	//Down right
	for (auto tile : recursiveTileSearchForLiner(Vec2(-1, -1), util::getHexDownRight(point), remainCost))
		tiles.push_back(tile);
	//Down left
	for (auto tile : recursiveTileSearchForLiner(Vec2(1, -1), util::getHexDownLeft(point), remainCost))
		tiles.push_back(tile);

	return tiles;
}


/*
* Recursive search
*/
std::vector<StageTile*> Stage::recursiveTileSearchForLiner(Vec2 intrusion, Vec2 point, int remainCost)
{
	// Out of range
	if (point.x < 0 || point.y < 0 || point.x > _map_size.x - 1 || point.y > _map_size.y - 1)
		return std::vector<StageTile*>();

	// If consume all cost, process end
	if ((remainCost -= 1) < 0)
		return std::vector<StageTile*>();

	std::vector<StageTile*> result;
	result.push_back(getTiles(point).front());

	//Up
	if (intrusion == Vec2(0, 1))
		for (auto tile : recursiveTileSearchForLiner(intrusion, util::getHexUp(point), remainCost))
			result.push_back(tile);
	//Up right
	if (intrusion == Vec2(-1, 1))
		for (auto tile : recursiveTileSearchForLiner(intrusion, util::getHexUpRight(point), remainCost))
			result.push_back(tile);
	//Up left
	if (intrusion == Vec2(1, 1))
		for (auto tile : recursiveTileSearchForLiner(intrusion, util::getHexUpLeft(point), remainCost))
			result.push_back(tile);
	//Down
	if (intrusion == Vec2(0, -1))
		for (auto tile : recursiveTileSearchForLiner(intrusion, util::getHexDown(point), remainCost))
			result.push_back(tile);
	//Down right
	if (intrusion == Vec2(-1, -1))
		for (auto tile : recursiveTileSearchForLiner(intrusion, util::getHexDownRight(point), remainCost))
			result.push_back(tile);
	//Down left
	if (intrusion == Vec2(1, -1))
		for (auto tile : recursiveTileSearchForLiner(intrusion, util::getHexDownLeft(point), remainCost))
			result.push_back(tile);

	return result;
}


/*
 * Recursive search Start for weapon
 */
std::vector<StageTile*> Stage::startRecursiveTileSearchForWeapon(Entity* executer, Entity* enemy, WeaponData* weapon)
{
	std::vector<StageTile*> tiles;
	std::queue<std::function<StageTile*(EntityType, bool, bool)>> queue;
	auto point = executer->getPositionAsTile();
	auto enemy_point = enemy->getPositionAsTile();

	auto intrusion = util::getDirection(executer->getPosition() - enemy->getPosition(), point);

	switch (weapon->getRange().direction_type)
	{
	case DirectionType::liner:
		tiles = recursiveTileSearchForLiner(intrusion, point, weapon->getRange().firing_range);
		break;
	case DirectionType::crescent:
	{
		for (auto tile : recursiveTileSearchForLiner(util::getVecRight(intrusion), util::getPosRight(intrusion, point), weapon->getRange().firing_range))
			tiles.push_back(tile);
		for (auto tile : recursiveTileSearchForLiner(util::getVecLeft(intrusion), util::getPosLeft(intrusion, point), weapon->getRange().firing_range))
			tiles.push_back(tile);
		auto tile = recursiveTileSearch(queue, intrusion, point, weapon->getRange().firing_range, EntityType::counter);
		if (tile)
			tiles.push_back(tile);
		break;
	}
	case DirectionType::overHalf:
	{
		for (auto tile : recursiveTileSearchForLiner(util::getVecRight(util::getVecRight(intrusion)), util::getPosRight(util::getVecRight(intrusion), util::getPosRight(intrusion, point)), weapon->getRange().firing_range))
			tiles.push_back(tile);
		for (auto tile : recursiveTileSearchForLiner(util::getVecLeft(util::getVecLeft(intrusion)), util::getPosLeft(util::getVecLeft(intrusion), util::getPosLeft(intrusion, point)), weapon->getRange().firing_range))
			tiles.push_back(tile);
		auto tile = recursiveTileSearch(queue, intrusion, point, weapon->getRange().firing_range, EntityType::counter);
		if (tile)
			tiles.push_back(tile);
		tile = recursiveTileSearch(queue, util::getVecRight(intrusion), util::getPosRight(intrusion, point), weapon->getRange().firing_range, EntityType::counter);
		if (tile)
			tiles.push_back(tile);
		tile = recursiveTileSearch(queue, util::getVecLeft(intrusion), util::getPosLeft(intrusion, point), weapon->getRange().firing_range, EntityType::counter);
		if (tile)
			tiles.push_back(tile);
		break;
	}
	case DirectionType::full:
		recursiveTileSearch(queue, Vec2(0, 0), executer->getPositionAsTile(), weapon->getRange().firing_range, EntityType::counter);
		break;
	case DirectionType::select:
		tiles.push_back(getTiles(enemy_point.x, enemy_point.y).front());
		break;
	}
	while (!queue.empty())
	{
		auto tile = queue.front()(EntityType::counter, true, false);
		if (tile)
		{
			tiles.push_back(tile);
			if (weapon->getRange().secondary_effect > 0)
				recursiveTileSearch(queue, Vec2(0, 0), tile->getPositionAsTile(), weapon->getRange().secondary_effect, EntityType::counter);
		}
		queue.pop();
	}
	for (auto tile : tiles)
		tile->setActualCost(0);
	return tiles;
}

/*
 * Recursive search
 * A* algolism
 * return Start -> Goal route
 */
std::vector<StageTile*> Stage::startRecursiveTileSearchByAstar(const std::vector<StageTile*>& goals, Vec2 start, int limit, EntityType type, bool isGetOnTheWay, bool isContainTileOnUnit, bool isContainStart, float limitation)
{
	// Calculate center position
	auto center = Vec2::ZERO;
	for (auto vec : goals)
		center += vec->getPosition();
	center = center / goals.size();

	// Check limitation
	auto diff = (center - start).getLengthSq();
	// Set center tile coordinate
	if (diff > limitation)
		center = getPositionAsTile(start + (center - start) * sqrt(limitation / diff));
	else
		center = getPositionAsTile(center);



	// Get base data
	auto openList = std::priority_queue<TilePointer, std::vector<TilePointer>, std::greater<TilePointer>>();
	auto closeList = std::vector<StageTile*>();
	auto tile = getTile(0, start.x, start.y);
	tile->setActualCost(0);
	tile->setHeuristicCost((tile->getPosition() - center).getLengthSq());
	tile->setParentNode(nullptr);
	tile->setSearchState(SearchState::closed);
	openList.push(TilePointer(tile));

	// Search by A star
	while (!openList.empty())
	{
		auto pos = tile->getPositionAsTile();
		for (auto tpos : {	pos + Vec2(0, -2), pos + Vec2((int)(pos.y) % 2, -1), pos + Vec2(((int)(pos.y) % 2) - 1, -1), 
							pos + Vec2(0, 2),  pos + Vec2((int)(pos.y) % 2, 1),  pos + Vec2((int)(pos.y) % 2 - 1, 1) })
		{
			// Out of range
			if (tpos.x < 0 || tpos.y < 0 || tpos.x > _map_size.x - 1 || tpos.y > _map_size.y - 1)
				continue;
			auto targets = getTiles(tpos);
			if (!targets.empty())
			{
				auto target = targets.front();
				if (target->getSearchState() == SearchState::none)
				{

					auto cost = 0;
					for (auto t : targets)
						cost += EntityToTile::getInstance()->getSearchCost(t->getTerrainType(), type);
					if (!isContainTileOnUnit)
						if (getUnit(target->getPositionAsTile()))
							cost += 100;
					auto dv = center - tile->getPositionAsTile();
					int dx = (dv.x > 0 ? dv.x : -dv.x) * 2;
					int dy = (dv.y > 0 ? dv.y : -dv.y) / 2;
					target->setActualCost(cost + tile->getActualCost());
					target->setHeuristicCost((dx > dy) ? dx : dy);
					target->setParentNode(tile);
					target->setSearchState(SearchState::open);
					openList.push(TilePointer(target));
				}
			}
		}

		tile = openList.top()._pointer;
		tile->setSearchState(SearchState::closed);
		openList.pop();
		closeList.push_back(tile);

		if (util::find(goals, tile))
			break;
	}

	auto result = std::vector<StageTile*>();
	if (!isGetOnTheWay && tile->getActualCost() > limit)
	{
		// Initialize
		for (auto t : closeList)
		{
			t->setSearchState(SearchState::none);
			t->setActualCost(0);
		}
		while (!openList.empty())
		{
			openList.top()._pointer->setSearchState(SearchState::none);
			openList.top()._pointer->setActualCost(0);
			openList.pop();
		}

		return result;
	}

	while (tile)
	{
		result.push_back(tile);
		tile = tile->getParentNode();
	}

	if (!isContainStart)
		result.pop_back();

	std::reverse(result.begin(), result.end());

	while (!result.empty() && result.back()->getActualCost() > limit)
		result.pop_back();

	// Initialize
	for (auto t : closeList)
	{
		t->setSearchState(SearchState::none);
		t->setActualCost(0);
	}
	while (!openList.empty())
	{
		openList.top()._pointer->setSearchState(SearchState::none);
		openList.top()._pointer->setActualCost(0);
		openList.pop();
	}

	return result;
}

/********************************************************************************/

/*
 * Move next city
 */
Vec2 Stage::nextCity(Owner owner, StageTile* nowTile)
{
	auto cities = _cities[owner];
	// If not select tile, return front city
	if (!nowTile || !util::instanceof<City>(nowTile))
		nowTile = cities.back();
	bool discovered = false;
	for (auto city : cities)
	{
		if (discovered)
		{
			moveView(city);
			return city->getPositionAsTile();
		}
		if (city == nowTile)
			discovered = true;
	}
	moveView(cities.front());
	return cities.front()->getPositionAsTile();
}

/*
 * Move next unit
 * If all unit acted, return Vec2(-1, -1)
 */
Vec2 Stage::nextUnit(Owner owner, Entity* nowUnit)
{
	auto units = _units[owner];
	if (!nowUnit)
		nowUnit = units.front();
	bool discovered = false;
	std::vector<Entity*> preUnits;
	// Search unit and select later unit
	for (auto unit : units)
		if (discovered && unit->getState() != EntityState::acted)
		{
			moveView(unit);
			return unit->getPositionAsTile();
		}
		else if (unit == nowUnit)
			discovered = true;
		else
			preUnits.push_back(unit);
	// Select former unit
	for (auto unit : preUnits)
		if (discovered && unit->getState() != EntityState::acted)
		{
			moveView(unit);
			return unit->getPositionAsTile();
		}

	// All acted, return Vec2(-1, -1)
	return Vec2(-1, -1);
}

/*
 * Move checking
 * Return movable area
 */
std::vector<StageTile*> Stage::moveCheck(Entity * entity)
{
	return startRecursiveTileSearch(entity->getPositionAsTile(), entity->getMobility(), entity->getType());
}

/*
 * Move unit to tile provisionally
 * Return route
 */
std::vector<StageTile*> Stage::provisionalMoveUnit(Entity * entity, StageTile * tile)
{
	auto route = startRecursiveTileSearchByAstar(tile, entity->getPositionAsTile(), entity->getMobility(), entity->getType());
	Vector<FiniteTimeAction*> acts;
	auto i = 0;
	for (auto t : route)
		if (i++ > 0)
			acts.pushBack(Sequence::create(
				MoveTo::create(0.5f, getPositionByTile(t->getPositionAsTile()) + Vec2(getChipSize().x / 2, 0)),
				NULL));
	entity->runAction(Sequence::create(acts));

	// Keep pre_position
	_pre_provisional_cor = entity->getPositionAsTile();
	// Move tag
	moveUnitPositionAsTile(route.back()->getPositionAsTile(), entity, true);
	return route;
}

/*
 * Cancel to move unit
 */
void Stage::provisionalMoveCancel(Entity * entity)
{
	entity->stopAllActions();
	moveUnitPositionAsTile(_pre_provisional_cor, entity);
}

/*
 * Move unit to tile 
 * With searching enemy
 * And set state moved
 */
void Stage::moveUnit(Entity * entity, std::vector<StageTile*> route)
{
	for (auto tile : route)
		for (auto t : startRecursiveTileSearch(tile->getPositionAsTile(), entity->getSearchingAbility(), EntityType::sight))
			discoverTile(t);
	entity->setState(EntityState::moved);
}


/*
 * Deploy unit
 */
void Stage::deployUnit(Entity * unit, City * city)
{
	unit->setTag(DEPLOY_CONST + unit->getTag());
	unit->setVisible(false);
	city->addDeoloyer(unit);
}

/*
 * Dispatch unit
 */
void Stage::dispatchUnit(Entity * unit, City * city)
{
	moveUnitPositionAsTile(city->getPositionAsTile(), unit);
	unit->setVisible(true);
	unit->setOpacity(255);
	city->removeDeployer(unit);
}

void Stage::dispatchUnit(Entity * unit)
{
	auto city = util::findElement<StageTile*>(getTiles(unit->getPositionAsTile()), [](StageTile* tile) {return util::instanceof<City>(tile); });

	if (city)
		dispatchUnit(unit, util::instance<City>(city));
}



/*
 * Render for AI Scene
 */
Node * Stage::renderForAIScene()
{
	Node* node = Node::create();
	return node;
}
