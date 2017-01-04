#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "cocos2d.h"

enum class DirectionRange
{
	liner, crescent, half, overHalf, full, select
};

typedef struct RangeType
{
	DirectionRange directionRange;
	int FiringRange;
	int secondaryEffect;
} RangeType;


/********************************************************************/
/*
 * Weapon data
 */
class WeaponData : public cocos2d::Ref
{
protected:
	WeaponData()
		: _antiPersonnel(0), _antiWizard(0)
		, _antiFire(0), _antiIce(0), _antiThunder(0), _antiGround(0)
		, _accuracy(0), _consumption(0)
	{};
	~WeaponData()
	{
		_antiPersonnel = _antiWizard = 0;
		_antiFire = _antiIce = _antiThunder = _antiGround = 0;
		_accuracy = _consumption = 0;
	};
	virtual bool init()
	{
		return true;
	};
public:
	CREATE_FUNC(WeaponData);
	CC_SYNTHESIZE(std::string, _name, Name);
	CC_SYNTHESIZE(int, _recovery, Recovery);
	CC_SYNTHESIZE(int, _antiPersonnel, AntiPersonnel);
	CC_SYNTHESIZE(int, _antiWizard, AntiWizard);
	CC_SYNTHESIZE(int, _antiFire, AntiFire);
	CC_SYNTHESIZE(int, _antiIce, AntiIce);
	CC_SYNTHESIZE(int, _antiThunder, AntiThunder);
	CC_SYNTHESIZE(int, _antiGround, AntiGround);
	CC_SYNTHESIZE(RangeType, _range, Range);
	CC_SYNTHESIZE(int, _accuracy, Accuracy);
	CC_SYNTHESIZE(int, _consumption, Consumption);
};

/********************************************************************/

class WeaponInformation
{
private:
	cocos2d::Map<std::string, WeaponData*> _weapons;
protected:
	WeaponInformation();
public:
	static WeaponInformation* getInstance()
	{
		static WeaponInformation info;
		return &info;
	};
	WeaponData* getWeapon(const std::string &name) { return _weapons.at(name); };
	std::string getRangeName(RangeType type);
};


#endif // __WEAPON_H__