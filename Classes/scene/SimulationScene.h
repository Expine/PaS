#ifndef __SIMULATION_SCENE_H__
#define __SIMULATION_SCENE_H__

#include "cocos2d.h"
#include "ai/Owner.h"

class Stage;

/*
 * SLG Game scene
 * Manage each Simulation scene
 */
class SimulationScene : public cocos2d::Layer
{
private:
	Stage* _stage;
	std::vector<Owner> _players;
	int _turn;
protected:
	SimulationScene()
		: _turn(0)
		, _stage(nullptr)
	{};
	~SimulationScene();
	virtual bool init();
public:
	CREATE_FUNC(SimulationScene);
	static cocos2d::Scene* createScene();
	void nextTurn();
public:
	inline void addPlayer(const Owner player) { _players.push_back(player); };
};

#endif // __SIMULATION_SCENE_H__
