#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Entity;
class StageTile;
class Stage;
class MenuLayer;
class WeaponData;

/*
 * SLG Game scene
 * For player turn
 */
class Game: public cocos2d::Layer
{
private:
	std::vector<StageTile*> _selectTiles;
	std::vector<StageTile*> _selectArea;
	std::vector<StageTile*> _moveRoot;
	Entity* _selectUnit;
	Entity* _selectEnemy;
	WeaponData* _weapon;
	void setCursol(Stage* stage, MenuLayer* menu, cocos2d::Vec2 tileCoordinate);
	void setSelectTiles(Stage* stage, MenuLayer* menu, std::vector<StageTile*> tiles);
	void setSelectUnit(Stage* stage, MenuLayer* menu, Entity* unit);
protected:
	Game()
		: _selectUnit(nullptr), _selectEnemy(nullptr), _weapon(nullptr)
	{};
	~Game()
	{
		_selectUnit = _selectEnemy = nullptr;
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
