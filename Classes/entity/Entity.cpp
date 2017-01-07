#include "Entity.h"
#include "Weapon.h"

#include "stage/Stage.h"
#include "util/Util.h"
#include "stage/Command.h"

USING_NS_CC;

/*
 * Initialize entity information
 */
EntityInformation::EntityInformation()
{
	auto lines = util::splitFile("info/entity.csv");
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
			switch (j)
			{
			case 1:	_name[static_cast<EntityType>(i - 1)] = item;					break;
			case 2: _mobility[static_cast<EntityType>(i - 1)] = atoi(item.c_str());	break;
			case 3: _material[static_cast<EntityType>(i - 1)] = atoi(item.c_str());	break;
			case 4: _search[static_cast<EntityType>(i - 1)] = atoi(item.c_str());	break;
			case 5: _defence[static_cast<EntityType>(i - 1)] = atoi(item.c_str());	break;
			case 6: _durability[static_cast<EntityType>(i - 1)] = atoi(item.c_str());	break;
			case 7: case 8: case 9: case 10: case 11: case 12:
				_unit_commands[static_cast<EntityType>(i - 1)][static_cast<Command>(j - 7 + static_cast<int>(Command::UNIT_START) + 1)] = item == "TRUE";
			}
		}
	}
}

/*
 * Create entity instance.
 * And set image.
 */
Entity* Entity::create(EntityType type, const int x, const int y, SpriteBatchNode * batch, Stage * stage)
{
	Entity* unit;
	switch (type)
	{
	case EntityType::infantry:	unit = Infantry::create();	break;
	case EntityType::heavy:		unit = Heavy::create();		break;
	case EntityType::spy:		unit = Spy::create();		break;
	case EntityType::fire:		unit = Fire::create();		break;
	case EntityType::ice:		unit = Ice::create();		break;
	case EntityType::thunder:	unit = Thunder::create();	break;
	case EntityType::ground:	unit = Ground::create();	break;
	case EntityType::king:		unit = King::create();		break;
	case EntityType::weapon:	unit = Weapon::create();	break;
	case EntityType::relief:	unit = Relief::create();	break;
	case EntityType::guardian:	unit = Guardian::create();	break;
	case EntityType::dark:		unit = Dark::create();		break;
	case EntityType::light:		unit = Light::create();		break;
	default:	unit = Infantry::create();
	}

	// Set base information
	auto info = EntityInformation::getInstance();
	unit->setType(type);
	unit->setName(info->getName(type));
	unit->setMobility(info->getMobility(type));
	unit->setMaterial(info->getMaterial(type));
	unit->setMaxMaterial(info->getMaterial(type));
	unit->setSearchingAbility(info->getSearch(type));
	unit->setDefence(info->getDefence(type));
	unit->setDurability(info->getDurability(type));
	unit->setMaxDurability(info->getDurability(type));

	//Set batch
	auto unitSize = Vec2(32, 32);
	unit->initWithTexture(batch->getTexture(), Rect(0, static_cast<int>(type) * unitSize.y, unitSize.x, unitSize.y));
	unit->setPosition(stage->getCoordinateByTile(x, y) + Vec2(stage->getChipSize().x /2, 0));
	unit->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	unit->setScale(0.5f);
	unit->setTag(x * stage->getMapSize().y + y);
	//Set unlock antialias
	unit->getTexture()->setAliasTexParameters();

	//For debug
	unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"空拳"));
	if (false)
		return unit;
	else if (type == EntityType::infantry)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"銃剣"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"短銃"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"小銃"));
	}
	else if (type == EntityType::heavy)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"銃剣"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"短銃"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"小銃"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"機関銃"));
	}
	else if (type == EntityType::spy)
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"短銃"));
	else if (type == EntityType::fire)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"火炎"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"延焼"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"炎砲"));
	}
	else if (type == EntityType::ice)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"氷結"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"吹雪"));
	}
	else if (type == EntityType::thunder)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"迅雷"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"神鳴"));
	}
	else if (type == EntityType::ground)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"地震"));
	}
	else if (type == EntityType::king)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"獅子炎"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"王臨"));
	}
	else if (type == EntityType::weapon)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"双剣"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"鎖鎌"));
	}
	else if (type == EntityType::relief)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"回復"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"慈雨"));
	}
	else if (type == EntityType::guardian)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"樹奏"));
	}
	else if (type == EntityType::dark)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"闇討"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"漆黒"));
	}
	else if (type == EntityType::light)
	{
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"回復"));
		unit->_weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"天光"));
	}

	return unit;
}

UnitLayer * Entity::getUnitLayer()
{
	return util::instance<UnitLayer>(getParent()->getParent());
}

Stage * Entity::getStage()
{
	return util::instance<Stage>(getUnitLayer()->getParent());
}

bool Entity::isAttakable()
{
	for (auto weapon : _weapons)
		if (weapon->isUsable(this))
			return true;
	return false;
}
