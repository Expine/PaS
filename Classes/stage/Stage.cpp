#include "Stage.h"

#include "util/Util.h"
#include "util/MultiListener.h"

USING_NS_CC;

Stage * Stage::parseStage(const std::string file)
{
	auto stage = Stage::create();
	auto lines = util::splitFile(file);
	auto fLine = util::splitString(lines[0], ',');
	auto sLine = util::splitString(lines[1], ',');
	
	//Set stage data
	stage->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	stage->setMapSize(Vec2(std::atoi(fLine[0].c_str()), std::atoi(fLine[1].c_str())));
	stage->setChipSize(Vec2(std::atoi(fLine[2].c_str()), std::atoi(fLine[3].c_str())));
	stage->setGap(std::atoi(fLine[4].c_str()));
	stage->setBackGround(fLine[5]);
	stage->setBGM(fLine[6]);
	stage->setTileFile(fLine[7]);

	//Set Layerdata
	for (int i = 0; i < 3; i++)
	{
		auto layer = StageLayer::create();
		layer->setTag(i);
		layer->setMapSize(stage->getMapSize());
		stage->addChild(layer);
	}

	//Set chip data
	int l = -1;
	int count = 0;
	for (std::string id : sLine)
	{
		if (count % (int)(stage->getMapSize().x * stage->getMapSize().y) == 0)
			l++;
		stage->addTile(l, StageTile::getTerrainTypeByID(std::atoi(id.c_str())));
		count++;
	}

	stage->render();

	//Set listener
	auto listener = MultiTouchListener::create();
	listener->onTap = [](Vec2 v)
	{
		CCLOG("Tapped (%f, %f)", v.x, v.y);
	};
	listener->onSwipe = [stage](Vec2 v, Vec2 diff, float time)
	{
		CCLOG("Swipe (%f, %f) diff(%f, %f) time:%f", v.x, v.y, diff.x, diff.y, time);
		stage->setPosition(stage->adjustArea(stage->getPosition() + diff));
		CCLOG("RESULT (%f, %f)", stage->getPosition().x, stage->getPosition().y);
	};
	listener->onLongTapBegan = [](Vec2 v)
	{
		CCLOG("LongTapBegan (%f, %f)", v.x, v.y);
	};
	listener->onLongTapEnd = [](Vec2 v)
	{
		CCLOG("LongTapEnd (%f, %f)", v.x, v.y);
	};
	listener->pinchIn = [stage] (Vec2 v, float ratio)
	{
		CCLOG("PinchIn (%f, %f) -> %f", v.x, v.y, ratio);
		ratio = ratio / (1 + ratio) / STAGE_RATIO_RATIO;
		auto diff_ratio = stage->getScale();
		stage->setScale(stage->adjustRatio(stage->getScale() * ratio));
		diff_ratio = stage->getScale() / diff_ratio;
		auto diff_x = v.x * (1 - diff_ratio);
		auto diff_y = v.y * (1 - diff_ratio);
		auto pos = (stage->getPosition() + Vec2(diff_x, diff_y));
		stage->setPosition(stage->adjustArea(stage->getPosition()  * diff_ratio + Vec2(diff_x, diff_y)));
	};
	listener->pinchOut = [stage](Vec2 v, float ratio)
	{
		CCLOG("PinchOut (%f, %f) -> %f", v.x, v.y, ratio);
		ratio = ratio / (1 + ratio) / STAGE_RATIO_RATIO;
		auto diff_ratio = stage->getScale();
		stage->setScale(stage->adjustRatio(stage->getScale() * ratio));
		diff_ratio = stage->getScale() / diff_ratio;
		auto diff_x = v.x * (1 - diff_ratio);
		auto diff_y = v.y * (1 - diff_ratio);
		auto pos = (stage->getPosition() + Vec2(diff_x, diff_y));
		stage->setPosition(stage->adjustArea(stage->getPosition()  * diff_ratio + Vec2(diff_x, diff_y)));
	};
	stage->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, stage);
	auto mouseListener = MultiMouseListener::create(listener);
	stage->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, stage);

	return stage;
}

bool Stage::init()
{
	if (!Node::init())
		return false;

	return true;
}

void Stage::render()
{
	for (int l = 0; l < 3; l++)
	{
		dynamic_cast<StageLayer*>(getChildByTag(l))->render();
	}
}

/*********************************************************/

void StageLayer::render()
{
	auto parent = dynamic_cast<Stage*>(getParent());
	auto batch = SpriteBatchNode::create("TileSet/" + parent->getTileFile());
//	batch->getTexture()->setAliasTexParameters();
	auto wnum = (int)(batch->getTextureAtlas()->getTexture()->getContentSize().width / parent->getChipSize().x);
	auto gap = parent->getGap();
	auto chipSize = parent->getChipSize();

	this->addChild(batch);

	for (int x = 0; x < _mapSize.x; x++)
	{
		for (int y = _mapSize.y - 1; y >= 0; y--)
		{
			auto id = getTile(x, y)->getId();
			auto tile = Sprite::createWithTexture(batch->getTexture(), Rect((id % wnum) * chipSize.x, (int)(id / wnum) * chipSize.y, chipSize.x, chipSize.y));
			tile->setPosition(x * (chipSize.x + gap) + (y % 2) * (chipSize.x + gap) / 2, y * chipSize.y / 2);
			tile->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
			batch->addChild(tile);
		}
	}
}