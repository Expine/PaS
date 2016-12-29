#ifndef __MENU_H__
#define __MENU_H__

#include "cocos2d.h"

class Stage;
class Entity;
class StageTile;
class City;

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
	cocos2d::Node* _unit;
	cocos2d::Node* _map;
	cocos2d::Node* _menu;
	cocos2d::Sprite* _info;
	std::vector<cocos2d::Label*> _unitLabels;
	std::vector<cocos2d::Label*> _mapLabels;
	std::vector<cocos2d::Label*> _menuLabels;
	cocos2d::Vector<cocos2d::Node*> _unit_command;
	cocos2d::Vector<cocos2d::Node*> _city_command;
	bool _isShowedCityCommand;
	bool _isShowedUnitCommand;

	void setFrameListener(cocos2d::Node *target, const std::vector<cocos2d::Label*> &targets, FrameType type, int moveX);
	void setMenuListener(cocos2d::Node* target, std::function<void()> func);
protected:
	MenuLayer()
		: _unit(nullptr), _map(nullptr), _menu(nullptr), _info(nullptr)
		, _onUnitFrame(false), _onMapFrame(false), _onMenuFrame(false)
		, _stage(nullptr)
		, _isShowedCityCommand(false), _isShowedUnitCommand(false)
		, endPhase(nullptr), nextCity(nullptr), nextUnit(nullptr), talkStaff(nullptr), save(nullptr), load(nullptr), option(nullptr)
	{};
	~MenuLayer()
	{
		_unit = _map = _menu = nullptr;
		_info = nullptr;
		_onUnitFrame = _onMapFrame = _onMenuFrame = false;
		_stage = nullptr;
		_isShowedCityCommand = _isShowedUnitCommand = false;
		endPhase = nextCity = nextUnit = talkStaff = save = load = option = nullptr;
	};
	virtual bool init();
public:
	CREATE_FUNC(MenuLayer);
	CC_SYNTHESIZE(Stage*, _stage, Stage);
	CC_SYNTHESIZE(bool, _onUnitFrame, OnUnitFrame);
	CC_SYNTHESIZE(bool, _onMapFrame, OnMapFrame);
	CC_SYNTHESIZE(bool, _onMenuFrame, OnMenuFrame);
	void setTile(std::vector<StageTile*> tiles, Entity *unit);
	void setUnit(std::vector<StageTile*> tiles, Entity *unit);
	void setInfo(int x, int y);
	void setUnitToTile(std::vector<StageTile*> tiles, Entity *unit);
	void showUnitCommand(Entity* entity);
	void moveUnitCommand();
	void hideUnitCommand();
	void showCityCommand(City* city);
	void moveCityCommand();
	void hideCityCommand();

	void resetOnFrame() { _onUnitFrame = _onMapFrame = _onMenuFrame = false; };
	bool checkAction(cocos2d::Vec2 diff, FrameType type, cocos2d::Node* target, bool *onTarget, bool isMenu);
	inline bool checkAllAction(cocos2d::Vec2 diff)
	{
		return checkAction(diff, FrameType::unit, _unit, &_onUnitFrame, false)
			&& checkAction(diff, FrameType::map,  _map,  &_onMapFrame,  false)
			&& checkAction(diff, FrameType::menu, _menu, &_onMenuFrame, false);
	};
	inline bool isRunningAction()
	{
		return _unit->getNumberOfRunningActions() != 0 | _map->getNumberOfRunningActions() != 0 | _menu->getNumberOfRunningActions() != 0;
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