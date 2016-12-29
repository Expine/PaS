#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "cocos2d.h"

class Command : public cocos2d::Node
{
protected:
	virtual bool init();
public:
	CREATE_FUNC(Command);
};

#endif // __COMMAND_H__
