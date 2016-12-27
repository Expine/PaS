#ifndef __SIMULATION_SCENE_H__
#define __SIMULATION_SCENE_H__

#include "cocos2d.h"

/*
 * SLG Game scene
 * Manage each Simulation scene
 */
class SimulationScene : public cocos2d::Layer
{
private:
	std::vector<std::string> _players;
	int _turn;
protected:
	SimulationScene()
		: _turn(0)
	{};
	~SimulationScene()
	{
		_turn = 0;
	}
	virtual bool init();
public:
	CREATE_FUNC(SimulationScene);
	static cocos2d::Scene* createScene();
	void nextTurn();
public:
	inline void addPlayer(const std::string &player) { _players.push_back(player); };
};

#endif // __SIMULATION_SCENE_H__
