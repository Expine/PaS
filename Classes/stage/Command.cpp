#include "Command.h"
#include "entity/Entity.h"
#include "stage/Tile.h"
#include "stage/Stage.h"
#include "ai/Owner.h"
#include "util/Util.h"

const std::string command::getName(UnitCommand com)
{
	switch (com)
	{
	case UnitCommand::supply: return u8"�⋋";
	case UnitCommand::move: return u8"�ړ�";
	case UnitCommand::attack: return u8"�U��";
	case UnitCommand::occupation: return u8"���";
	case UnitCommand::spec: return u8"���\";
	case UnitCommand::wait: return u8"�ҋ@";
	default: return "ERROR";
	}
};
const std::string command::getName(CityCommand com)
{
	switch (com)
	{
	case CityCommand::supply: return u8"�⋋";
	case CityCommand::deployment: return u8"�z��";
	case CityCommand::dispatch: return u8"�h��";
	default: return "ERROR";
	}
}
const std::string command::getName(MoveCommand com)
{
	switch (com)
	{
	case MoveCommand::start: return u8"�m��";
	case MoveCommand::end: return u8"���~";
	default: return "ERROR";
	}
}

bool command::isEnable(UnitCommand com, Entity * unit, std::vector<StageTile*> tile)
{
	if (com == UnitCommand::supply)
		return unit->getState() != EntityState::acted;
	else if (com == UnitCommand::move)
		return unit->getState() < EntityState::moved;
	else if (com == UnitCommand::attack)
	{
		if (unit->getState() == EntityState::acted)
			return false;

		auto stage = unit->getStage();
		auto pos = unit->getTileCoordinate(stage->getMapSize().y);
		CCLOG("POS(%f,%f)", pos.x, pos.y);
		auto tiles = stage->startRecursiveTileSearch(pos, 1, EntityType::counter);
		for (auto t : tiles)
		{
			auto cor = t->getTileCoordinate(stage->getMapSize().y);
			auto unit = stage->getUnit(cor.x, cor.y);
			CCLOG("(%f,%f)", cor.x, cor.y);
			if (unit && unit->getAffiliation() != Owner::player)
				return true;
		}
		return false;
	}
	else if (com == UnitCommand::occupation)
	{
		if (unit->getState() == EntityState::acted)
			return false;

		auto stage = unit->getStage();
		auto pos = unit->getTileCoordinate(stage->getMapSize().y);
		auto tiles = stage->getTiles(pos.x, pos.y);
		for (auto t : tiles)
			if (t->getTerrainType() == TerrainType::city || t->getTerrainType() == TerrainType::capital)
				if (util::instance<City>(t)->getOwner() != Owner::player)
					return true;
		return false;

	}
	else if (com == UnitCommand::spec)
		return true;
	else if (com == UnitCommand::wait)
		return unit->getState() != EntityState::acted;
}

bool command::isEnable(CityCommand com, Entity * unit, std::vector<StageTile*> tile)
{
	return true;
}

bool command::isEnable(MoveCommand com, Entity * unit, std::vector<StageTile*> tile)
{
	return true;
}
