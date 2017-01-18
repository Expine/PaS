#ifndef __STAGE_H__
#define __STAGE_H__

#include "cocos2d.h"

class Stage;
class StageTile;
class TilePointer;
class City;
class Entity;
class WeaponData;
enum class EntityType;
enum class Owner;

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
	StageLayer();
	virtual ~StageLayer();
public:
	CREATE_FUNC(StageLayer);
	CC_SYNTHESIZE(cocos2d::Vec2, _map_size, MapSize);
	CC_SYNTHESIZE(cocos2d::SpriteBatchNode*, _batch, Batch);
	Stage* getStage();
	StageTile* setTile(cocos2d::Vec2 cor, int id);
	inline  StageTile* setTile(int x, int y, int id) { return setTile(cocos2d::Vec2(x, y), id); };
	StageTile* getTile(int x, int y);
	inline StageTile* getTile(cocos2d::Vec2 cor) { return getTile(cor.x, cor.y); };
	StageTile* removeTile(int x, int y);
	inline StageTile* removeTile(cocos2d::Vec2 cor) { return removeTile(cor.x, cor.y); };
};

/*********************************************************/

/*
 * Unit Layer in unit data
 */
class UnitLayer : public cocos2d::Node
{
protected:
	UnitLayer();
	~UnitLayer();
public:
	CREATE_FUNC(UnitLayer);
	CC_SYNTHESIZE(cocos2d::Vec2, _map_size, MapSize);
	CC_SYNTHESIZE(cocos2d::SpriteBatchNode*, _batch, Batch);
	Stage* getStage();
	Entity* setUnit(cocos2d::Vec2 cor, EntityType type);
	inline Entity* setUnit(int x, int y, EntityType type) { return setUnit(cocos2d::Vec2(x, y), type); };
	void setUnit(cocos2d::Vec2 cor, Entity *unit);
	inline void setUnit(int x, int y, Entity *unit) { setUnit(cocos2d::Vec2(x, y), unit); };
	Entity* getUnit(int x, int y);
	Entity* removeUnit(int x, int y);
	Entity* removeUnit(cocos2d::Vec2 cor) { removeUnit(cor.x, cor.y); };
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
	cocos2d::Sprite* selector;
	cocos2d::Vec2 _pre_provisional_cor;
public:
	/** listener action */
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onTap;
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onDoubleTap;
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onLongTapBegan;
	std::function<void(cocos2d::Vec2, std::vector<StageTile*>)> onLongTapEnd;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipeCheck;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onFlickCheck;
protected:
	Stage();
	virtual ~Stage();
	virtual bool init();
public:
	static Stage* parseStage(const std::string file);
	void addUnitForDebug();
	CREATE_FUNC(Stage);
	CC_SYNTHESIZE(cocos2d::Vec2, _map_size, MapSize);
	CC_SYNTHESIZE(cocos2d::Vec2, _chip_size, ChipSize);
	CC_SYNTHESIZE(int, _gap, Gap);
	CC_SYNTHESIZE(std::string, _background, BackGround);
	CC_SYNTHESIZE(std::string, _bgm, BGM);
	CC_SYNTHESIZE(std::string, _tile_file, TileFile);
	CC_SYNTHESIZE_RETAIN(cocos2d::SpriteBatchNode*, _tile_batch, TileBatch);
	CC_SYNTHESIZE_RETAIN(cocos2d::SpriteBatchNode*, _unit_batch, UnitBatch);

	/** Function for utility **/
	/** Get stage layer*/
	inline StageLayer* getStageLayer(int l) { return dynamic_cast<StageLayer*>(getChildByTag(l)); };
	/** Get unit layer*/
	inline UnitLayer* getUnitLayer() { return dynamic_cast<UnitLayer*>(getChildByTag(3)); };
	/** Get shadow layer*/
	inline StageLayer* getShadowLayer() { return dynamic_cast<StageLayer*>(getChildByTag(4)); };
	/** Get horizontal number of chip*/
	inline int getHorizontalChipNumber() { return _tile_batch->getTextureAtlas()->getTexture()->getContentSize().width / _chip_size.x; };
	/** Get vertical number of chip*/
	inline int getVerticalChipNumber() { return _tile_batch->getTextureAtlas()->getTexture()->getContentSize().height / _chip_size.y; };
	/** Get actual map width*/
	inline float getActualMapWidth() { return _map_size.x * (_chip_size.x + _gap) + (_chip_size.x - _gap) / 2; };
	/** Get actual map height*/
	inline float getActualMapHeight() { return _map_size.y * _chip_size.y / 2 + _chip_size.y / 2; };
	/** adjust ratio from min to max */
	inline float adjustRatio(float ratio) { return ratio > MAX_STAGE_RARIO ? MAX_STAGE_RARIO : ratio < MIN_STAGE_RARIO ? MIN_STAGE_RARIO : ratio; };
	cocos2d::Vec2 adjustArea(cocos2d::Vec2 v);
	cocos2d::Vec2 getPositionAsTile(cocos2d::Vec2 pos);
	cocos2d::Vec2 getPositionByTile(int x, int y);
	inline cocos2d::Vec2 getPositionByTile(cocos2d::Vec2 v) { return getPositionByTile(v.x, v.y); };
	inline std::vector<City*>& getCities(Owner owner) { return _cities[owner]; };
	inline std::map<Owner, std::vector<City*>>& getCities() { return _cities; }
	inline std::vector<Entity*>& getUnits(Owner owner) { return _units[owner]; }
	inline std::map<Owner, std::vector<Entity*>>& getUnits() { return _units; }
	int getUnitNumber();

