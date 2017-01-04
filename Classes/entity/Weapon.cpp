#include "Weapon.h"
#include "util/Util.h"

WeaponInformation::WeaponInformation()
{
	auto lines = util::splitFile("info/weapon.csv");
	auto i = -1;
	for (auto line : lines)
	{
		if (++i == 0)
			continue;

		auto items = util::splitString(line, ',');
		auto j = 0;
		auto weapon = WeaponData::create();
		auto type = RangeType();
		for (auto item : items)
		{
			if (++j == 1)
				weapon->setName(item);
			else if (j == 2)
				weapon->setRecovery(std::atoi(item.c_str()));
			else if (j == 3)
				weapon->setAntiPersonnel(std::atoi(item.c_str()));
			else if (j == 4)
				weapon->setAntiWizard(std::atoi(item.c_str()));
			else if (j == 5)
				weapon->setAntiFire(std::atoi(item.c_str()));
			else if (j == 6)
				weapon->setAntiIce(std::atoi(item.c_str()));
			else if (j == 7)
				weapon->setAntiThunder(std::atoi(item.c_str()));
			else if (j == 8)
				weapon->setAntiGround(std::atoi(item.c_str()));
			else if (j == 9)
				type.directionRange = static_cast<DirectionRange>(std::atoi(item.c_str()));
			else if (j == 10)
				type.FiringRange = (std::atoi(item.c_str()));
			else if (j == 11)
				type.secondaryEffect = (std::atoi(item.c_str()));
			else if (j == 12)
				weapon->setAccuracy(std::atoi(item.c_str()));
			else if (j == 13)
				weapon->setConsumption(std::atoi(item.c_str()));
		}
		weapon->setRange(type);
		_weapons.insert(weapon->getName(), weapon);
	}
}

std::string WeaponInformation::getRangeName(RangeType type)
{
	switch (type.directionRange)
	{
	case DirectionRange::liner:
		return u8"直線";
	case DirectionRange::crescent:
		return u8"三方向";
	case DirectionRange::half:
		return u8"半円";
	case DirectionRange::overHalf:
		return u8"五方向";
	case DirectionRange::full:
		return u8"全方位";
	case DirectionRange::select:
		return u8"選択";
	}
}
