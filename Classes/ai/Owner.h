#ifndef __OWNER_H__
#define __OWNER_H__

#include "cocos2d.h"

enum class Owner
{
	player, enemy
};

class OwnerInformation
{
private:
	std::map<Owner, cocos2d::Color3B> _colors;
protected:
	OwnerInformation()
	{
		_colors[Owner::player] = cocos2d::Color3B(0, 0, 255);
		_colors[Owner::enemy] = cocos2d::Color3B(255, 0, 0);
	};
public:
	static OwnerInformation* getInstance()
	{
		static OwnerInformation owner;
		return &owner;
	};
	cocos2d::Color3B getColor(Owner owner) { return _colors[owner]; };
};

#endif // __OWNER_H__
