#ifndef __MENU_H__
#define __MENU_H__

#include "cocos2d.h"

class Stage;
class Entity;
class StageTile;
class City;
class WeaponData;
class SingleTouchListener;
enum class Command;

enum class MenuMode
{
	none, 
	city_supply, deploy, dispatch, dispatching,
	move, moving, attack, attacking, occupy, wait,
};

/** Frame type*/
enum class FrameType 
{ 
	unit, map, menu, info 
};
/*********************************************************/

/*
 * Menu layer
 * Menu command and tile, unit, information
 */
class MenuLayer : public cocos2d::Node
{
private:
	cocos2d::Node* _unit_frame;
	cocos2d::Node* _enemy_unit_frame;
	cocos2d::Node* _map_frame;
	cocos2d::Node* _menu_frame;
	cocos2d::Sprite* _info_frame;
	std::vector<cocos2d::Label*> _unit_labels;
	std::vector<cocos2d::Label*> _map_labels;
	std::vector<cocos2d::Label*> _menu_labels;
	std::map<FrameType, bool> _on_frame;
	std::map<FrameType, bool> _is_hided;
	std::map<Command, cocos2d::Node*> _command_frames;
	std::map<Command, std::function<void()>> _command_functions;
	bool _is_showed_city_command;
	bool _is_showed_unit_command;

	cocos2d::Node* setEachFrame(const cocos2d::Vec2 anchor, const int x, const int y, const int w, const int h, const int cut_mask, std::vector<std::string> words, std::vector<cocos2d::Label*>&);
	cocos2d::Node* setCommand(const std::string &name, const int x, const int y, const int width, const int height);
	void setFrameListener(cocos2d::Node *target, const std::vector<cocos2d::Label*> &labels, FrameType type, int moveX);
	void showFrame(cocos2d::Node *target, const std::vector<cocos2d::Label*> &labels, FrameType type, int moveX);
	void hideFrame(cocos2d::Node *target, const std::vector<cocos2d::Label*> &labels, FrameType type, int moveX);
	void setCommandListener(cocos2d::Node* target, std::function<void()> &func, bool isWhiteEnable = true);

	bool isHidableBySwipe(FrameType type, cocos2d::Vec2 diff);
	bool isShowableBySwipe(FrameType type, cocos2d::Vec2 diff);


	void showUnitCommandByOne(int x, int y, Command com);
	void hideUnitCommandByOne(Command com);
	void showCityCommandByOne(int x, int y, Command com);
	void hideCityCommandByOne(Command com);
protected:
	MenuLayer();
	virtual ~MenuLayer();
	virtual bool init();
public:
	CREATE_FUNC(MenuLayer);
	CC_SYNTHESIZE(MenuMode, _mode, Mode);
	CC_SYNTHESIZE(Stage*, _stage, Stage);
	CC_SYNTHESIZE(int, _select_weapon, SelectWeapon);
	CC_SYNTHESIZE(Entity*, _select_deployer, SelectDeployer);
	std::function<void(WeaponData*)> attack_decision;
	std::function<void(WeaponData*)> attack_cancel;

	/** Set command function called by tapping*/
	inline void setFunction(Command com, std::function<void()> func) { _command_functions[com] = func; };
	/** Get command function*/
	inline std::function <void()> getFunction(Command com) { return _command_functions[com]; };
	/** Set mode with check and move command for unit*/
	inline void setModeWithCheckAndMoveForUnit(MenuMode mode, Entity* entity, std::vector<StageTile*> tiles, Entity* enemy, WeaponData* weapon, std::vector<StageTile*> area)
	{
		hideUnitCommand(); setMode(mode); checkUnitCommand(entity, tiles, enemy, weapon, area); moveUnitCommand();
	};
	/** Set mode with check and move command for city*/
	inline void setModeWithCheckAndMoveForCity(MenuMode mode, Entity* entity, std::vector<StageTile*> tiles, Entity* enemy, WeaponData* weapon, std::vector<StageTile*> area)
	{
		hideCityCommand(); setMode(mode); checkCityCommand(entity, tiles, enemy, weapon, area); moveCityCommand();
	};

	/** Function to set information **/
	void setTileInformation(std::vector<StageTile*> tiles, Entity *unit);
	void setCityDurability(City* city);
	void setUnitInformation(std::vector<StageTile*> tiles, Entity *unit, cocos2d::Node* target);
	inline void setUnitInformation(std::vector<StageTile*> tiles, Entity *unit) { setUnitInformation(tiles, unit, _unit_frame); };
	void setUnitAbility(Entity* unit, cocos2d::Node* target);
	void setCoordinateInformation(cocos2d::Vec2 cor);
	void setUnitToTileInformation(std::vector<StageTile*> tiles, Entity *unit);

	/** Function to manage unit command **/
	void checkUnitCommand(Entity* entity, std::vector<StageTile*> tiles, Entity* enemy, WeaponData* weapon, std::vector<StageTile*> area);
	void moveUnitCommand();
	void hideUnitCommand();
	/** Function to manage city command **/
	void checkCityCommand(Entity* entity, std::vector<StageTile*> tiles, Entity* enemy, WeaponData* weapon, std::vector<StageTile*> area);
	void moveCityCommand();
	void hideCityCommand();
	/** Check whether city command mode */
	inline bool isCityMenuModeAble() { return _mode == MenuMode::none || _mode == MenuMode::city_supply || _mode == MenuMode::deploy || _mode == MenuMode::dispatch; };
	/** Check whether unit command mode */
	inline bool isUnitMenuModeAble() { return _mode == MenuMode::none || !isCityMenuModeAble(); };

	/** Function to check action **/
	bool checkAction(cocos2d::Vec2 diff, FrameType type, cocos2d::Node* target);
	/** Check whether all action unlock onFrame */
	inline bool checkAllAction(cocos2d::Vec2 diff)
	{
		return checkAction(diff, FrameType::unit, _unit_frame) && checkAction(diff, FrameType::map,  _map_frame) && checkAction(diff, FrameType::menu, _menu_frame);
	};
	/** Check whether any action running*/
	inline bool isRunningAction()
	{
		return _unit_frame->getNumberOfRunningActions() != 0 || _map_frame->getNumberOfRunningActions() != 0 || _menu_frame->getNumberOfRunningActions() != 0;
	};

	/** Function to make additional information**/
	void showEnemyUnit(Entity* enemy);
	void hideEnemyUnit();
	void showWeaponFrame(Entity* unit);
	void showWeapon(cocos2d::Node* target, Entity* unit, cocos2d::Color3B color, bool checkUsable = true);
	void renderWeapon(cocos2d::Node* target, Entity* unit, WeaponData* weapon, int no, cocos2d::Color3B color, bool checkUsable = true);
	void hideWeaponFrame();
	void showSpecFrame(Entity* unit);
	SingleTouchListener* showDeployers(City* city);
	void renderDeployer(cocos2d::Node* target, Entity* unit, int y);
	void hideDeployers();

	void deploy(City* city);
	void dispatch(City* city);
};

#endif // __MENU_H__