#include "Entity.h"

#include "stage/Stage.h"
#include "util/Util.h"
#include "stage/Command.h"

USING_NS_CC;

/*
 * Initialize entity information
 */
EntityInformation::EntityInformation()
{
	auto lines = util::splitFile("entity/entity.csv");
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
				_unit_commands[static_cast<EntityType>(i - 1)][static_cast<UnitCommand>(j - 7)] = item == "TRUE";
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

	return unit;
}
