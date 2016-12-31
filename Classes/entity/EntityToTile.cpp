#include "EntityToTile.h"

#include "Entity.h"
#include "stage/Tile.h"

#include "util/Util.h"

/*
 * This class is singleton.
 * So the constructor is initializing information.
 */
EntityToTile::EntityToTile()
{
	auto kind = 0;
	for (auto file : { "info/search.csv" , "info/effect.csv" })
	{
		auto lines = util::splitFile(file);
		auto i = -1;
		for (auto line : lines)
		{
			if (++i == 0)
				continue;

			auto items = util::splitString(line, ',');
			auto j = -1;
			for (auto item : items)
			{
				if (++j == 0)
					continue;

				if(kind == 0)
					_search_cost[static_cast<TerrainType>(j-1)][static_cast<EntityType>(i-1)] = std::atoi(item.c_str());
				else
					_effect[static_cast<TerrainType>(j-1)][static_cast<EntityType>(i-1)] = std::atoi(item.c_str());
			}
		}
		kind++;
	}

	// Set sight cost
	_search_cost[TerrainType::prairie][EntityType::sight] = 1;
	_search_cost[TerrainType::woods][EntityType::sight] = 0;
	_search_cost[TerrainType::mountain][EntityType::sight] = 0;
	_search_cost[TerrainType::river][EntityType::sight] = 1;
	_search_cost[TerrainType::ocean][EntityType::sight] = 1;
	_search_cost[TerrainType::road][EntityType::sight] = 1;
	_search_cost[TerrainType::bridge][EntityType::sight] = 0;
	_search_cost[TerrainType::capital][EntityType::sight] = 0;
	_search_cost[TerrainType::city][EntityType::sight] = 0;
	_search_cost[TerrainType::territory][EntityType::sight] = 1;

	//Set counter cost
	_search_cost[TerrainType::prairie][EntityType::counter] = 1;
	_search_cost[TerrainType::woods][EntityType::counter] = 0;
	_search_cost[TerrainType::mountain][EntityType::counter] = 0;
	_search_cost[TerrainType::river][EntityType::counter] = 1;
	_search_cost[TerrainType::ocean][EntityType::counter] = 1;
	_search_cost[TerrainType::road][EntityType::counter] = 1;
	_search_cost[TerrainType::bridge][EntityType::counter] = 0;
	_search_cost[TerrainType::capital][EntityType::counter] = 0;
	_search_cost[TerrainType::city][EntityType::counter] = 0;
	_search_cost[TerrainType::territory][EntityType::counter] = 1;
}
