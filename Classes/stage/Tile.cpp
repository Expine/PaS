#include "Tile.h"
#include "Stage.h"

USING_NS_CC;

/*
 * Create tile data
 */
StageTile * StageTile::create(const int id, const int x, const int y, SpriteBatchNode* batch, Stage* stage)
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
	case 16:tile = Capital::create();	break;
	case 17:tile = City::create();		break;
	default:tile = None::create();		break;
	}

	//Set ID
	tile->setId(id);

	//Set batch
	auto wnum = (int)(batch->getTextureAtlas()->getTexture()->getContentSize().width / stage->getChipSize().x);
	auto gap = stage->getGap();
	auto chipSize = stage->getChipSize();

	auto fix_y = (int)(stage->getMapSize().y - 1 - y);
	tile->initWithTexture(batch->getTexture(), Rect((id % wnum) * chipSize.x, (int)(id / wnum) * chipSize.y, chipSize.x, chipSize.y));
	tile->setPosition(stage->getCoordinateByTile(x, y));
	tile->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	tile->setTag(x * stage->getMapSize().y + y);

	return tile;
}
