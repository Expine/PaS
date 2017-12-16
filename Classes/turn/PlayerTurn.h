#ifndef __GAME_LAYER_H__
#define __GAME_LAYER_H__

#include "TurnLayer.h"

class Entity;
class StageTile;
class Stage;
class MenuLayer;
class WeaponData;
enum class MenuMode;
enum class Command;

/*
 * SLG Game layer
 * For player turn
 */
class PlayerTurn: public ITurnLayer
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
	PlayerTurn();
	virtual ~PlayerTurn();
	virtual bool init(Stage* stage);
public:
	virtual void initTurn() {};
	static cocos2d::Scene* createScene(Stage* stage);
	static PlayerTurn* create(Stage* stage)
	{
		PlayerTurn *pRet = new(std::nothrow) PlayerTurn();
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
};

#endif // __GAMEE_LAYER_H__
