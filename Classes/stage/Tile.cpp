#include "Tile.h"
#include "Stage.h"

#include "stage/Command.h"
#include "entity/Entity.h"
#include "ai/Owner.h"
#include "util/Util.h"

USING_NS_CC;

/*
 * Initialize tile information
 */
TileInformation::TileInformation()
{
	auto lines = util::splitFile("info/tile.csv");
	auto i = -1;
	for (auto line : lines)
	{
		if (++i == 0)
			continue;

		auto items = util::splitString(line, ',');
		auto j = -1;
		for (auto item : items)
		{
			if (++j == 0)
				continue;
			switch (j)
			{
			case 1:	_name[static_cast<TerrainType>(i - 1)] = item;					break;
			case 2: case 3: case 4:
				_commands_enable[static_cast<TerrainType>(i - 1)][static_cast<Command>(j - 2 + static_cast<int>(Command::CITY_START) + 1)] = item == "TRUE";
			}
		}
	}
}

/*
 * Constructor of tile
 */
StageTile::StageTile()
	: _id(0), _terrain(TerrainType::none), _searched(false)
	, _search_state(SearchState::none), _actual_cost(0), _heuristic_cost(0), _parent_node(nullptr)
{}

/*
 * Destructor
 */
StageTile::~StageTile()
{
	_searched = false;
	_id = _actual_cost = _heuristic_cost = 0;
	_parent_node = nullptr;
}

/*
 * Create tile instance by id and set base information
 */
StageTile * StageTile::create(const int id)
{
	StageTile* tile;
	switch (id)
	{
	case 0: tile = None::create();		break;
	case 1: tile = Ocean::create();		break;
	case 2: tile = River::create();		break;
	case 4: tile = Territory::create();	break;
	case 5: tile = Prairie::create();	break;
	case 6: tile = Prairie::create();	break;
	case 8: tile = Road::create();		break;
	case 11:tile = Woods::create();		break;
	case 12:tile = Mountain::create();	break;
	case 13:tile = Bridge::create();	break;
	case 14:tile = Bridge::create();	break;
	case 16:tile = Capital::create();
		util::instance<Capital>(tile)->setOwner(Owner::player);
		util::instance<Capital>(tile)->setMaxDurability(200);
		util::instance<Capital>(tile)->setDurability(100);
		util::instance<Capital>(tile)->setMaxDeployer(30);
		break;
	case 17:tile = City::create();
		util::instance<City>(tile)->setOwner(Owner::player);
		util::instance<City>(tile)->setMaxDurability(200);
		util::instance<City>(tile)->setDurability(100);
		util::instance<City>(tile)->setMaxDeployer(20);
		break;
	case 24:tile = Capital::create();
		util::instance<Capital>(tile)->setOwner(Owner::enemy);
		util::instance<Capital>(tile)->setMaxDurability(200);
		util::instance<Capital>(tile)->setDurability(100);
		util::instance<Capital>(tile)->setMaxDeployer(30);
		break;
	case 25:tile = City::create();
		util::instance<City>(tile)->setOwner(Owner::enemy);
		util::instance<City>(tile)->setMaxDurability(200);
		util::instance<City>(tile)->setDurability(100);
		util::instance<City>(tile)->setMaxDeployer(20);
		break;
	default:tile = None::create();		break;
	}

	//Set ID
	tile->setId(id);

	return tile;
}

/*
 * Create tile data
 * And set batch, set position
 */
StageTile * StageTile::create(const int id, Vec2 cor, Stage* stage)
{
	// Create tile instance
	auto tile = create(id);
	auto batch = stage->getTileBatch();

	//Set batch
	auto wnum = stage->getHorizontalChipNumber();
	auto gap = stage->getGap();
	auto chipSize = stage->getChipSize();

	tile->initWithTexture(batch->getTexture(), Rect((id % wnum) * chipSize.x, (int)(id / wnum) * chipSize.y, chipSize.x, chipSize.y));
	tile->setPosition(stage->getPositionByTile(cor));
	tile->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	tile->setTag(cor.x * stage->getMapSize().y + cor.y);

	return tile;
}

/*
 * Get tile coordinate
 */
cocos2d::Vec2 StageTile::getPositionAsTile()
{
	int mapy = getStage()->getMapSize().y;
	return cocos2d::Vec2(getTag() / mapy, getTag() % mapy);
}

/*
 * Get Stage layer
 */
StageLayer * StageTile::getStageLayer()
{
	return dynamic_cast<StageLayer*>(getParent()->getParent());
}

/*
 * Get Stage
 */
Stage * StageTile::getStage()
{
	return dynamic_cast<Stage*>(getStageLayer()->getParent());
}

/*********************************************************/
/*
 * Constructor for city
 */
City::City()
	: _owner(Owner::player), _max_durability(0), _durability(0),_max_deployer(0)
{}

/*
 * Destructor of city
 */
City::~City()
{
	_durability = _max_durability = _max_deployer = 0;
}

/*
 * Add deployer to city
 */
void City::addDeoloyer(Entity * entity)
{
	_deployers.pushBack(entity);
}

/*
 * Remove deployer from city
 */
void City::removeDeployer(Entity * entity)
{
	_deployers.eraseObject(entity);
}

/*
 * Check whether this city can supply for unit
 */
bool City::isSuppliable()
{
	auto unit = getStage()->getUnit(getPositionAsTile());
	return unit && unit->getAffiliation() == _owner && unit->getState() == EntityState::none;
}

/*
 * Check whether this city can deploy unit
 */
bool City::isDeployable()
{
	auto unit = getStage()->getUnit(getPositionAsTile());
	return unit && unit->getAffiliation() == _owner && _deployers.size() < _max_deployer;
}

/*
 * Check whether this city can dispatch unit
 */
bool City::isDispatchable()
{
	auto unit = getStage()->getUnit(getPositionAsTile());
	return !unit && _deployers.size() > 0;
}

/*
 * Supply to unit
 */
void City::supply(Entity * entity)
{
	entity->setMaterial(entity->getMaxMaterial());
}

/*
 * Get occupied damage. 
 * Occupied when it becomes zero.
 */
void City::damaged(Soldier * unit)
{
	// Calculate damage
	util::initRand();
	auto damage = unit->getOccupationAbility() * util::getRand(0.8f, 1.2f);
	auto damaged_durability = _durability - damage;
	if (damaged_durability > 0)
		setDurability(damaged_durability);
	else
	{
		// Change city owner
		setDurability(getMaxDurability() / 2);
		setOwner(unit->getAffiliation());
	}
}

