#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cocos2d.h"
/*********************************************************/

enum class Department
{
	soldier,
	wizard
};

enum class EntityType
{
	infantry,
	heavy,
	spy,
	
	fire,
	ice,
	thunder,
	ground,
	king,
	weapon,
	relief,
	guardian,
	dark,
	light,

	COUNT
};

/*********************************************************/

class Entity : public cocos2d::Sprite
{
protected:
	Entity()
		: _department(Department::soldier)
		, _type(EntityType::infantry)
	{

	};
	virtual bool init();
public:
	CC_SYNTHESIZE(Department, _department, Department);
	CC_SYNTHESIZE(EntityType, _type, Type);
	CREATE_FUNC(Entity);
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

};

class Heavy : public Soldier
{

};

class Spy : public Soldier
{

};
/*********************************************************/

class Fire : public Wizard
{

};

class Ice : public Wizard
{

};

class Thunder : public Wizard
{

};

class Ground : public Wizard
{

};

class King : public Wizard
{

};

class Weapon : public Wizard
{

};

class Relief : public Wizard
{

};

class Guardian : public Wizard
{

};

class Dark : public Wizard
{

};

class Light : public Wizard
{

};
#endif // __ENTITY_H__
