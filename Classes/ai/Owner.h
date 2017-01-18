#ifndef __OWNER_H__
#define __OWNER_H__

#include "cocos2d.h"

/*
 * Affiliation force
 */
enum class Owner
{
	player, enemy
};

/*
 * Basic data of affiliation force
 */
class OwnerInformation
{
private:
	std::map<Owner, std::string> _group;
	std::map<Owner, cocos2d::Color3B> _colors;
protected:
	/** Initialize information */
	OwnerInformation()
	{
		// Set color
		_colors[Owner::player] = cocos2d::Color3B(0, 0, 255);
		_colors[Owner::enemy] = cocos2d::Color3B(255, 0, 0);

		// Set group
		_group[Owner::player] = u8"友軍";
		_group[Owner::enemy] = u8"敵軍";
	};
public:
	/** Get instance for singleton*/
	static OwnerInformation* getInstance()
	{
		static OwnerInformation owner;
		return &owner;
	};
	/** Get color of affiliation */
	cocos2d::Color3B getColor(Owner owner) { return _colors[owner]; };
	/** Set group of affiliation */
	void setGroup(Owner owner, const std::string name) { _group[owner] = name; };
	/** Get group of affiliation */
	std::string getGroup(Owner owner) { return _group[owner]; };
	/** Whether the two belong to the same affiliation */
	bool isSameGroup(Owner main, Owner sub) { return _group[main] == _group[sub]; };
};

#endif // __OWNER_H__
