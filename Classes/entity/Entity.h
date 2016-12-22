#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cocos2d.h"

class Entity : public cocos2d::Ref
{
public:
	virtual bool init();
	CREATE_FUNC(Entity);
};

#endif // __ENTITY_H__
