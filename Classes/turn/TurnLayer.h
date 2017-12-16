#ifndef __TURN_LAYER__
#define __TURN_LAYER__

#include "cocos2d.h"

/*
 * Simulation turn layer
 * Should implement init and next
 */
class ITurnLayer : public cocos2d::Layer 
{
protected:
	std::function<void()> _nextTurn;
public:
	virtual void initTurn() = 0;
	inline void setNextTurn(std::function<void()> nextTurn) { _nextTurn = nextTurn; };
};

#endif // __TURN_LAYER__