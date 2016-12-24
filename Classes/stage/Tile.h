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
	territory
};
/*********************************************************/
class StageTile : public cocos2d::Ref
{
public:
	CC_SYNTHESIZE(int, _id, Id);
	CC_SYNTHESIZE(TerrainType, _terrain, TerrainType);
	CC_SYNTHESIZE(int, _mobility, Mobility);
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
		return true;
	};
	CREATE_FUNC(None);
};

/*********************************************************/
class Prairie : public StageTile
{

};

class Woods : public Prairie
{

};

/*********************************************************/
class Mountain : public StageTile
{

};

/*********************************************************/
class Ocean : public StageTile
{

};

class River : public Ocean
{

};

/*********************************************************/
class Road : public StageTile
{

};

class Bridge : public StageTile
{

};

class City : public StageTile
{

};

class Capital : public City
{

};

/*********************************************************/
class Territory : public StageTile
{

};

#endif // __StageTile_H__
