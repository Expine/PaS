#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cocos2d.h"

class Stage;
class Weapon;

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
	COUNT
};

/** Unit state*/
enum class EntityState
{
	none, supplied, moved, end
};
/*********************************************************/

/*
 * Unit on stage and sprite.
 */
class Entity : public cocos2d::Sprite
{
protected:
	cocos2d::Vector<Weapon*> weapons;
	Entity()
		: _department(Department::soldier), _type(EntityType::infantry), _explanation(""), _state(EntityState::none)
		, _usingWeapon(0), _mobility(0), _material(0), _maxMaterial(0), _searchingAbility(0), _defence(0)
	{};
	~Entity()
	{
		_usingWeapon = _mobility = _material = _maxMaterial = _searchingAbility = _defence = 0;
	};
public:
	CREATE_FUNC(Entity);
	CC_SYNTHESIZE(EntityState, _state, State);
	CC_SYNTHESIZE(int, _usingWeapon, UsingWeapon);
	CC_SYNTHESIZE(int, _material, Material);
	CC_SYNTHESIZE_READONLY(Department, _department, Department);
	CC_SYNTHESIZE_READONLY(EntityType, _type, Type);
	CC_SYNTHESIZE_READONLY(std::string, _explanation, Explanation);
	CC_SYNTHESIZE_READONLY(int, _mobility, Mobility);
	CC_SYNTHESIZE_READONLY(int, _maxMaterial, MaxMaterial);
	CC_SYNTHESIZE_READONLY(int, _searchingAbility, SearchingAbility);
	CC_SYNTHESIZE_READONLY(int, _defence, Defence);
	static Entity* create(EntityType type, const int x, const int y, cocos2d::SpriteBatchNode* batch, Stage* stage);
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

		_type = EntityType::infantry;
		_explanation = u8"一般人による兵隊。\n都市の占領、統治が可能。";
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

		_type = EntityType::heavy;
		_explanation = u8"訓練された兵隊。\n都市の占領、統治が可能。\nそれなりに戦える。";
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

		_type = EntityType::spy;
		_explanation = u8"諜報部隊。\n少数のため移動力が高い。";
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

		_type = EntityType::fire;
		_explanation = u8"炎峰一族の魔法使い。\n一対多に長ける。\n森林戦が得意。";
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

		_type = EntityType::ice;
		_explanation = u8"雹牢一族の魔法使い。\n一対一に長ける。\n水を凍らせて渡れる。";
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

		_type = EntityType::thunder;
		_explanation = u8"雷翔一族の魔法使い。\n移動力が極めて高い。\n人工物上での戦闘に強い。";
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

		_type = EntityType::ground;
		_explanation = u8"大地一族の魔法使い。\n山脈をやすやすと上る。\n山での戦闘に長ける。";
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

		_type = EntityType::king;
		_explanation = u8"王藤一族の魔法使い。\n燃費は悪いが強力。\n場所を選ばず戦える。";
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

		_type = EntityType::weapon;
		_explanation = u8"双霊一族の魔法使い。\n一戦闘ごとに補給必須。\n都市戦を得意とする。";
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

		_type = EntityType::relief;
		_explanation = u8"愛貴一族の魔法使い。\n後方支援に長ける。\n戦闘能力は低い。";
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

		_type = EntityType::guardian;
		_explanation = u8"樹奏一族の魔法使い。\n防衛能力に長ける。\n森林戦で本来の実力を発揮する。";
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

		_type = EntityType::dark;
		_explanation = u8"真影一族の魔法使い。\n移動力と攻撃力に長ける。\n防御力に欠ける";
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

		_type = EntityType::light;
		_explanation = u8"光琴一族の魔法使い。\n支援能力に長ける。\n戦闘向きではない。";
		return true;
	}
public:
	CREATE_FUNC(Light);
};
#endif // __ENTITY_H__
