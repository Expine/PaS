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
	case UnitCommand::supply: return u8"補給";
	case UnitCommand::move: return u8"移動";
	case UnitCommand::attack: return u8"攻撃";
	case UnitCommand::occupation: return u8"占領";
	case UnitCommand::spec: return u8"性能";
	case UnitCommand::wait: return u8"待機";
	default: return "ERROR";
	}
};
const std::string command::getName(CityCommand com)
{
	switch (com)
	{
	case CityCommand::supply: return u8"補給";
	case CityCommand::deployment: return u8"配備";
	case CityCommand::dispatch: return u8"派遣";
	default: return "ERROR";
	}
}
const std::string command::getName(MoveCommand com)
{
	switch (com)
	{
	case MoveCommand::start: return u8"開始";
	case MoveCommand::end: return u8"中止";
	case MoveCommand::decision: return u8"確定";
	case MoveCommand::cancel: return u8"撤回";
	default: return "ERROR";
	}
}

const std::string command::getName(AttackCommand com)
{
	switch (com)
	{
	case AttackCommand::decision: return u8"確定";
	case AttackCommand::cancel: return u8"撤回";
	case AttackCommand::attack: return u8"開始";
	case AttackCommand::end: return u8"撤回";
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
		auto tiles = stage->startRecursiveTileSearch(pos, 1, EntityType::counter);
		for (auto t : tiles)
		{
			auto cor = t->getTileCoordinate(stage->getMapSize().y);
			auto unit = stage->getUnit(cor.x, cor.y);
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
	return true;
}

bool command::isEnable(CityCommand com, Entity * unit, std::vector<StageTile*> tile)
{
	return true;
}

bool command::isEnable(MoveCommand com, Entity * unit, std::vector<StageTile*> tile)
{
	return true;
}

bool command::isEnable(AttackCommand com, Entity * unit, std::vector<StageTile*> tile)
{
	if (com == AttackCommand::decision)
		return unit && unit->getAffiliation() != Owner::player;
	else
		return true;
}
