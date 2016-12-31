#include "Tile.h"
#include "Stage.h"

#include "stage/Command.h"
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
				_commands[static_cast<TerrainType>(i - 1)][static_cast<CityCommand>(j - 2)] = item == "TRUE";
			}
		}
	}
}

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
	case 16:tile = Capital::create();	util::instance<Capital>(tile)->setOwner(Owner::player); break;
	case 17:tile = City::create();		util::instance<City>(tile)->setOwner(Owner::player);	break;
	case 24:tile = Capital::create();	util::instance<Capital>(tile)->setOwner(Owner::enemy);	break;
	case 25:tile = City::create();		util::instance<City>(tile)->setOwner(Owner::enemy);		break;
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

