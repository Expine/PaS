#include "EntityToTile.h"

#include "Entity.h"
#include "stage/Tile.h"

/*
 * This class is singleton.
 * So the constructor is initializing information.
 */
EntityToTile::EntityToTile()
{
	// Set default value
	int defaultCost[] = { 0, 3, 4, 20, 10, 99, 2, 3, 0, 0, 0 };
	for (int j = 0; j < static_cast<int>(TerrainType::COUNT); j++)
	{
		for (int i = 0; i < static_cast<int>(EntityType::COUNT); i++)
		{
			_search_cost[static_cast<TerrainType>(j)][static_cast<EntityType>(i)] = defaultCost[j];
			_effect[static_cast<TerrainType>(j)][static_cast<EntityType>(i)] = 0;
		}
	}

	// Set unique search cost
	_search_cost[TerrainType::woods][EntityType::guardian] = 2;
	_search_cost[TerrainType::woods][EntityType::dark] = 3;
	_search_cost[TerrainType::mountain][EntityType::ground] = 5;
	_search_cost[TerrainType::river][EntityType::ice] = 5;
	_search_cost[TerrainType::ocean][EntityType::ice] = 10;
	_search_cost[TerrainType::ocean][EntityType::king] = 20;
	_search_cost[TerrainType::ocean][EntityType::light] = 30;
	_search_cost[TerrainType::road][EntityType::thunder] = 1;

	// Set unique effect
	_effect[TerrainType::woods][EntityType::fire] = 50;
	_effect[TerrainType::woods][EntityType::guardian] = 20;
	_effect[TerrainType::mountain][EntityType::ground] = 50;
	_effect[TerrainType::river][EntityType::ice] = 20;
	_effect[TerrainType::river][EntityType::thunder] = 40;
	_effect[TerrainType::ocean][EntityType::ice] = 50;
	_effect[TerrainType::city][EntityType::weapon] = 50;
	_effect[TerrainType::capital][EntityType::weapon] = 50;
}
