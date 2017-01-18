#ifndef __ENTITY_TO_TILE_H__
#define __ENTITY_TO_TILE_H__

#include "cocos2d.h"

enum class TerrainType;
enum class EntityType;

/*
 * Entity to tile information.
 * Search cost
 * Terrain effect
 */
class EntityToTile
{
private:
	std::map<TerrainType, std::map<EntityType, int>> _search_cost;
	std::map<TerrainType, std::map<EntityType, int>> _effect;
protected:
	EntityToTile();
public:
	/** Get instance for singleton */
	static EntityToTile* getInstance()
	{ 
		static EntityToTile instance;
		return &instance; 
	};
	/** Get serach cost by terrain and unit*/
	inline int getSearchCost(TerrainType terrain, EntityType entity) { return _search_cost[terrain][entity]; };
	/** Get effect by terrain and unit*/
	inline int getEffect(TerrainType terrain, EntityType entity) { return _effect[terrain][entity]; };
};

#endif // __ENTITY_TO_TILE_H__
