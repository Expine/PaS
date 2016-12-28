#ifndef __StageTile_H__
#define __StageTile_H__

#include "cocos2d.h"

#include "ai/Owner.h"

class Stage;

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

/*
 * Tile class
 * Inheritance is substance
 */
class StageTile : public cocos2d::Sprite
{
protected:
	StageTile()
		: _id(0), _terrain(TerrainType::none), _explanation(""), _searched(false)
	{};
	~StageTile()
	{
		_id = 0;
	}
public:
	CC_SYNTHESIZE(int, _id, Id);
	CC_SYNTHESIZE(TerrainType, _terrain, TerrainType);
	CC_SYNTHESIZE(std::string, _explanation, Explanation);
	CC_SYNTHESIZE(bool, _searched, Searched);
	inline cocos2d::Vec2 getTileCoordinate(int mapy) { return cocos2d::Vec2(getTag() / mapy, getTag() % mapy); };
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
		setExplanation("NO DATA");
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
		setExplanation(u8"開けた草原。\n移動しやすい。");
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
		setExplanation(u8"茂った森林。\n見通しが悪い。\n火の魔法が有効。");
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
		setExplanation(u8"険しい山。\n移動しづらい。\n土の魔法が有効。");
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
		setExplanation(u8"深い海。\n一部移動可能。\n水の魔法が有効。");
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
		setExplanation(u8"歩ける浅瀬。\n移動しにくい。\n水の魔法が有効。");
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
		setExplanation(u8"整備された道。\n移動しやすい。\n土の魔法が有効。");
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
		setExplanation(u8"建設された橋。\n移動しやすい。");
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
		setExplanation(u8"拠点となる都市。\n兵士を駐在できる。\n");
		return true;
	};
public:
	CREATE_FUNC(City);
	CC_SYNTHESIZE(Owner, _owner, Owner);
};

class Capital : public City
{
protected:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;

		setTerrainType(TerrainType::capital);
		setExplanation(u8"重要な首都。\n兵士を配備できる。\n");
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
		setExplanation(u8"自然の龍脈。\n魔法使いが補給\nできる。\n");
		return true;
	};
public:
	CREATE_FUNC(Territory);
};

#endif // __StageTile_H__
