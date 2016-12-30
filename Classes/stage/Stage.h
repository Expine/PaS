#ifndef __STAGE_H__
#define __STAGE_H__

#include "cocos2d.h"

#include "ai/Owner.h"

class StageTile;
class City;
class Entity;
enum class EntityType;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

constexpr float MIN_STAGE_RARIO = 1.0f;
constexpr float MAX_STAGE_RARIO = 7.0f;
constexpr float STAGE_RATIO_RATIO = 0.5f;

/*********************************************************/

/*
 * Stage Layer in tile data
 */
class StageLayer : public cocos2d::Node
{
protected:
	StageLayer()
		: _batch(nullptr)
	{};
	~StageLayer()
	{
		_batch = nullptr;
	}
public:
	CREATE_FUNC(StageLayer);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	CC_SYNTHESIZE(cocos2d::SpriteBatchNode*, _batch, Batch);
	StageTile* setTile(int x, int y, int id);
	StageTile* getTile(int x, int y);
	StageTile* removeTile(int x, int y);
};

/*********************************************************/

/*
 * Unit Layer in unit data
 */
class UnitLayer : public cocos2d::Node
{
protected:
	UnitLayer()
		: _batch(nullptr)
	{};
	~UnitLayer()
	{
		_batch = nullptr;
	};
public:
	CREATE_FUNC(UnitLayer);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	CC_SYNTHESIZE(cocos2d::SpriteBatchNode*, _batch, Batch);
	Entity* setUnit(int x, int y, EntityType type);
	Entity* getUnit(int x, int y);
};

/*********************************************************/

/*
 * Stage class
 */
class Stage : public cocos2d::Node
{
private:
	/** city and unit vector */
	std::map<Owner, std::vector<City*>> _cities;
	std::map<Owner, std::vector<Entity*>> _units;
	std::queue<std::function<std::vector<StageTile*>()>> searchQueue;
	std::vector<StageTile*> _searchResult;
	int searchCost;
public:
	/** listener action */
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onTap;
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onLongTapBegan;
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onLongTapEnd;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipeCheck;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onFlickCheck;
protected:
	Stage()
		: _gap(0)
		, onTap(nullptr), onLongTapBegan(nullptr), onLongTapEnd(nullptr), onSwipeCheck(nullptr), onFlickCheck(nullptr)
	{};
	~Stage()
	{
		_gap = 0;
		onTap = onLongTapBegan = onLongTapEnd = nullptr;
		onSwipeCheck = onFlickCheck = nullptr;
	}
	virtual bool init();
public:
	static Stage* parseStage(const std::string file);
	CREATE_FUNC(Stage);
	CC_SYNTHESIZE(cocos2d::Vec2, _mapSize, MapSize);
	CC_SYNTHESIZE(cocos2d::Vec2, _chipSize, ChipSize);
	CC_SYNTHESIZE(int, _gap, Gap);
	CC_SYNTHESIZE(std::string, _background, BackGround);
	CC_SYNTHESIZE(std::string, _bgm, BGM);
	CC_SYNTHESIZE(std::string, _tileFile, TileFile);
	/** Get stage layer*/
	inline StageLayer* getStageLayer(int l) { return dynamic_cast<StageLayer*>(getChildByTag(l)); };
	/** Get unit layer*/
	inline UnitLayer* getUnitLayer() { return dynamic_cast<UnitLayer*>(getChildByTag(3)); };
	/** Get shadow layer*/
	inline StageLayer* getShadowLayer() { return dynamic_cast<StageLayer*>(getChildByTag(4)); };

	/** Get tile by x, y, layer number*/
	inline StageTile* getTile(int l, int x, int y) { return getStageLayer(l)->getTile(x, y); };
	std::vector<StageTile*> getTiles(int x, int y);
	void blinkTile(StageTile* tile, cocos2d::Color3B color = cocos2d::Color3B::WHITE);
	void blinkChange(StageTile* tile, cocos2d::Color3B color);
	void blinkOffTile(StageTile* tile);

	/** Set unit*/
	void setUnit(int x, int y, EntityType type, const Owner owner);
	/** Get unit*/
	inline Entity* getUnit(int x, int y) { return getUnitLayer()->getUnit(x, y); };
	void blinkUnit(Entity* unit);
	void blinkOffUnit(Entity* unit);

	/** Get map width*/
	inline float getWidth() { return _mapSize.x * (_chipSize.x + _gap) + (_chipSize.x - _gap) / 2; };
	/** Get map height*/
	inline float getHeight() { return _mapSize.y * _chipSize.y / 2 + _chipSize.y / 2; };

	/** adjust ratio from min to max */
	inline float adjustRatio(float ratio) { return max(MIN_STAGE_RARIO, min(MAX_STAGE_RARIO, ratio)); };
	cocos2d::Vec2 adjustArea(cocos2d::Vec2 v);

	cocos2d::Vec2 getTileCoordinate(cocos2d::Vec2 cor);
	cocos2d::Vec2 getCoordinateByTile(int x, int y);
	void movePosition(int x, int y);

	void initTileSearched(Owner owner);
	std::vector<StageTile*> startRecursiveTileSearch(cocos2d::Vec2 point, int remainCost, EntityType type);
	std::vector<StageTile*> recursiveTileSearch(cocos2d::Vec2 intrusion, cocos2d::Vec2 point, int remainCost, EntityType type);
	std::vector<StageTile*> startRecursiveTileSearchForMove(cocos2d::Vec2 goal, cocos2d::Vec2 point, int remainCost, EntityType type);
	std::vector<StageTile*> recursiveTileSearchForMove(cocos2d::Vec2 goal, cocos2d::Vec2 intrusion, cocos2d::Vec2 point, int remainCost, EntityType type, std::vector<StageTile*> result);

	cocos2d::Vec2 nextCity(Owner owner, StageTile* nowTile);
	cocos2d::Vec2 nextUnit(Owner owner, Entity* nowUnit);

	std::vector<StageTile*> moveCheck(Entity* entity);
	void moveUnit(Entity *entiy, StageTile* tile);
};

#endif // __STAGE_H__