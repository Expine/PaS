#include "Entity.h"

#include "stage/Stage.h"

USING_NS_CC;

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