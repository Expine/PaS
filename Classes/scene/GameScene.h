#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Entity;

class Game: public cocos2d::Layer
{
private:
	cocos2d::Vector<cocos2d::Sprite*> _preTiles;
	Entity* _preUnit;
	std::vector<std::string> _players;
	int turn;
protected:
	Game() 
		: turn(0)
	{
		addPlayer("My");
		addPlayer("Enemy");
		addPlayer("Friend");
	};
	virtual bool init();
public:
    static cocos2d::Scene* createScene();
	inline void addPlayer(const std::string &player) { _players.push_back(player); };
    CREATE_FUNC(Game);
};

#endif // __GAME_SCENE_H__
