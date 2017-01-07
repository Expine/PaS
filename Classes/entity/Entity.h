#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cocos2d.h"

#include "ai/Owner.h"

class Stage;
class WeaponData;
class UnitLayer;
enum class Command;


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
	none, supplied, moved, acted
};

/*********************************************************/

/*
 * Entity base data
 */
class EntityInformation
{
private:
	std::map<EntityType, std::string> _name;
	std::map<EntityType, int> _mobility;
	std::map<EntityType, int> _material;
	std::map<EntityType, int> _search;
	std::map<EntityType, int> _defence;
	std::map<EntityType, int> _durability;
	std::map<EntityType, std::map<Command, bool>> _unit_commands;
protected:
	EntityInformation();
public:
	static EntityInformation* getInstance()
	{
		static EntityInformation info;
		return &info;
	};
	inline const std::string& getName(EntityType type) { return _name[type]; };
	inline bool getCommand(EntityType type, Command command) { return _unit_commands[type].count(command) == 0 || _unit_commands[type][command]; };
	inline int getMobility(EntityType type) { return _mobility[type]; };
	inline int getMaterial(EntityType type) { return _material[type]; };
	inline int getSearch(EntityType type) { return _search[type]; };
	inline int getDefence(EntityType type) { return _defence[type]; };
	inline int getDurability(EntityType type) { return _durability[type]; };
};

/*********************************************************/

/*
 * Unit on stage and sprite.
 */
class Entity : public cocos2d::Sprite
{
protected:
	std::vector<WeaponData*> _weapons;
	Entity()
		: _department(Department::soldier), _type(EntityType::infantry), _state(EntityState::none)
		, _usingWeapon(0), _mobility(0), _material(0), _maxMaterial(0), _searchingAbility(0), _defence(0), _durability(0), _maxDurability(0)
	{};
	~Entity()
	{
		_usingWeapon = _mobility = _material = _maxMaterial = _searchingAbility = _defence = _durability = _maxDurability = 0;
	};
public:
	CREATE_FUNC(Entity);
	CC_SYNTHESIZE(Owner, _affiliation, Affiliation);
	inline Entity* setAffiliationRetThis(Owner owner) { _affiliation = owner; return this; };
	CC_SYNTHESIZE(EntityState, _state, State);
	CC_SYNTHESIZE(EntityType, _type, Type);
	CC_SYNTHESIZE(Department, _department, Department);
	CC_SYNTHESIZE(std::string, _name, EntityName);
	CC_SYNTHESIZE(int, _usingWeapon, UsingWeapon);
	CC_SYNTHESIZE(int, _mobility, Mobility);
	CC_SYNTHESIZE(int, _material, Material);
	CC_SYNTHESIZE(int, _maxMaterial, MaxMaterial);
	CC_SYNTHESIZE(int, _searchingAbility, SearchingAbility);
	CC_SYNTHESIZE(int, _defence, Defence);
	CC_SYNTHESIZE(int, _durability, Durability);
	CC_SYNTHESIZE(int, _maxDurability, MaxDurability);
	static Entity* create(EntityType type, const int x, const int y, cocos2d::SpriteBatchNode* batch, Stage* stage);
	UnitLayer* getUnitLayer();
	Stage* getStage();
	inline cocos2d::Vec2 getTileCoordinate(int mapy) { return cocos2d::Vec2(getTag() / mapy, getTag() % mapy); };
	inline std::vector<WeaponData*>& getWeaponsByRef() { return _weapons; };
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
