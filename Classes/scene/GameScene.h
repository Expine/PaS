#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Entity;
class StageTile;
class Stage;
class MenuLayer;
class WeaponData;
enum class MenuMode;
enum class Command;

/*
 * SLG Game scene
 * For player turn
 */
class Game: public cocos2d::Layer
{
private:
	Stage* _stage;
	MenuLayer* _menu;
	std::vector<StageTile*> _select_tiles;
	std::vector<StageTile*> _select_area;
	std::vector<StageTile*> _move_route;
	Entity* _select_unit;
	Entity* _select_enemy;
	WeaponData* _select_weapon;

	void setMoveFunction();
	void setAttackFunction();
	void setOccupyFunction();
	void setWaitFuction();

	bool callCommand(Command com);
	void setCursor(cocos2d::Vec2 cor);
	void setSelectTiles(std::vector<StageTile*> tiles);
	void setSelectUnit(Entity* unit);

	/** Tap event function **/
	void onTap(cocos2d::Vec2 cor, std::vector<StageTile*> tiles);
	void onLongTapBagan(cocos2d::Vec2 cor, std::vector<StageTile*> tiles);
	void onDoubleTap(cocos2d::Vec2 cor, std::vector<StageTile*> tiles);
	void onDoubleTapByNone();
	void onDoubleTapByMove(bool isSameUnit);
	void onDoubleTapByMoving();
	void onDoubleTapByAttack(bool isSameUnit);
	void onDoubleTapByAttacking(Entity* target);
protected:
	Game();
	virtual ~Game();
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
	CC_SYNTHESIZE(std::function<void()>, _end_function, EndFunction);
};

#endif // __GAME_SCENE_H__
