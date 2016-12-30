﻿#ifndef __MENU_H__
#define __MENU_H__

#include "cocos2d.h"

class Stage;
class Entity;
class StageTile;
class City;
enum class UnitCommand;
enum class CityCommand;
enum class MoveCommand;

enum class MenuMode
{
	unit, move
};

/*********************************************************/

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
	std::map<UnitCommand, cocos2d::Node*> _unit_command;
	std::map<UnitCommand, std::function<void()>> _unit_function;
	std::map<CityCommand, cocos2d::Node*> _city_command;
	std::map<CityCommand, std::function<void()>> _city_function;
	std::map<MoveCommand, cocos2d::Node*> _move_command;
	std::map<MoveCommand, std::function<void()>> _move_function;
	bool _isShowedCityCommand;
	bool _isShowedUnitCommand;
	MenuMode _mode;

	Node* setCommand(const std::string &name, const int x, const int y, const int width, const int height);
	void setFrameListener(cocos2d::Node *target, const std::vector<cocos2d::Label*> &targets, FrameType type, int moveX);
	void setMenuListener(cocos2d::Node* target, std::function<void()> func);
protected:
	MenuLayer()
		: _unit(nullptr), _map(nullptr), _menu(nullptr), _info(nullptr)
		, _onUnitFrame(false), _onMapFrame(false), _onMenuFrame(false)
		, _stage(nullptr)
		, _isShowedCityCommand(false), _isShowedUnitCommand(false)
		, _mode(MenuMode::unit)
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
		return _unit->getNumberOfRunningActions() != 0 || _map->getNumberOfRunningActions() != 0 || _menu->getNumberOfRunningActions() != 0;
	};
	bool isHided(FrameType type);
	bool isHidableBySwipe(FrameType type, cocos2d::Vec2 diff);
	bool isUnHidableBySwipe(FrameType type, cocos2d::Vec2 diff);
	
	inline std::function<void()> getUnitFunction(UnitCommand com) { return _unit_function[com]; };
	inline std::function<void()> getCityFunction(CityCommand com) { return _city_function[com]; };
	inline std::function<void()> getMoveFunction(MoveCommand com) { return _move_function[com]; };
	inline void setUnitFunction(UnitCommand com, std::function<void()> func) { _unit_function[com] = func; };
	inline void setCityFunction(CityCommand com, std::function<void()> func) { _city_function[com] = func; };
	inline void setMoveFunction(MoveCommand com, std::function<void()> func) { _move_function[com] = func; };

	inline MenuMode getMenuMode() { return _mode; };
	void setMenuMode(MenuMode mode);

	std::function<void()> endPhase;
	std::function<void()> nextCity;
	std::function<void()> nextUnit;
	std::function<void()> talkStaff;
	std::function<void()> save;
	std::function<void()> load;
	std::function<void()> option;
};

#endif // __MENU_H__