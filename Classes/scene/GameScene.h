﻿#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Entity;
class StageTile;
class Stage;
class MenuLayer;
class WeaponData;

enum class GameMode
{
	normal, move, moving,
	attack, attacking
};

/*
 * SLG Game scene
 * For player turn
 */
class Game: public cocos2d::Layer
{
private:
	GameMode _mode;
	std::vector<StageTile*> _preTiles;
	std::vector<StageTile*> _moveTiles;
	std::vector<StageTile*> _moveRoot;
	Entity* _preUnit;
	Entity* _enemy;
	WeaponData* _weapon;
	void setPreTiles(Stage* stage, MenuLayer* menu, std::vector<StageTile*> tiles);
	void setPreUnit(Stage* stage, MenuLayer* menu, Entity* unit);
protected:
	Game()
		: _mode(GameMode::normal), _preUnit(nullptr), _enemy(nullptr), _weapon(nullptr)
	{};
	~Game()
	{
		_preUnit = _enemy = nullptr;
		_weapon = nullptr;
	}
	virtual bool init(Stage* stage);
public:
	static cocos2d::Scene* createScene(Stage* stage);
	static Game* create(Stage* stage)
	{
		Game *pRet = new(std::nothrow) Game();
		if (pRet && pRet->init(stage))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
	CC_SYNTHESIZE(std::function<void()>, _endFunction, EndFunction);
};

#endif // __GAME_SCENE_H__
