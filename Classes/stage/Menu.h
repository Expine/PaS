#ifndef __MENU_H__
#define __MENU_H__

#include "cocos2d.h"

class Stage;
class Entity;
class StageTile;
class City;
class WeaponData;
enum class Command;

enum class MenuMode
{
	none, 
	city_supply, deploy, dispatch,
	move, moving, attack, attacking, occupy, wait,
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
	MenuMode _mode;
	cocos2d::Node* _unit;
	cocos2d::Node* _enemy_unit;
	cocos2d::Node* _map;
	cocos2d::Node* _menu;
	cocos2d::Sprite* _info;
	std::vector<cocos2d::Label*> _unitLabels;
	std::vector<cocos2d::Label*> _mapLabels;
	std::vector<cocos2d::Label*> _menuLabels;
	std::map<Command, cocos2d::Node*> _commands;
	std::map<Command, std::function<void()>> _functions;
	bool _isShowedCityCommand;
	bool _isShowedUnitCommand;
	int _selectWeapon;

	Node* setCommand(const std::string &name, const int x, const int y, const int width, const int height);
	void setFrameListener(cocos2d::Node *target, const std::vector<cocos2d::Label*> &targets, FrameType type, int moveX);
	void setMenuListener(cocos2d::Node* target, std::function<void()> func, bool isWhiteEnable = true);

	void showUnitCommandByOne(int x, int y, cocos2d::Node* command);
	void hideUnitCommandByOne(cocos2d::Node* command);
	void showCityCommandByOne(int x, int y, cocos2d::Node* command);
	void hideCityCommandByOne(cocos2d::Node* command);
protected:
	MenuLayer()
		: _unit(nullptr), _map(nullptr), _menu(nullptr), _info(nullptr)
		, _onUnitFrame(false), _onMapFrame(false), _onMenuFrame(false)
		, _stage(nullptr)
		, _isShowedCityCommand(false), _isShowedUnitCommand(false)
		, _mode(MenuMode::none)
		, _selectWeapon(0)
		, attack_decision(nullptr), attack_cancel(nullptr)
	{};
	~MenuLayer()
	{
		_unit = _map = _menu = nullptr;
		_info = nullptr;
		_onUnitFrame = _onMapFrame = _onMenuFrame = false;
		_stage = nullptr;
		_isShowedCityCommand = _isShowedUnitCommand = false;
		_selectWeapon = 0;
		attack_decision = attack_cancel = nullptr;
	};
	virtual bool init();
public:
	CREATE_FUNC(MenuLayer);
	CC_SYNTHESIZE(Stage*, _stage, Stage);
	CC_SYNTHESIZE(bool, _onUnitFrame, OnUnitFrame);
	CC_SYNTHESIZE(bool, _onMapFrame, OnMapFrame);
	CC_SYNTHESIZE(bool, _onMenuFrame, OnMenuFrame);
	void setTile(std::vector<StageTile*> tiles, Entity *unit);
	void setUnit(Node* target, std::vector<StageTile*> tiles, Entity *unit);
	inline void setUnit(std::vector<StageTile*> tiles, Entity *unit) { setUnit(_unit, tiles, unit); };
	void setInfo(int x, int y);
	inline void setInfo(cocos2d::Vec2 v) { setInfo(v.x, v.y); };
	void setUnitToTile(std::vector<StageTile*> tiles, Entity *unit);
	void checkUnitCommand(Entity* entity, std::vector<StageTile*> tiles, bool able = false);
	void moveUnitCommand();
	void hideUnitCommand();
	void checkCityCommand(Entity* entity, std::vector<StageTile*> tiles, City* city);
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

	inline std::function<void()> getFunction(Command com) { return _functions[com]; };
	inline void setFunction(Command com, std::function<void()> func) { _functions[com] = func; };

	inline MenuMode getMenuMode() { return _mode; };
	void setMenuMode(MenuMode mode, Entity *unit, std::vector<StageTile*> tiles);

	void showEnemyUnit(Entity* enemy);
	void hideEnemyUnit();

	void showWeaponFrame(Entity* unit);
	void showWeapon(cocos2d::Node* target, Entity* unit, cocos2d::Color3B color, bool checkUsable = true);
	void renderWeapon(cocos2d::Node* target, Entity* unit, WeaponData* weapon, int no, cocos2d::Color3B color, bool checkUsable = true);
	void hideWeaponFrame();

	void showSpecFrame(Entity* unit);

	void showDeployers(City* city);
	void renderDeployer(cocos2d::Node* target, Entity* unit, int y);
	void hideDeployers();

	std::function<void(WeaponData*)> attack_decision;
	std::function<void(WeaponData*)> attack_cancel;

	inline bool isCityMenuModeAble() { return _mode == MenuMode::none || _mode == MenuMode::city_supply || _mode == MenuMode::deploy || _mode == MenuMode::dispatch; };
	inline bool isUnitMenuModeAble() { return _mode == MenuMode::none || !isCityMenuModeAble(); };
};

#endif // __MENU_H__