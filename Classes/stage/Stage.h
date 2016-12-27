#ifndef __STAGE_H__
#define __STAGE_H__

#include "cocos2d.h"
#include "Tile.h"
#include "entity/Entity.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

constexpr float MIN_STAGE_RARIO = 1.0f;
constexpr float MAX_STAGE_RARIO = 7.0f;
constexpr float STAGE_RATIO_RATIO = 0.5f;

class StageLayer : public cocos2d::Node
{
protected:
	StageLayer()
		: _batch(nullptr)
	{

	};
public:
	CREATE_FUNC(StageLayer);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	CC_SYNTHESIZE(cocos2d::SpriteBatchNode*, _batch, Batch);
	void setTile(int x, int y, int id);
	inline StageTile* getTile(int x, int y)
	{
		return dynamic_cast<StageTile*>(getChildByTag(0)->getChildByTag(x * _mapSize.y + y));
	};
};
/*********************************************************/

class UnitLayer : public cocos2d::Node
{
protected:
	UnitLayer()
		: _batch(nullptr)
	{

	};
public:
	CREATE_FUNC(UnitLayer);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	CC_SYNTHESIZE(cocos2d::SpriteBatchNode*, _batch, Batch);
	Entity* setUnit(int x, int y, EntityType type);
	Entity* getUnit(int x, int y)
	{
		return dynamic_cast<Entity*>(getChildByTag(0)->getChildByTag(x * _mapSize.y + y));
	}
};

/*********************************************************/

class Stage : public cocos2d::Node
{
private:
	std::vector<City*> _cities;
	std::map<std::string, std::vector<Entity*>> _units;
public:
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onTap;
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onLongTapBegan;
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onLongTapEnd;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipeCheck;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onFlickCheck;
protected:
	Stage()
		: _gap(0)
		, onTap(nullptr)
		, onLongTapBegan(nullptr)
		, onLongTapEnd(nullptr)
	{

	};
	virtual bool init();
public:
	CREATE_FUNC(Stage);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	CC_SYNTHESIZE(cocos2d::Vec2, _chipSize, ChipSize);
	CC_SYNTHESIZE(int, _gap, Gap);
	CC_SYNTHESIZE(std::string, _background, BackGround);
	CC_SYNTHESIZE(std::string, _bgm, BGM);
	CC_SYNTHESIZE(std::string, _tileFile, TileFile);
	inline StageTile* getTile(int l, int x, int y)
	{
		return dynamic_cast<StageLayer*>(getChildByTag(l))->getTile(x, y);
	};
	std::vector<StageTile*> getTiles(int x, int y)
	{
		std::vector<StageTile*> tiles;
		for (int i = 0; i < 3; i++)
		{
			auto tile = getTile(i, x, y);
			if (!tile)
				continue;
			tiles.push_back(tile);
		}
		return tiles;
	};
	inline void setUnit(int x, int y, EntityType type, const std::string &name)
	{
		_units[name].push_back(dynamic_cast<UnitLayer*>(getChildByTag(3))->setUnit(x, y, type));
	};
	inline Entity* getUnit(int x, int y)
	{
		return dynamic_cast<UnitLayer*>(getChildByTag(3))->getUnit(x, y);
	};
	inline float getWidth() { return _mapSize.x * (_chipSize.x + _gap) + (_chipSize.x - _gap) / 2; };
	inline float getHeight() { return _mapSize.y * _chipSize.y / 2 + _chipSize.y / 2; };
	cocos2d::Vec2 adjustArea(cocos2d::Vec2 v) {
		auto maxWidth = getWidth() * getScale() - cocos2d::Director::getInstance()->getWinSize().width;
		auto maxHeight = getHeight() * getScale() - cocos2d::Director::getInstance()->getWinSize().height;
		return cocos2d::Vec2(
			(maxWidth < 0) ? 0 : max(-maxWidth, min(v.x, 0)),
			(maxHeight < 0) ? 0 : max(-maxHeight, min(v.y, 0)));
	};
	inline float adjustRatio(float ratio) { return max(MIN_STAGE_RARIO, min(MAX_STAGE_RARIO, ratio)); };
	static Stage* parseStage(const std::string file);

	cocos2d::Vec2 getTileCoordinate(cocos2d::Vec2 cor);
	inline cocos2d::Vec2 getCoordinateByTile(int x, int y)
	{
		return cocos2d::Vec2(x * (_chipSize.x + _gap) + (y % 2) * (_chipSize.x + _gap) / 2, (_mapSize.y - 1 - y) * _chipSize.y / 2);
	};
};

#endif // __STAGE_H__