#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "cocos2d.h"

#define forCommand(counter, command) for(int counter=0;counter<static_cast<int>(command::COUNT);counter++)
#define forUnit(counter) forCommand(counter, UnitCommand)
#define forCity(counter) forCommand(counter, CityCommand)
#define forMove(counter) forCommand(counter, MoveCommand)
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
	COUNT
};

namespace command
{
	const std::string getName(UnitCommand com);
	const std::string getName(CityCommand com);
	const std::string getName(MoveCommand com);
};


#endif // __COMMAND_H__
