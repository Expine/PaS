#ifndef __WEAPON_H__
#define __WEAPON_H__

#include "cocos2d.h"

class Entity;

constexpr int WEAPON_MAX_ATTACK = 1000;

/** Type of direction */
enum class DirectionType
{
	liner, crescent, overHalf, full, select
};

/** Type of range */
typedef struct RangeType
{
	DirectionType direction_type;
	int firing_range;
	int secondary_effect;
} RangeType;


/********************************************************************/
/*
 * Weapon data
 */
class WeaponData : public cocos2d::Ref
{
protected:
	WeaponData();
	virtual ~WeaponData();
	virtual bool init() { return true; };
public:
	CREATE_FUNC(WeaponData);
	CC_SYNTHESIZE(std::string, _name, Name);
	CC_SYNTHESIZE(int, _recovery, Recovery);
	CC_SYNTHESIZE(int, _anti_personnel, AntiPersonnel);
	CC_SYNTHESIZE(int, _anti_wizard, AntiWizard);
	CC_SYNTHESIZE(int, _anti_fire, AntiFire);
	CC_SYNTHESIZE(int, _anti_ice, AntiIce);
	CC_SYNTHESIZE(int, _anti_thunder, AntiThunder);
	CC_SYNTHESIZE(int, _anti_ground, AntiGround);
	CC_SYNTHESIZE(RangeType, _range, Range);
	CC_SYNTHESIZE(int, _accuracy, Accuracy);
	CC_SYNTHESIZE(int, _consumption, Consumption);

	bool isUsable(Entity* unit);
	bool isUsableByConsumption(Entity* unit);
	bool isUsableByArea(Entity* unit);
	cocos2d::ParticleSystemQuad* animation(cocos2d::Vec2 pos);
	/** Get max attack performance*/
	inline int getMaxAttack()
	{
		return	(_anti_personnel > _anti_wizard) ? _anti_personnel : _anti_wizard
			+	(_anti_fire > _anti_ice && _anti_fire > _anti_thunder && _anti_fire > _anti_ground) ? _anti_fire
			:	(_anti_ice > _anti_thunder && _anti_ice > _anti_ground) ? _anti_ice
			:	(_anti_thunder > _anti_ground) ? _anti_thunder
			:	_anti_ground;
	}
};

/********************************************************************/

/*
 * Weapon base data
 */
class WeaponInformation
{
private:
	cocos2d::Map<std::string, WeaponData*> _weapons;
protected:
	WeaponInformation();
public:
	/** Get instance for singleton*/
	static WeaponInformation* getInstance()
	{
		static WeaponInformation info;
		return &info;
	};
	/** Get weapon by name*/
	WeaponData* getWeapon(const std::string &name) { return _weapons.at(name); };
	std::string getRangeName(RangeType type);
};


#endif // __WEAPON_H__