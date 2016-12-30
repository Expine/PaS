#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "cocos2d.h"

#include "ai/Owner.h"
#include "stage/Command.h"

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
	COUNT,
	sight
};

/** Unit state*/
enum class EntityState
{
	none, supplied, moved, end
};

/*********************************************************/

class EntityInformation
{
private:
	std::map<EntityType, std::string> _name;
	std::map<EntityType, std::map<UnitCommand, bool>> _unit_commands;
protected:
	EntityInformation()
	{
		_name[EntityType::infantry] = u8"軽歩兵";
		_name[EntityType::heavy] = u8"重装兵";
		_name[EntityType::spy] = u8"諜報員";
		_name[EntityType::fire] = u8"炎峰魔士";
		_name[EntityType::ice] = u8"雹牢魔士";
		_name[EntityType::thunder] = u8"雷翔魔士";
		_name[EntityType::ground] = u8"大地魔士";
		_name[EntityType::king] = u8"王藤魔士";
		_name[EntityType::weapon] = u8"双霊魔士";
		_name[EntityType::relief] = u8"愛貴魔士";
		_name[EntityType::guardian] = u8"樹奏魔士";
		_name[EntityType::dark] = u8"真影魔士";
		_name[EntityType::light] = u8"光琴魔士";
		
		for (int i = 0; i < static_cast<int>(EntityType::COUNT); i++)
			forUnit(j)
				if (j == static_cast<int>(UnitCommand::occupation) && i > static_cast<int>(EntityType::heavy))
					_unit_commands[static_cast<EntityType>(i)][static_cast<UnitCommand>(j)] = false;
				else if (j == static_cast<int>(UnitCommand::supply))
					_unit_commands[static_cast<EntityType>(i)][static_cast<UnitCommand>(j)] = false;
				else
					_unit_commands[static_cast<EntityType>(i)][static_cast<UnitCommand>(j)] = true;
	};
public:
	static EntityInformation* getInstance()
	{
		static EntityInformation info;
		return &info;
	};
	inline const std::string& getName(EntityType type) { return _name[type]; };
	inline bool getCommand(EntityType type, UnitCommand command) { return _unit_commands[type][command]; };
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
		, _usingWeapon(0), _mobility(0), _material(0), _maxMaterial(0), _searchingAbility(0), _defence(0), _durability(0), _maxDurability(0)
	{};
	~Entity()
	{
		_usingWeapon = _mobility = _material = _maxMaterial = _searchingAbility = _defence = _durability = _maxDurability = 0;
	};
public:
	CREATE_FUNC(Entity);
	CC_SYNTHESIZE(Owner, _affiliation, Affiliation);
	inline Entity* setAffiliationRetThis(Owner owner) { _affiliation = owner; return this; };
	CC_SYNTHESIZE(EntityState, _state, State);
	CC_SYNTHESIZE(int, _usingWeapon, UsingWeapon);
	CC_SYNTHESIZE(int, _material, Material);
	CC_SYNTHESIZE(int, _durability, Durability);
	CC_SYNTHESIZE_READONLY(Department, _department, Department);
	CC_SYNTHESIZE_READONLY(EntityType, _type, Type);
	CC_SYNTHESIZE_READONLY(std::string, _explanation, Explanation);
	CC_SYNTHESIZE_READONLY(int, _mobility, Mobility);
	CC_SYNTHESIZE_READONLY(int, _maxMaterial, MaxMaterial);
	CC_SYNTHESIZE_READONLY(int, _searchingAbility, SearchingAbility);
	CC_SYNTHESIZE_READONLY(int, _defence, Defence);
	CC_SYNTHESIZE_READONLY(int, _maxDurability, MaxDurability);
	static Entity* create(EntityType type, const int x, const int y, cocos2d::SpriteBatchNode* batch, Stage* stage);
	inline cocos2d::Vec2 getTileCoordinate(int mapy) { return cocos2d::Vec2(getTag() / mapy, getTag() % mapy); };
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 2;
		_defence = 10;
		_durability = _maxDurability = 100;
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
		_mobility = 8;
		_material = _maxMaterial = 80;
		_searchingAbility = 1;
		_defence = 30;
		_durability = _maxDurability = 200;
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
		_mobility = 16;
		_material = _maxMaterial = 10;
		_searchingAbility = 5;
		_defence = 2;
		_durability = _maxDurability = 50;
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 120;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 20;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 18;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
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
		_mobility = 12;
		_material = _maxMaterial = 100;
		_searchingAbility = 1;
		_defence = 100;
		_durability = _maxDurability = 300;
		return true;
	}
public:
	CREATE_FUNC(Light);
};
#endif // __ENTITY_H__
