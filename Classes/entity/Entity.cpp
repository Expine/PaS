#include "Entity.h"
#include "Weapon.h"

#include "stage/Stage.h"
#include "stage/Tile.h"
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
 * Constructor
 */
Entity::Entity()
	: _department(Department::soldier), _type(EntityType::infantry), _state(EntityState::none)
	, _using_weapon(0), _mobility(0), _material(0), _max_material(0), _searchingAbility(0), _defence(0), _durability(0), _max_durability(0)
	, _reserved(false)
{
}

/*
 * Destructor
 */
Entity::~Entity()
{
	_using_weapon = _mobility = _material = _max_material = _searchingAbility = _defence = _durability = _max_durability = 0;
}

/*
 * Create entity instance.
 * And set base information.
 */
 Entity * Entity::create(const EntityType type)
 {
	 // Set unit instance
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

	 return unit;
 }

 /*
 * Create entity instance.
 * And set base information.
 * And set image, place by pos.
 */
 Entity* Entity::create(const EntityType type, const Vec2 cor, Stage * stage)
{
	 // Create instance and set base information
	 auto unit = create(type);

	//Set batch
	auto unitSize = Vec2(32, 32);
	unit->initWithTexture(stage->getUnitBatch()->getTexture(), Rect(0, static_cast<int>(type) * unitSize.y, unitSize.x, unitSize.y));
	unit->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	unit->setScale(0.5f);
	stage->moveUnitPositionAsTile(cor, unit);
	//Set unlock antialias
	unit->getTexture()->setAliasTexParameters();

	//For debug
	unit->setBasicWeaponForDebug();

	return unit;
}

 /*
  * Set basic weapon
  * This is the function for debug
  */
 void Entity::setBasicWeaponForDebug()
 {
	 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"空拳"));
	 if (false)
		 return;
	 else if (_type == EntityType::infantry)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"銃剣"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"短銃"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"小銃"));
	 }
	 else if (_type == EntityType::heavy)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"銃剣"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"短銃"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"小銃"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"機関銃"));
	 }
	 else if (_type == EntityType::spy)
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"短銃"));
	 else if (_type == EntityType::fire)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"火炎"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"延焼"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"炎砲"));
	 }
	 else if (_type == EntityType::ice)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"氷結"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"吹雪"));
	 }
	 else if (_type == EntityType::thunder)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"迅雷"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"神鳴"));
	 }
	 else if (_type == EntityType::ground)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"地震"));
	 }
	 else if (_type == EntityType::king)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"獅子炎"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"王臨"));
	 }
	 else if (_type == EntityType::weapon)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"双剣"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"鎖鎌"));
	 }
	 else if (_type == EntityType::relief)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"回復"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"慈雨"));
	 }
	 else if (_type == EntityType::guardian)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"樹奏"));
	 }
	 else if (_type == EntityType::dark)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"闇討"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"漆黒"));
	 }
	 else if (_type == EntityType::light)
	 {
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"回復"));
		 _weapons.push_back(WeaponInformation::getInstance()->getWeapon(u8"天光"));
	 }
 }

/*
 * Get stage
 * However, This function is impossible if this unit is not placed on the screen.
 */
Stage * Entity::getStage()
{
	return util::instance<Stage>(getParent()->getParent()->getParent());
}

/*
 * Get coordinate as tile
 */
Vec2 Entity::getPositionAsTile()
{
	int mapy = getStage()->getMapSize().y;
	int tag = getTag() % DEPLOY_CONST;
	return Vec2(tag / mapy, tag % mapy);
}

/*
 * Check whether this unit can attack by any weapon
 */
bool Entity::isAttakable()
{
	for (auto weapon : _weapons)
		if (weapon->isUsable(this))
			return true;
	return false;
}

/*
 * Check whether this unit can occupy
 */
bool Entity::isOcuppyable()
{
	auto city = util::findElement<StageTile*>(getStage()->getTiles(getPositionAsTile()), [](StageTile* tile){return util::instanceof<City>(tile);});
	return city && util::instance<City>(city)->getOwner() != getAffiliation();
}

/** Check whether this unit can be selected and it is enemy*/
bool Entity::isSelectableEnemy(Owner owner)
{
	return isSelectable() && !OwnerInformation::getInstance()->isSameGroup(owner, _affiliation);
}

/*
 * Attack enemy by weapon
 */
void Entity::attack(Entity * enemy, WeaponData* weapon)
{
	float damage = 0.0f;

	// Correct damage by department and type
	damage += correctDamageByDepartment(enemy, weapon);
	damage += correctDamageByType(enemy, weapon);

	// Decrease damage by defence
	damage -= enemy->getDefence() / 10;
	if (damage < 0)
		damage = 0.0f;

	// Check hit and damage adjustment
	damage = correctDamageByHit(enemy, weapon, damage);

	// Damage to enemy
	enemy->damaged(damage);

	// Decrease material
	_material -= weapon->getConsumption();

	// Animation
	getStage()->addChild(weapon->animation(enemy->getPosition()));
}

/*
 * Correct damage by enemy department
 */
float Entity::correctDamageByDepartment(Entity * enemy, WeaponData * weapon)
{
	switch (enemy->getDepartment())
	{
	case Department::soldier:	return weapon->getAntiPersonnel();
	case Department::wizard:	return weapon->getAntiWizard();
	default:					return 0.0f;
	}
}

float Entity::correctDamageByType(Entity * enemy, WeaponData * weapon)
{
	switch (enemy->getType())
	{
	case EntityType::fire:
	case EntityType::king:
		return weapon->getAntiFire();
	case EntityType::ice:
	case EntityType::dark:
		return weapon->getAntiIce();
	case EntityType::thunder:
	case EntityType::light:
		return weapon->getAntiThunder();
	case EntityType::ground:
	case EntityType::guardian:
		return weapon->getAntiGround();
	case EntityType::relief:
	default:
		return 0.0f;
	}
}

/*
 * Correct damage by hit
 */
float Entity::correctDamageByHit(Entity * enemy, WeaponData * weapon, const float damage)
{
	// Calculate damage unit
	const int damage_split = 10;
	auto damageUnit = damage / damage_split;

	// Correct damage by hit
	auto result = 0.0f;
	util::initRand();
	for (int i = 0; i < damage_split; i++)
		if (util::getRand(0, 99) < weapon->getAccuracy())
			result += damageUnit;

	return result;
}

/*
 * Suffer from damage
 */
void Entity::damaged(float damage)
{
	auto damaged_durability = getDurability() - damage;
	if (damaged_durability > 0)
		setDurability(damaged_durability);
	else
		getStage()->removeUnit(this);
}

/*
 * Occupy the city
 */
void Entity::occupy(City * city)
{
	city->damaged(util::instance<Soldier>(this));

	auto particle = ParticleSystemQuad::create("particle/ice.plist");
	particle->setAutoRemoveOnFinish(true);
	particle->resetSystem();
	particle->setPosition(city->getPosition());
	particle->setScale(0.2f);
	getStage()->addChild(particle);
}

/*
 * Set entity state
 * If acted, put E on image
 * If none, remove E on image
 */
void Entity::setState(EntityState state)
{
	_state = state;
	if (state == EntityState::acted)
	{
		auto acted = Sprite::createWithTexture(getTexture(), Rect(0, getTexture()->getPixelsHigh() - 32, 32, 32));
		acted->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		this->addChild(acted);
	}
	else if (state == EntityState::none)
	{
		this->removeAllChildren();
	}
}

