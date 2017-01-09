#ifndef __StageTile_H__
#define __StageTile_H__

#include "cocos2d.h"

#include "ai/Owner.h"

class Stage;
class StageLayer;
class Entity;
enum class Command;

constexpr int STAGE_TILE_WHITE = 39;
constexpr int STAGE_TILE_DARK = 38;
constexpr int STAGE_TILE_FRAME = 36;

/*********************************************************/

/*
 * Terrain type
 */
enum class TerrainType
{
	none, 
	prairie, woods, mountain, river, ocean,
	road, bridge, capital, city, territory,

	COUNT
};
/*********************************************************/

class TileInformation
{
private:
	std::map<TerrainType, std::string> _name;
	std::map<TerrainType, std::map<Command, bool>> _commands;
protected:
	TileInformation();
public:
	static TileInformation* getInstance()
	{
		static TileInformation info;
		return &info;
	};
	std::string getName(TerrainType type) { return _name[type]; };
	bool getCommand(TerrainType type, Command com) { return _commands[type][com]; };
};

/*********************************************************/

/*
 * Tile class
 * Inheritance is substance
 */
class StageTile : public cocos2d::Sprite
{
protected:
	StageTile()
		: _id(0), _terrain(TerrainType::none), _searched(false), _remainCost(-1)
	{};
	~StageTile()
	{
		_id = 0;
	}
public:
	CC_SYNTHESIZE(int, _id, Id);
	CC_SYNTHESIZE(TerrainType, _terrain, TerrainType);
	CC_SYNTHESIZE(bool, _searched, Searched);
	CC_SYNTHESIZE(int, _remainCost, RemainCost);
	cocos2d::Vec2 getTileCoordinate();
	StageLayer* getStageLayer();
	Stage* getStage();
	static StageTile* create(const int id, const int x, const int y, cocos2d::SpriteBatchNode* batch, Stage* stage);
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

class City : public StageTile
{
protected:
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
	CC_SYNTHESIZE(int, _maxDurability, MaxDurability);
	CC_SYNTHESIZE(int, _Durability, Durability);
	CC_SYNTHESIZE(int, _maxDeployer, MaxDeployer);
	cocos2d::Vector<Entity*> _deployers;
	void addDeoloyer(Entity* entity);
	void removeDeployer(Entity* entity);
	inline Entity* getDeploter(int no) { return _deployers.at(no); };
	inline const cocos2d::Vector<Entity*>& getDeployersByRef() { return _deployers; };
	void supply(Entity* entity);
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
