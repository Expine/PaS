#ifndef __SIMULATION_SCENE_H__
#define __SIMULATION_SCENE_H__

#include "util/BasedScene.h"

class ITurnLayer;
class Stage;
enum class Owner;

class TurnFactory {
public:
	static ITurnLayer* getTurn(Owner owner, Stage* stage);
};


/*
 * SLG Game scene
 * Manage each Simulation scene
 */
class SimulationScene : public NarUtils::IBasedScene<SimulationScene>
{
private:
	Stage* _stage;
	std::queue<Owner> _players;
public:
	SimulationScene()
	: _stage(nullptr), _game_scene(nullptr), _ai_scene(nullptr)
	{};
	virtual ~SimulationScene();
	virtual bool init();
	CC_SYNTHESIZE_RETAIN(cocos2d::Scene*, _game_scene, GameScene);
	CC_SYNTHESIZE_RETAIN(cocos2d::Scene*, _ai_scene, AiScene);
	void nextTurn(bool init = false);
public:
	/** Add player*/
	inline void addPlayer(const Owner player) { _players.push(player); };
};

#endif // __SIMULATION_SCENE_H__
