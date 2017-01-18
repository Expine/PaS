#include "Weapon.h"
#include "Entity.h"

#include "ai/Owner.h"
#include "stage/Stage.h"
#include "stage/Tile.h"
#include "util/Util.h"

/*
 * Constructor of weapon information
 * Parse weapon information
 */
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
				type.direction_type = static_cast<DirectionType>(std::atoi(item.c_str()));
			else if (j == 10)
				type.firing_range = (std::atoi(item.c_str()));
			else if (j == 11)
				type.secondary_effect = (std::atoi(item.c_str()));
			else if (j == 12)
				weapon->setAccuracy(std::atoi(item.c_str()));
			else if (j == 13)
				weapon->setConsumption(std::atoi(item.c_str()));
		}
		weapon->setRange(type);
		_weapons.insert(weapon->getName(), weapon);
	}
}

/*
 * Get range name
 */
std::string WeaponInformation::getRangeName(RangeType type)
{
	switch (type.direction_type)
	{
	case DirectionType::liner:		return u8"直線";
	case DirectionType::crescent:	return u8"三方向";
	case DirectionType::overHalf:	return u8"五方向";
	case DirectionType::full:		return u8"全方位";
	case DirectionType::select:		return u8"選択";
	default:						return "ERROR";
	}
}

/*
 * Constructor of weapon data
 */
WeaponData::WeaponData()
	: _anti_personnel(0), _anti_wizard(0)
	, _anti_fire(0), _anti_ice(0), _anti_thunder(0), _anti_ground(0)
	, _accuracy(0), _consumption(0)
{}

/*
 * Destructor of weapon data
 */
WeaponData::~WeaponData()
{
	_anti_personnel = _anti_wizard = _anti_fire = _anti_ice = _anti_thunder = _anti_ground = 0;
	_accuracy = _consumption = 0;
}

/*
 * Check whether this is enable to use
 */
bool WeaponData::isUsable(Entity * unit)
{
	return isUsableByConsumption(unit) && isUsableByArea(unit);
}

/*
 * Check whether this is enable to use by consumption
 */
bool WeaponData::isUsableByConsumption(Entity * unit)
{
	return unit->getMaterial() > _consumption;
}

/*
 * Check whether this is enable to use by searching enemy in area
 */
bool WeaponData::isUsableByArea(Entity * unit)
{
	auto stage = unit->getStage();
	// Get tiles in area
	std::vector <StageTile*> tiles = _range.direction_type == DirectionType::liner ?
			stage->startRecursiveTileSearchForLiner(unit->getPositionAsTile(), _range.firing_range)
		:	stage->startRecursiveTileSearch(unit->getPositionAsTile(), _range.firing_range, EntityType::counter);
	for (auto tile : tiles)
	{
		auto target = stage->getUnit(tile->getPositionAsTile());
		if (target && target->isSelectableEnemy(unit->getAffiliation()))
			return true;
	}
	return false;
}

// TODO unimplement method
/*
 * Get animation
 */
ParticleSystemQuad* WeaponData::animation(cocos2d::Vec2 pos)
{
	auto particle = ParticleSystemQuad::create("particle/ice.plist");
	particle->setAutoRemoveOnFinish(true);
	particle->resetSystem();
	particle->setPosition(pos);
	particle->setScale(0.2f);
	return particle;
}