	/** Function to operate tile **/
	/** Get tile by x, y, layer number*/
	inline StageTile* getTile(int l, int x, int y) { return getStageLayer(l)->getTile(x, y); };
	inline StageTile* getTile(int l, cocos2d::Vec2 cor) { return getTile(l, cor.x, cor.y); };
	std::vector<StageTile*> getTiles(int x, int y);
	inline std::vector<StageTile*> getTiles(cocos2d::Vec2 cor) { return getTiles(cor.x, cor.y); };
	void selectTile(int x, int y);
	inline void selectTile(cocos2d::Vec2 v) { selectTile(v.x, v.y); };
	void blinkTile(StageTile* tile, cocos2d::Color3B color = cocos2d::Color3B::WHITE);
	void blinkChange(StageTile* tile, cocos2d::Color3B color);
	void blinkOffTile(StageTile* tile);

	/** Function to operate unit **/
	Entity* setUnit(int x, int y, EntityType type, const Owner owner);
	inline Entity* setUnit(cocos2d::Vec2 cor, EntityType type, const Owner owner) { return setUnit(cor.x, cor.y, type, owner); };
	void moveUnitPositionAsTile(cocos2d::Vec2 cor, Entity* unit, bool isOnliSetTag = false);
	/** Get unit*/
	inline Entity* getUnit(int x, int y) { return getUnitLayer()->getUnit(x, y); };
	inline Entity* getUnit(cocos2d::Vec2 cor) { return getUnit(cor.x, cor.y); };
	void removeUnit(Entity* unit);
	void blinkUnit(Entity* unit);
	void blinkOffUnit(Entity* unit);

	/** Function to operate stage **/
	void moveView(int x, int y);
	inline void moveView(cocos2d::Vec2 pos) { moveView(pos.x, pos.y); };
	void moveView(Entity* unit);
	void moveView(StageTile* tile);

	void initStage(Owner owner);
	void discoverTile(StageTile* tile);

	/** Function to search area **/
	std::vector<StageTile*> startRecursiveTileSearch(cocos2d::Vec2 point, int remainCost, EntityType type, bool isBlockByUnit = true, bool isContainTileOnUnit = false);
	StageTile* recursiveTileSearch(std::queue<std::function<StageTile*(EntityType, bool, bool)>> &queue, cocos2d::Vec2 intrusion, cocos2d::Vec2 point, int remainCost, EntityType type, bool isBlockByUnit = true, bool isContainTileOnUnit = false);
	std::vector<StageTile*> startRecursiveTileSearchForLiner(cocos2d::Vec2 point, int remainCost);
	std::vector<StageTile*> recursiveTileSearchForLiner(cocos2d::Vec2 intrusion, cocos2d::Vec2 point, int remainCost);
	std::vector<StageTile*> startRecursiveTileSearchForWeapon(Entity* executer, Entity* enemy, WeaponData* weapon);
	std::vector<StageTile*> startRecursiveTileSearchByAstar(const std::vector<StageTile*>& goals, cocos2d::Vec2 start, int limit, EntityType type);
	inline std::vector<StageTile*> startRecursiveTileSearchByAstar(StageTile* goal, cocos2d::Vec2 start, int limit, EntityType type)
	{
		auto v = std::vector<StageTile*>();
		v.push_back(goal);
		return startRecursiveTileSearchByAstar(v, start, limit, type);
	};

	/** Function to implement command **/
	cocos2d::Vec2 nextCity(Owner owner, StageTile* nowTile);
	cocos2d::Vec2 nextUnit(Owner owner, Entity* nowUnit);

	std::vector<StageTile*> moveCheck(Entity* entity);
	std::vector<StageTile*> provisionalMoveUnit(Entity *entity, StageTile *tile);
	void provisionalMoveCancel(Entity *entity);
	void moveUnit(Entity *entiy, std::vector<StageTile*> route);

	void deployUnit(Entity* unit, City* city);
	void dispatchUnit(Entity* unit, City* city);

	cocos2d::Node* renderForAIScene();

};

#endif // __STAGE_H__