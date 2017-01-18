#ifndef __SIMULATION_SCENE_H__
#define __SIMULATION_SCENE_H__

#include "cocos2d.h"

class Stage;
enum class Owner;

/*
 * SLG Game scene
 * Manage each Simulation scene
 */
class SimulationScene : public cocos2d::Layer
{
private:
	Stage* _stage;
	std::queue<Owner> _players;
protected:
	SimulationScene();
	virtual ~SimulationScene();
	virtual bool init();
public:
	CREATE_FUNC(SimulationScene);
	CC_SYNTHESIZE_RETAIN(cocos2d::Scene*, _game_scene, GameScene);
	CC_SYNTHESIZE_RETAIN(cocos2d::Scene*, _ai_scene, AiScene);
	static cocos2d::Scene* createScene();
	void nextTurn(bool init = false);
public:
	/** Add player*/
	inline void addPlayer(const Owner player) { _players.push(player); };
};

#endif // __SIMULATION_SCENE_H__
