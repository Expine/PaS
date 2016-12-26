#ifndef __ENTITY_TO_TILE_H__
#define __ENTITY_TO_TILE_H__

#include "cocos2d.h"

extern enum class TerrainType;
extern enum class EntityType;

class EntityToTile
{
private:
	std::map<TerrainType, std::map<EntityType, int>> _search_cost;
	std::map<TerrainType, std::map<EntityType, int>> _effect;
protected:
	EntityToTile();
public:
	EntityToTile* getInstance() 
	{ 
		static EntityToTile instance;
		return &instance; 
	};
};

#endif // __ENTITY_TO_TILE_H__
