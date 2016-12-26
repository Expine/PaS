#include "Tile.h"
#include "Stage.h"

USING_NS_CC;

StageTile * StageTile::create(const int id, const int x, const int y, SpriteBatchNode* batch, Stage* stage)
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

	//Set batch
	auto wnum = (int)(batch->getTextureAtlas()->getTexture()->getContentSize().width / stage->getChipSize().x);
	auto gap = stage->getGap();
	auto chipSize = stage->getChipSize();

	auto fix_y = (int)(stage->getMapSize().y - 1 - y);
	tile->initWithTexture(batch->getTexture(), Rect((id % wnum) * chipSize.x, (int)(id / wnum) * chipSize.y, chipSize.x, chipSize.y));
	tile->setPosition(x * (chipSize.x + gap) + (y % 2) * (chipSize.x + gap) / 2, fix_y * chipSize.y / 2);
	tile->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	tile->setTag(x * stage->getMapSize().y + y);

	return tile;
}

bool StageTile::init()
{
	return true;
}

