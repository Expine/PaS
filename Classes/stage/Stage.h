#ifndef __STAGE_H__
#define __STAGE_H__

#include "cocos2d.h"
#include "Tile.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

class Stage;

constexpr float MIN_STAGE_RARIO = 1.0f;
constexpr float MAX_STAGE_RARIO = 7.0f;
constexpr float STAGE_RATIO_RATIO = 0.5f;

class StageLayer : public cocos2d::Node
{
private:
	cocos2d::Vector<StageTile*> _tiles;
protected:
	StageLayer()
	{

	};
	~StageLayer()
	{
		_tiles.clear();
		_tiles.shrinkToFit();
	}
public:
	CREATE_FUNC(StageLayer);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	inline void addTile(StageTile* tile) { _tiles.pushBack(tile); };
	inline StageTile* getTile(int x, int y)
	{
		return _tiles.at(x * _mapSize.y + y);
	};
	void render();
};

/*********************************************************/

class Stage : public cocos2d::Node
{
private:
	cocos2d::Vec2 getTileCoordinate(cocos2d::Vec2 cor);
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
public:
	virtual bool init();
	CREATE_FUNC(Stage);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	CC_SYNTHESIZE(cocos2d::Vec2, _chipSize, ChipSize);
	CC_SYNTHESIZE(int, _gap, Gap);
	CC_SYNTHESIZE(std::string, _background, BackGround);
	CC_SYNTHESIZE(std::string, _bgm, BGM);
	CC_SYNTHESIZE(std::string, _tileFile, TileFile);
	inline void addTile(int l, StageTile* tile) { if(l<3) dynamic_cast<StageLayer*>(getChildByTag(l))->addTile(tile); };
	inline StageTile* getTile(int l, int x, int y)
	{
		return dynamic_cast<StageLayer*>(getChildByTag(l))->getTile(x, y);
	};
	std::vector<cocos2d::Sprite*> getTileSprite(int x, int y)
	{
		std::vector<cocos2d::Sprite*> tiles;
		for (int i = 0; i < 3; i++)
		{
			auto tile = dynamic_cast<cocos2d::Sprite*>(getChildByTag(i)->getChildByTag(0)->getChildByTag(x * _mapSize.y + y));
			if (!tile)
				continue;
			tiles.push_back(tile);
		}
		return tiles;
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
	void render();
};

#endif // __STAGE_H__