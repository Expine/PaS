#ifndef __MENU_H__
#define __MENU_H__

#include "cocos2d.h"

class Stage;
class Entity;
class StageTile;

/*
 * Menu layer
 * Menu command and tile, unit, information
 */
class MenuLayer : public cocos2d::Node
{
public:
	/** Frame type*/
	enum class FrameType { unit, map, menu, info };
private:
	cocos2d::Sprite* _unit;
	cocos2d::Sprite* _map;
	cocos2d::Sprite* _menu;
	cocos2d::Sprite* _info;
	std::vector<cocos2d::Label*> _unitLabels;
	std::vector<cocos2d::Label*> _mapLabels;
	std::vector<cocos2d::Label*> _menuLabels;

	void setFrameListener(cocos2d::Sprite *target, const std::vector<cocos2d::Label*> &targets, FrameType type, int moveX);
	void setMenuListener(cocos2d::Node* target, std::function<void()> func);
protected:
	MenuLayer()
		: _unit(nullptr), _map(nullptr), _menu(nullptr), _info(nullptr)
		, _onUnitFrame(false), _onMapFrame(false), _onMenuFrame(false)
		, _stage(nullptr)
		, endPhase(nullptr), nextCity(nullptr), nextUnit(nullptr), talkStaff(nullptr), save(nullptr), load(nullptr), option(nullptr)
	{};
	~MenuLayer()
	{
		_unit = _map = _menu = _info = nullptr;
		_onUnitFrame = _onMapFrame = _onMenuFrame = false;
		_stage = nullptr;
		endPhase = nextCity = nextUnit = talkStaff = save = load = option = nullptr;
	};
	virtual bool init();
public:
	CREATE_FUNC(MenuLayer);
	CC_SYNTHESIZE(Stage*, _stage, Stage);
	CC_SYNTHESIZE(bool, _onUnitFrame, OnUnitFrame);
	CC_SYNTHESIZE(bool, _onMapFrame, OnMapFrame);
	CC_SYNTHESIZE(bool, _onMenuFrame, OnMenuFrame);
	void setTile(StageTile *tile, Entity* entity);
	void setUnit(Entity *unit);
	void setInfo(int x, int y);
	void resetOnFrame() { _onUnitFrame = _onMapFrame = _onMenuFrame = false; };
	bool checkAction(cocos2d::Vec2 diff, FrameType type, cocos2d::Sprite* target, bool *onTarget, bool isMenu);
	inline bool checkAllAction(cocos2d::Vec2 diff)
	{
		return checkAction(diff, FrameType::unit, _unit, &_onUnitFrame, false)
			&& checkAction(diff, FrameType::map,  _map,  &_onMapFrame,  false)
			&& checkAction(diff, FrameType::menu, _menu, &_onMenuFrame, false);
	};
	bool isHided(FrameType type);
	bool isHidableBySwipe(FrameType type, cocos2d::Vec2 diff);
	bool isUnHidableBySwipe(FrameType type, cocos2d::Vec2 diff);

	std::function<void()> endPhase;
	std::function<void()> nextCity;
	std::function<void()> nextUnit;
	std::function<void()> talkStaff;
	std::function<void()> save;
	std::function<void()> load;
	std::function<void()> option;
};

#endif // __MENU_H__