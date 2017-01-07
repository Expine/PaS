#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "cocos2d.h"

class Entity;
class StageTile;

enum class Command
{
	MENU_START,

	endPhase, nextUnit, nextCity, talkStaff, save, load, option,

	MENU_END,
	UNIT_START,

	supply, move, attack, occupation, spec, wait,

	UNIT_END,
	CITY_START,

	city_supply, deployment, dispatch,

	CITY_END,
	MOVE_START,

	move_start, move_end,

	MOVE_END,
	MOVE_2_START,

	move_decision, move_cancel,

	MOVE_2_END,
	ATTACK_START,

	attack_target, attack_change, attack_end,

	ATTACK_END,
	ATTACK_2_START,

	attack_start, attack_cancel,

	ATTACK_2_END,
	OCCUPATION_START,

	occupation_start, occupation_end,

	OCCUPATION_END,
	WAIT_START,

	wait_start, wait_end,

	WAIT_END,
	CITY_SUPPLY_START,

	city_supply_start, city_supply_end,

	CITY_SUPPLY_END,
	DEPLOY_START,

	deploy_start, deploy_end,

	DEPLOY_END,
	DISPATCH_START,

	dispatch_start, dispatch_change, dispatch_cancel
};

namespace command
{
	inline Command castCommand(int i) { return static_cast<Command>(i); };

	void forMenu(std::function<void(Command, int)> func);

	void forUnit(std::function<void(Command, int)> func);
	void forMove(std::function<void(Command, int)> func);
	void forMove2(std::function<void(Command, int)> func);
	void forAttack(std::function<void(Command, int)> func);
	void forAttack2(std::function<void(Command, int)> func);
	void forOccupation(std::function<void(Command, int)> func);
	void forWait(std::function<void(Command, int)> func);

	void forCity(std::function<void(Command, int)> func);
	void forCitySupply(std::function<void(Command, int)> func);
	void forDeploy(std::function<void(Command, int)> func);
	void forDispatch(std::function<void(Command, int)> func);

	void forAllUnit(std::function<void(Command, int)> func);
	void forAllCity(std::function<void(Command, int)> func);
	inline void forAll(std::function<void(Command, int)> func) { forAllUnit(func); forAllCity(func); };

	const std::string getName(Command com);

	bool isEnable(Command com, Entity *unit, std::vector<StageTile*> tile);
};


#endif // __COMMAND_H__
