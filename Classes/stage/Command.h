#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "cocos2d.h"

class Entity;
class StageTile;

#define forCommand(counter, first, end) for(int counter=static_cast<int>(first);counter<static_cast<int>(end);counter++)
#define forUnit(counter) forCommand(counter, 0, UnitCommand::COUNT)
#define forCity(counter) forCommand(counter, 0, CityCommand::COUNT)
#define forMove(counter) forCommand(counter, 0, MoveCommand::COUNT)
#define forMove2(counter) forCommand(counter, MoveCommand::decision, MoveCommand::COUNT2)
#define castCommand(counter, command) static_cast<command>(counter)
#define castUnit(counter) castCommand(counter, UnitCommand)
#define castCity(counter) castCommand(counter, CityCommand)
#define castMove(counter) castCommand(counter, MoveCommand)

enum class UnitCommand
{
	supply, move, attack, occupation, spec, wait,
	COUNT
};

enum class CityCommand
{
	supply, deployment, dispatch,
	COUNT
};

enum class MoveCommand
{
	start, end,
	COUNT,
	decision, cancel,
	COUNT2
};

namespace command
{
	const std::string getName(UnitCommand com);
	const std::string getName(CityCommand com);
	const std::string getName(MoveCommand com);

	bool isEnable(UnitCommand com, Entity *unit, std::vector<StageTile*> tile);
	bool isEnable(CityCommand com, Entity *unit, std::vector<StageTile*> tile);
	bool isEnable(MoveCommand com, Entity *unit, std::vector<StageTile*> tile);
};


#endif // __COMMAND_H__
