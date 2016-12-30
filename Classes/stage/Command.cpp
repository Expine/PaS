#include "Command.h"

const std::string command::getName(UnitCommand com)
{
	switch (com)
	{
	case UnitCommand::supply: return u8"•â‹‹";
	case UnitCommand::move: return u8"ˆÚ“®";
	case UnitCommand::attack: return u8"UŒ‚";
	case UnitCommand::occupation: return u8"è—Ì";
	case UnitCommand::spec: return u8"«”\";
	case UnitCommand::wait: return u8"‘Ò‹@";
	default: return "ERROR";
	}
};
const std::string command::getName(CityCommand com)
{
	switch (com)
	{
	case CityCommand::supply: return u8"•â‹‹";
	case CityCommand::deployment: return u8"”z”õ";
	case CityCommand::dispatch: return u8"”hŒ­";
	default: return "ERROR";
	}
}
const std::string command::getName(MoveCommand com)
{
	switch (com)
	{
	case MoveCommand::start: return u8"Šm’è";
	case MoveCommand::cancel: return u8"“P‰ñ";
	case MoveCommand::end: return u8"’†~";
	default: return "ERROR";
	}
}
