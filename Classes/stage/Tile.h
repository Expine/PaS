#ifndef __StageTile_H__
#define __StageTile_H__

#include "cocos2d.h"

/*********************************************************/
enum class TerrainType
{
	none, 
	prairie,
	woods,
	mountain,
	river,
	ocean,
	road,
	bridge,
	capital,
	city,
	territory,

	COUNT
};
/*********************************************************/
class StageTile : public cocos2d::Ref
{
protected:
	StageTile()
		: _id(0)
		, _mobility(0)
		, _terrain(TerrainType::none)
		, _explanation("")
	{

	};
public:
	CC_SYNTHESIZE(int, _id, Id);
	CC_SYNTHESIZE(TerrainType, _terrain, TerrainType);
	CC_SYNTHESIZE(int, _mobility, Mobility);
	CC_SYNTHESIZE(std::string, _explanation, Explanation);
	virtual bool init();
	CREATE_FUNC(StageTile);
	static StageTile* getTerrainTypeByID(const int id);
};

/*********************************************************/
class None : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(0);
		setExplanation("NO DATA");
		return true;
	};
	CREATE_FUNC(None);
};

/*********************************************************/
class Prairie : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"開けた草原。\n移動しやすい。");
		return true;
	};
	CREATE_FUNC(Prairie);
};

class Woods : public Prairie
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"茂った森林。\n見通しが悪い。\n火の魔法が有効。");
		return true;
	};
	CREATE_FUNC(Woods);
};

/*********************************************************/
class Mountain : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"険しい山。\n移動しづらい。\n土の魔法が有効。");
		return true;
	};
	CREATE_FUNC(Mountain);
};

/*********************************************************/
class Ocean : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"深い海。\n一部移動可能。\n水の魔法が有効。");
		return true;
	};
	CREATE_FUNC(Ocean);
};

class River : public Ocean
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"歩ける浅瀬。\n移動しにくい。\n水の魔法が有効。");
		return true;
	};
	CREATE_FUNC(River);
};

/*********************************************************/
class Road : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"整備された道。\n移動しやすい。\n土の魔法が有効。");
		return true;
	};
	CREATE_FUNC(Road);
};

class Bridge : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"建設された橋。\n移動しやすい。");
		return true;
	};
	CREATE_FUNC(Bridge);
};

class City : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"拠点となる都市。\n兵士を駐在できる。\n");
		return true;
	};
	CREATE_FUNC(City);
};

class Capital : public City
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"重要な首都。\n兵士を配備できる。\n");
		return true;
	};
	CREATE_FUNC(Capital);
};

/*********************************************************/
class Territory : public StageTile
{
public:
	virtual bool init()
	{
		if (!StageTile::init())
			return false;
		setMobility(3);
		setExplanation(u8"自然の龍脈。\n魔法使いが補給\nできる。\n");
		return true;
	};
	CREATE_FUNC(Territory);
};

#endif // __StageTile_H__
