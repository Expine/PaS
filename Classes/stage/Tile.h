#ifndef __StageTile_H__
#define __StageTile_H__

#include "cocos2d.h"

#include "ai/Owner.h"

class Stage;
class StageLayer;
class Entity;
class Soldier;
enum class Command;

constexpr int STAGE_TILE_WHITE = 39;
constexpr int STAGE_TILE_DARK = 38;
constexpr int STAGE_TILE_FRAME = 36;

constexpr int CITY_DEPLOY_MAX = 50;

/*********************************************************/

/** Terrain type */
enum class TerrainType
{
	none,
	prairie, woods, mountain, river, ocean,
	road, bridge, capital, city, territory,

	COUNT
};

/** Search state for A* algolism */
enum class SearchState
{
	none, open, closed
};

/*********************************************************/

/*
 * Tile base data
 */
class TileInformation
{
private:
	std::map<TerrainType, std::string> _name;
	std::map<TerrainType, std::map<Command, bool>> _commands_enable;
protected:
	TileInformation();
public:
	/** Get instance for singleton*/
	static TileInformation* getInstance()
	{
		static TileInformation info;
		return &info;
	};
	/** Get name */
	std::string getName(TerrainType type) { return _name[type]; };
	/** Check whether this command can be used*/
	bool getCommandEnable(TerrainType type, Command com) { return _commands_enable[type].count(com) == 0 || _commands_enable[type][com]; };
};

/*********************************************************/

/*
 * Tile class
 * Inheritance is substance
 */
class StageTile : public cocos2d::Sprite
{
protected:
	StageTile();
	virtual ~StageTile();
public:
	CC_SYNTHESIZE(int, _id, Id);
	CC_SYNTHESIZE(TerrainType, _terrain, TerrainType);
	CC_SYNTHESIZE(bool, _searched, Searched);
	CC_SYNTHESIZE(SearchState, _search_state, SearchState);
	CC_SYNTHESIZE(int, _actual_cost, ActualCost);
	CC_SYNTHESIZE(int, _heuristic_cost, HeuristicCost);
	CC_SYNTHESIZE(StageTile*, _parent_node, ParentNode);
	static StageTile* create(const int id);
	static StageTile* create(const int id, cocos2d::Vec2 cor, Stage* stage);

	cocos2d::Vec2 getPositionAsTile();
	StageLayer* getStageLayer();
	Stage* getStage();
};

/** Tile pointer for priority queue*/
class TilePointer
{
public:
	TilePointer(StageTile* tile) { _pointer = tile; };
	StageTile* _pointer;
	bool operator < (const TilePointer& comp) const
	{
		auto a = _pointer->getActualCost() + _pointer->getHeuristicCost();
		auto b = comp._pointer->getActualCost() + comp._pointer->getHeuristicCost();
		return (a == b) ? _pointer->getActualCost() < comp._pointer->getActualCost() : a < b;
	};
	inline bool operator > (const TilePointer& comp) const
	{
		auto a = _pointer->getActualCost() + _pointer->getHeuristicCost();
		auto b = comp._pointer->getActualCost() + comp._pointer->getHeuristicCost();
		return (a == b) ? _pointer->getActualCost() > comp._pointer->getActualCost() : a > b;
	};
};

/*********************************************************/
class None : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setTerrainType(TerrainType::none);
		return true;
	};
public:
	CREATE_FUNC(None);
};

/*********************************************************/
class Prairie : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setTerrainType(TerrainType::prairie);
		return true;
	};
public:
	CREATE_FUNC(Prairie);
};

class Woods : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setTerrainType(TerrainType::woods);
		return true;
	};
public:
	CREATE_FUNC(Woods);
};

/*********************************************************/
class Mountain : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::mountain);
		return true;
	};
public:
	CREATE_FUNC(Mountain);
};

/*********************************************************/
class Ocean : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::ocean);
		return true;
	};
public:
	CREATE_FUNC(Ocean);
};

class River : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::river);
		return true;
	};
public:
	CREATE_FUNC(River);
};

/*********************************************************/
class Road : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::road);
		return true;
	};
public:
	CREATE_FUNC(Road);
};

class Bridge : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::bridge);
		return true;
	};
public:
	CREATE_FUNC(Bridge);
};

/*********************************************************/
class City : public StageTile
{
protected:
	City();
	virtual ~City();
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::city);
		return true;
	};
public:
	CREATE_FUNC(City);
	CC_SYNTHESIZE(Owner, _owner, Owner);
	CC_SYNTHESIZE(int, _durability, Durability);
	CC_SYNTHESIZE(int, _max_durability, MaxDurability);
	CC_SYNTHESIZE(int, _max_deployer, MaxDeployer);
	cocos2d::Vector<Entity*> _deployers;

	void addDeoloyer(Entity* entity);
	void removeDeployer(Entity* entity);
	/** Get deployer by number */
	inline Entity* getDeploter(int no) { return _deployers.at(no); };
	/** Get vector of deployers by reference */
	inline cocos2d::Vector<Entity*>& getDeployersByRef() { return _deployers; };

	bool isSuppliable();
	bool isDeployable();
	bool isDispatchable();

	void supply(Entity* entity);
	void damaged(Soldier* unit);
};

class Capital : public City
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::capital);
		return true;
	};
public:
	CREATE_FUNC(Capital);
};

/*********************************************************/
class Territory : public StageTile
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::territory);
		return true;
	};
public:
	CREATE_FUNC(Territory);
};

#endif // __StageTile_H__
