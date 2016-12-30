#include "Command.h"

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
	case MoveCommand::cancel: return u8"�P��";
	case MoveCommand::end: return u8"���~";
	default: return "ERROR";
	}
}
