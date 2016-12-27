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
	static EntityToTile* getInstance() 
	{ 
		static EntityToTile instance;
		return &instance; 
	};
	inline int getSearchCost(TerrainType terrain, EntityType entity) { return _search_cost[terrain][entity]; };
	inline int getEffect(TerrainType terrain, EntityType entity) { return _effect[terrain][entity]; };
};

#endif // __ENTITY_TO_TILE_H__
