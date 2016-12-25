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

class Entity : public cocos2d::Ref
{
public:
	virtual bool init();
	CREATE_FUNC(Entity);
};

/*********************************************************/

class Infantry : public Entity
{

};

#endif // __ENTITY_H__
