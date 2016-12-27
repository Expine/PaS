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

	// Set unit layer
	auto layer = UnitLayer::create();
	layer->setTag(3);
	layer->setMapSize(stage->getMapSize());
	auto batch = SpriteBatchNode::create("image/unit.png");
	batch->setTag(0);
	layer->setBatch(batch);
	layer->addChild(batch);
	stage->addChild(layer, 10);

	//Set chip data
	int count = 0;
	for (int l = 0; l < 3; l++)
	{
		auto batch = SpriteBatchNode::create("TileSet/" + stage->getTileFile());
		batch->setTag(0);
		auto layer = dynamic_cast<StageLayer*>(stage->getChildByTag(l));
		layer->setBatch(batch);
		layer->addChild(batch);
		for (int x = 0; x < stage->getMapSize().x; x++)
		{
			for (int y = 0; y < stage->getMapSize().y; y++)
			{
				if (sLine.size() == count)
					break;
				layer->setTile(x, y, std::atoi(sLine[count++].c_str()));
			}
		}
	}

	std::string names[] = { "My", "Enemy", "Friend" };
	std::vector<Vec2> poses;
	bool check = true;
	for (int i = 0; i < 100; i++)
	{
		auto pos = Vec2(std::rand() % (int)(stage->getMapSize().x), std::rand() % (int)(stage->getMapSize().y));
		auto type = stage->getTile(0, pos.x, pos.y)->getTerrainType();
		if (type == TerrainType::ocean)
			continue;
		if (type == TerrainType::river)
			continue;
		for (auto p : poses)
			if (p.x == pos.x && p.y == pos.y)
			{
				check = false;
			}
		if (check)
		{
			stage->setUnit(pos.x, pos.y, static_cast<EntityType>(std::rand() % static_cast<int>(EntityType::COUNT)), names[i % 3]);
			poses.push_back(pos);
		}
		check = true;
	}

	return stage;
}

bool Stage::init()
{
	if (!Node::init())
		return false;

	//Set listener
	auto listener = MultiTouchListener::create();
	listener->onTouchBegan = [this](Vec2 v)
	{
		stopAllActions();
		setPosition(adjustArea(getPosition()));
	};
	listener->onTap = [this](Vec2 v)
	{
		if (onTap)
		{
			auto cor = getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			onTap(cor, tiles);
		}
	};
	listener->onSwipeCheck = [this](Vec2 v, Vec2 diff, float time)
	{
		return !onSwipeCheck || onSwipeCheck(v, diff, time);
	};
	listener->onSwipe = [this](Vec2 v, Vec2 diff, float time)
	{
		setPosition(adjustArea(getPosition() + diff));
	};
	listener->onFlickCheck = [this](Vec2 v, Vec2 diff, float time)
	{
		return !onFlickCheck || onFlickCheck(v, diff, time);
	};
	listener->onFlick = [this](Vec2 v, Vec2 diff, float time)
	{
		runAction(Spawn::create(
			Sequence::create(
				MoveBy::create(0.5f, diff / time / 3),
				DelayTime::create(0.5f),
				CallFunc::create([this] {
			stopAllActions();
		}),
				NULL),
			Repeat::create(Sequence::create(
				CallFunc::create([this] {
			setPosition(adjustArea(getPosition()));
		}),
				DelayTime::create(0.005f),
			NULL)
				, -1),
			NULL));
	};
	listener->onLongTapBegan = [this](Vec2 v)
	{
		if (onLongTapBegan)
		{
			auto cor = getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			onLongTapBegan(cor, tiles);
		}
	};
	listener->onLongTapEnd = [this](Vec2 v)
	{
		if (onLongTapEnd)
		{
			auto cor = getTileCoordinate(v);
			std::vector<StageTile*> tiles;
			for (int i = 0; i < 3; i++)
			{
				auto tile = getTile(i, cor.x, cor.y);
				if (tile->getId() != 0)
					tiles.push_back(tile);
			}
			onLongTapEnd(cor, tiles);
		}
	};
	listener->pinchIn = [this](Vec2 v, float ratio)
	{
		ratio = ratio / (1 + ratio) / STAGE_RATIO_RATIO;
		auto diff_ratio = getScale();
		setScale(adjustRatio(getScale() * ratio));
		diff_ratio = getScale() / diff_ratio;
		auto diff_x = v.x * (1 - diff_ratio);
		auto diff_y = v.y * (1 - diff_ratio);
		auto pos = (getPosition() + Vec2(diff_x, diff_y));
		setPosition(adjustArea(getPosition()  * diff_ratio + Vec2(diff_x, diff_y)));
	};
	listener->pinchOut = [this](Vec2 v, float ratio)
	{
		ratio = ratio / (1 + ratio) / STAGE_RATIO_RATIO;
		auto diff_ratio = getScale();
		setScale(adjustRatio(getScale() * ratio));
		diff_ratio = getScale() / diff_ratio;
		auto diff_x = v.x * (1 - diff_ratio);
		auto diff_y = v.y * (1 - diff_ratio);
		auto pos = (getPosition() + Vec2(diff_x, diff_y));
		setPosition(adjustArea(getPosition()  * diff_ratio + Vec2(diff_x, diff_y)));
	};
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	auto mouseListener = MultiMouseListener::create(listener);
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);

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

void StageLayer::setTile(int x, int y, int id)
{
	_batch->addChild(StageTile::create(id, x, y, _batch, dynamic_cast<Stage*>(getParent())));
}

Entity* UnitLayer::setUnit(int x, int y, EntityType type)
{
	auto entity = Entity::create(type, x, y, _batch, dynamic_cast<Stage*>(getParent()));
	_batch->addChild(entity);
	return entity;
}
