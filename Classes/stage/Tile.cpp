#include "Tile.h"

USING_NS_CC;

StageTile * StageTile::getTerrainTypeByID(const int id)
{
	StageTile* tile;
	switch (id)
	{
	case 0: tile = None::create();		tile->setTerrainType(TerrainType::none);		break;
	case 1: tile = Ocean::create();		tile->setTerrainType(TerrainType::ocean);		break;
	case 2: tile = River::create();		tile->setTerrainType(TerrainType::river);		break;
	case 4: tile = Territory::create();	tile->setTerrainType(TerrainType::territory);	break;
	case 5: tile = Prairie::create();	tile->setTerrainType(TerrainType::prairie);		break;
	case 6: tile = Prairie::create();	tile->setTerrainType(TerrainType::prairie);		break;
	case 8: tile = Road::create();		tile->setTerrainType(TerrainType::road);		break;
	case 11:tile = Woods::create();		tile->setTerrainType(TerrainType::woods);		break;
	case 12:tile = Mountain::create();	tile->setTerrainType(TerrainType::mountain);	break;
	case 13:tile = Bridge::create();	tile->setTerrainType(TerrainType::bridge);		break;
	case 14:tile = Bridge::create();	tile->setTerrainType(TerrainType::bridge);		break;
	case 16:tile = Capital::create();	tile->setTerrainType(TerrainType::capital);		break;
	case 17:tile = City::create();		tile->setTerrainType(TerrainType::city);		break;
	default:tile = None::create();		tile->setTerrainType(TerrainType::none);		break;
	}

	//Set ID
	tile->setId(id);
	return tile;
}

bool StageTile::init()
{
	return true;
}

