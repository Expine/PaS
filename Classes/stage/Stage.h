#ifndef __STAGE_H__
#define __STAGE_H__

#include "cocos2d.h"
#include "Tile.h"

class Stage;

class StageLayer : public cocos2d::Node
{
private:
	cocos2d::Vector<StageTile*> _tiles;
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
	static Stage* parseStage(const std::string file);
	void render();
};

#endif // __STAGE_H__