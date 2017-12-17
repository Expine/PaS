#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cocos2d.h"

class Stage;
class WeaponData;
class UnitLayer;
class City;
enum class Command;
enum class Owner;

/** Max value of duability for calculating force power*/
constexpr int UNIT_MAX_DURABILITY = 10000;
/** Max value of material for calculating force power*/
constexpr int UNIT_MAX_MATERIAL = 10000;
/** Max value of mobility for calculating force power*/
constexpr int UNIT_MAX_MOBILITY = 100;
/** Max value of searching ability for calculating force power*/
constexpr int UNIT_MAX_SEARCHING = 10;
/** Max value of defence for calculating force power*/
constexpr int UNIT_MAX_DEFENCE = 100;

/** When unit is deploied, add tag to it*/
constexpr int DEPLOY_CONST = 100000000;

/*********************************************************/

/** Military department */
enum class Department
{
	soldier, wizard
};

/** Entity type */
enum class EntityType
{
	infantry, heavy, spy,
	fire, ice, thunder, ground, king, weapon, relief, guardian, dark, light,
	COUNT,
	sight, counter
};

/** Unit state*/
enum class EntityState
{
	none, supplied, moved, acted,
};

/*********************************************************/

/*
 * Entity base data
 */
class EntityInformation
{
private:
	std::map<EntityType, std::string> _name;
	std::map<EntityType, int> _durability;
	std::map<EntityType, int> _material;
	std::map<EntityType, int> _mobility;
	std::map<EntityType, int> _search;
	std::map<EntityType, int> _defence;
	std::map<EntityType, std::map<Command, bool>> _unit_commands;
protected:
	EntityInformation();
public:
	/** Get instance for singleton*/
	static EntityInformation* getInstance()
	{
		static EntityInformation info;
		return &info;
	};
	/** Get base name*/
	inline const std::string& getName(EntityType type) { return _name[type]; };
	/** Get base durability*/
	inline int getDurability(EntityType type) { return _durability[type]; };
	/** Get base material*/
	inline int getMaterial(EntityType type) { return _material[type]; };
	/** Get base mobility*/
	inline int getMobility(EntityType type) { return _mobility[type]; };
	/** Get base search*/
	inline int getSearch(EntityType type) { return _search[type]; };
	/** Get base defence*/
	inline int getDefence(EntityType type) { return _defence[type]; };
	/** Get whether command can be used. Available by default.*/
	inline bool getCommand(EntityType type, Command command) { return _unit_commands[type].count(command) == 0 || _unit_commands[type][command]; };
};

/*********************************************************/

/*
 * Unit on stage and sprite.
 */
class Entity : public cocos2d::Sprite
{
protected:
	std::vector<WeaponData*> _weapons;
	Entity();
	virtual ~Entity();
public:
	CREATE_FUNC(Entity);
	CC_SYNTHESIZE(Owner, _affiliation, Affiliation);
	CC_SYNTHESIZE_READONLY(EntityState, _state, State);
	CC_SYNTHESIZE(Department, _department, Department);
	CC_SYNTHESIZE(EntityType, _type, Type);
	CC_SYNTHESIZE(std::string, _name, EntityName);
	CC_SYNTHESIZE(int, _durability, Durability);
	CC_SYNTHESIZE(int, _max_durability, MaxDurability);
	CC_SYNTHESIZE(int, _material, Material);
	CC_SYNTHESIZE(int, _max_material, MaxMaterial);
	CC_SYNTHESIZE(int, _mobility, Mobility);
	CC_SYNTHESIZE(int, _searchingAbility, SearchingAbility);
	CC_SYNTHESIZE(int, _defence, Defence);
	CC_SYNTHESIZE(int, _using_weapon, UsingWeapon);
	CC_SYNTHESIZE(bool, _reserved, Reserved);
	static Entity* create(const EntityType type);
	static Entity* create(const EntityType type, const cocos2d::Vec2 cor, Stage* stage);
	void setBasicWeaponForDebug();

	Stage* getStage();

	/** Get vector of weapons by reference*/
	inline std::vector<WeaponData*>& getWeaponsByRef() { return _weapons; };

	cocos2d::Vec2 getPositionAsTile();
	bool isAttakable();
	bool isOcuppyable();
	/** Check whether this unit can be selected*/
	inline bool isSelectable() { return getOpacity() != 0 && isVisible(); };
	bool isSelectableEnemy(Owner owner);
	inline bool isDeployer() { return getTag() > DEPLOY_CONST; };

	void attack(Entity* enemy, WeaponData* weapon);
	float correctDamageByDepartment(Entity* enemy, WeaponData* weapon);
	float correctDamageByType(Entity* enemy, WeaponData* weapon);
	float correctDamageByHit(Entity* enemy, WeaponData* weapon, const float damage);
	void damaged(float damage);
	void occupy(City* city);

	void setState(EntityState state);
};

/*********************************************************/

class Unit : public Entity
{

};

/*********************************************************/

class Soldier : public Entity
{
protected:
	virtual bool init()
	{
		if (!Entity::init())
			return false;

		_department = Department::soldier;
		return true;
	}
public:
	CREATE_FUNC(Soldier);
	CC_SYNTHESIZE(int, _occupation_ability, OccupationAbility);
};

class Wizard : public Entity
{
protected:
	virtual bool init()
	{
		if (!Entity::init())
			return false;

		_department = Department::wizard;
		return true;
	}
public:
	CREATE_FUNC(Wizard);
};

/*********************************************************/

class Infantry : public Soldier
{
protected:
	virtual bool init()
	{
		if (!Soldier::init())
			return false;
		return true;
	}
public:
	CREATE_FUNC(Infantry);
};

class Heavy : public Soldier
{
protected:
	virtual bool init()
	{
		if (!Soldier::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Heavy);
};

class Spy : public Soldier
{
protected:
	virtual bool init()
	{
		if (!Soldier::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Spy);
};
/*********************************************************/

class Fire : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Fire);
};

class Ice : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Ice);
};

class Thunder : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Thunder);
};

class Ground : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Ground);
};

class King : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(King);
};

class Weapon : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Weapon);
};

class Relief : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Relief);
};

class Guardian : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Guardian);
};

class Dark : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Dark);
};

class Light : public Wizard
{
protected:
	virtual bool init()
	{
		if (!Wizard::init())
			return false;

		return true;
	}
public:
	CREATE_FUNC(Light);
};
#endif // __ENTITY_H__
