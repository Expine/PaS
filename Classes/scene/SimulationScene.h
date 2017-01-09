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
	std::vector<Owner> _players;
protected:
	SimulationScene()
		: _stage(nullptr)
		, _gameScene(nullptr), _aiScene(nullptr)
	{};
	~SimulationScene();
	virtual bool init();
public:
	CREATE_FUNC(SimulationScene);
	CC_SYNTHESIZE_RETAIN(cocos2d::Scene*, _gameScene, GameScene);
	CC_SYNTHESIZE_RETAIN(cocos2d::Scene*, _aiScene, AiScene);
	static cocos2d::Scene* createScene();
	void nextTurn(Owner owner);
public:
	inline void addPlayer(const Owner player) { _players.push_back(player); };
};

#endif // __SIMULATION_SCENE_H__
