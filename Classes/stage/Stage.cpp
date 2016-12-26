﻿#include "Stage.h"

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

	auto layer = UnitLayer::create();
	layer->setTag(3);
	stage->addChild(layer);

	//Set chip data
	int count = 0;
	for (int l = 0; l < 3; l++)
	{
		auto batch = SpriteBatchNode::create("TileSet/" + stage->getTileFile());
		batch->setTag(0);
		for (int x = 0; x < stage->getMapSize().x; x++)
		{
			for (int y = 0; y < stage->getMapSize().y; y++)
			{
				if (sLine.size() == count)
					break;
				batch->addChild(StageTile::create(std::atoi(sLine[count++].c_str()), x, y, batch, stage));
			}
		}
		stage->getChildByTag(l)->addChild(batch);
	}

	//Set listener
	auto listener = MultiTouchListener::create();
	listener->onTouchBegan = [stage](Vec2 v)
	{
		stage->stopAllActions();
		stage->setPosition(stage->adjustArea(stage->getPosition()));
	};
	listener->onTap = [stage] (Vec2 v)
	{
		CCLOG("Tapped (%f, %f)", v.x, v.y);
		if (stage->onTap)
		{
			auto cor = stage->getTileCoordinate(v);
			CCLOG("TileCor (%f, %f)", cor.x, cor.y);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = stage->getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			stage->onTap(cor, tiles);
		}
	};
	listener->onSwipeCheck = [stage](Vec2 v, Vec2 diff, float time)
	{
		return !stage->onSwipeCheck || stage->onSwipeCheck(v, diff, time);
	};
	listener->onSwipe = [stage](Vec2 v, Vec2 diff, float time)
	{
		CCLOG("Swipe (%f, %f) diff(%f, %f) time:%f", v.x, v.y, diff.x, diff.y, time);
		stage->setPosition(stage->adjustArea(stage->getPosition() + diff));
		CCLOG("RESULT (%f, %f)", stage->getPosition().x, stage->getPosition().y);
	};
	listener->onFlickCheck = [stage](Vec2 v, Vec2 diff, float time)
	{
		return !stage->onFlickCheck || stage->onFlickCheck(v, diff, time);
	};
	listener->onFlick = [stage](Vec2 v, Vec2 diff, float time)
	{
		CCLOG("Flick (%f, %f) time:%f", diff.x, diff.y, time);
		stage->runAction(Spawn::create(
			Sequence::create(
				MoveBy::create(0.5f, diff / time / 3),
				DelayTime::create(0.5f),
				CallFunc::create([stage] {
					stage->stopAllActions();
				}),
			NULL),
			Repeat::create(Sequence::create(
				CallFunc::create([stage] {
					stage->setPosition(stage->adjustArea(stage->getPosition()));
				}),
				DelayTime::create(0.005f),
				NULL)
			, -1),
		NULL));
	};
	listener->onLongTapBegan = [stage](Vec2 v)
	{
		CCLOG("LongTapBegan (%f, %f)", v.x, v.y);
		if (stage->onLongTapBegan)
		{
			auto cor = stage->getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = stage->getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			stage->onLongTapBegan(cor, tiles);
		}
	};
	listener->onLongTapEnd = [stage](Vec2 v)
	{
		CCLOG("LongTapEnd (%f, %f)", v.x, v.y);
		if (stage->onLongTapEnd)
		{
			auto cor = stage->getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = stage->getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			stage->onLongTapEnd(cor, tiles);
		}
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

Vec2 Stage::getTileCoordinate(Vec2 cor)
{
	auto fix = (cor - getPosition()) / getScale();
	fix.y = getHeight() - fix.y;
	auto even = (int)fix.x % (int)(_chipSize.x + _gap) <= (_chipSize.x + _gap) / 2;
	auto x = (int)(fix.x / (_chipSize.x + _gap));
	auto y = (int)((fix.y - (even ? 0 : _chipSize.y / 2)) / _chipSize.y) * 2 + (even ? 0 : 1);
	return Vec2(x, y);
}	