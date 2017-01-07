#include "Command.h"
#include "entity/Entity.h"
#include "stage/Tile.h"
#include "stage/Stage.h"
#include "ai/Owner.h"
#include "util/Util.h"

#define forCommand(counter, first, end) for(int counter=static_cast<int>(first)+1;counter<static_cast<int>(end);counter++)
#define forFunc(counter, name) forCommand(i, Command::name _START, Command::name _END)

void command::forMenu(std::function<void(Command, int)> func)
{
	forCommand(i, Command::MENU_START, Command::MENU_END)
		func(castCommand(i), i - static_cast<int>(Command::MENU_START) - 1);
}

void command::forUnit(std::function<void(Command, int)> func)
{
	forCommand(i, Command::UNIT_START, Command::UNIT_END)
		func(castCommand(i), i - static_cast<int>(Command::UNIT_START) - 1);
}

void command::forMove(std::function<void(Command, int)> func)
{
	forCommand(i, Command::MOVE_START, Command::MOVE_END)
		func(castCommand(i), i - static_cast<int>(Command::MOVE_START) - 1);
}

void command::forMove2(std::function<void(Command, int)> func)
{
	forCommand(i, Command::MOVE_2_START, Command::MOVE_2_END)
		func(castCommand(i), i - static_cast<int>(Command::MOVE_2_START) - 1);
}

void command::forAttack(std::function<void(Command, int)> func)
{
	forCommand(i, Command::ATTACK_START, Command::ATTACK_END)
		func(castCommand(i), i - static_cast<int>(Command::ATTACK_START) - 1);
}

void command::forAttack2(std::function<void(Command, int)> func)
{
	forCommand(i, Command::ATTACK_2_START, Command::ATTACK_2_END)
		func(castCommand(i), i - static_cast<int>(Command::ATTACK_2_START) - 1);
}

void command::forOccupation(std::function<void(Command, int)> func)
{
	forCommand(i, Command::OCCUPATION_START, Command::OCCUPATION_END)
		func(castCommand(i), i - static_cast<int>(Command::OCCUPATION_START) - 1);
}

void command::forWait(std::function<void(Command, int)> func)
{
	forCommand(i, Command::WAIT_START, Command::WAIT_END)
		func(castCommand(i), i - static_cast<int>(Command::WAIT_START) - 1);
}

void command::forCity(std::function<void(Command, int)> func)
{
	forCommand(i, Command::CITY_START, Command::CITY_END)
		func(castCommand(i), i - static_cast<int>(Command::CITY_START) - 1);
}

void command::forCitySupply(std::function<void(Command, int)> func)
{
	forCommand(i, Command::CITY_SUPPLY_START, Command::CITY_SUPPLY_END)
		func(castCommand(i), i - static_cast<int>(Command::CITY_SUPPLY_START) - 1);
}

void command::forDeploy(std::function<void(Command, int)> func)
{
	forCommand(i, Command::DEPLOY_START, Command::DEPLOY_END)
		func(castCommand(i), i - static_cast<int>(Command::DEPLOY_START) - 1);
}

void command::forDispatch(std::function<void(Command, int)> func)
{
	forCommand(i, Command::DISPATCH_START, Command::DISPATCH_START)
		func(castCommand(i), i - static_cast<int>(Command::DISPATCH_START) - 1);
}

void command::forAllUnit(std::function<void(Command, int)> func)
{
	forUnit(func);
	forMove(func);
	forMove2(func);
	forAttack(func);
	forAttack2(func);
	forOccupation(func);
	forWait(func);
}

void command::forAllCity(std::function<void(Command, int)> func)
{
	forCity(func);
	forCitySupply(func);
	forDeploy(func);
	forDispatch(func);
}

const std::string command::getName(Command com)
{
	switch (com)
	{
	case Command::endPhase: return u8"フェイズ終了";
	case Command::nextUnit: return u8"次のユニット";
	case Command::nextCity: return u8"次の都市";
	case Command::talkStaff: return u8"参謀と話す";
	case Command::save: return u8"セーブ";
	case Command::load: return u8"ロード";
	case Command::option: return u8"オプション";

	case Command::supply: return u8"補給";
	case Command::move: return u8"移動";
	case Command::attack: return u8"攻撃";
	case Command::occupation: return u8"占領";
	case Command::spec: return u8"性能";
	case Command::wait: return u8"待機";

	case Command::city_supply: return u8"補給";
	case Command::deployment: return u8"配備";
	case Command::dispatch: return u8"派遣";

	case Command::move_start: return u8"開始";
	case Command::move_end: return u8"中止";
	case Command::move_decision: return u8"確定";
	case Command::move_cancel: return u8"撤回";

	case Command::attack_target: return u8"照準";
	case Command::attack_change: return u8"変更";
	case Command::attack_end: return u8"中止";
	case Command::attack_start: return u8"開始";
	case Command::attack_cancel: return u8"撤回";

	case Command::occupation_start: return u8"実行";
	case Command::occupation_end: return u8"中止";

	case Command::wait_start: return u8"実行";
	case Command::wait_end: return u8"中止";

	case Command::city_supply_start: return u8"決定";
	case Command::city_supply_end: return u8"中止";

	case Command::deploy_start: return u8"決定";
	case Command::deploy_end: return u8"中止";

	case Command::dispatch_start: return u8"出発";
	case Command::dispatch_change: return u8"別隊";
	case Command::dispatch_cancel: return u8"中止";

	default: return "ERROR";
	}
};

bool command::isEnable(Command com, Entity * unit, std::vector<StageTile*> tiles)
{
	// Check unit can use this command
	if (unit && !EntityInformation::getInstance()->getCommand(unit->getType(), com))
		return false;

	switch (com)
	{
	// Unit Command
	case Command::supply:
		return unit && unit->getState() != EntityState::acted;
	case Command::move:
		return unit && unit->getState() < EntityState::moved;
	case Command::attack:
	{
		if (!unit || unit->getState() == EntityState::acted)
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
	case Command::occupation:
	{
		if (!unit || unit->getState() == EntityState::acted)
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
	case Command::spec:
		return true;
	case Command::wait:
		return unit && unit->getState() != EntityState::acted;

	// City Command
	case Command::city_supply:
	{
		StageTile* tile;
		for (auto t : tiles)
			if(t && t->getId() != 0)
				tile = t;
		auto stage = tile->getStage();
		auto pos = tile->getTileCoordinate(stage->getMapSize().y);
		auto unit = stage->getUnit(pos.x, pos.y);
		if (unit && unit->getAffiliation() == Owner::player)
			return true;
		return false;
	}
	case Command::deployment:
	case Command::dispatch:
		return true;

	// Move Command
	case Command::move_start:
	case Command::move_end:
	case Command::move_decision:
	case Command::move_cancel:
		return true;

	// Attack Command
	case Command::attack_target:
	case Command::attack_change:
	case Command::attack_end:
	case Command::attack_start:
	case Command::attack_cancel:
		return true;

	// Occupation Command
	case Command::occupation_start:
	case Command::occupation_end:
		return true;

	// Wait Command
	case Command::wait_start:
	case Command::wait_end:
		return true;

	// City supply Command
	case Command::city_supply_start:
	case Command::city_supply_end:
		return true;

	// Deployment Command
	case Command::deploy_start:
	case Command::deploy_end:
		return true;

	// Dispatch Command
	case Command::dispatch_start:
	case Command::dispatch_change:
	case Command::dispatch_cancel:
		return true;

	default:
		return false;
	}
}
