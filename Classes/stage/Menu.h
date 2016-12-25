#ifndef __MENU_H__
#define __MENU_H__

#include "cocos2d.h"
#include "stage/Tile.h"

class MenuLayer : public cocos2d::Node
{
public:
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
protected:
	MenuLayer();
	~MenuLayer();
public:
	CREATE_FUNC(MenuLayer);
	CC_SYNTHESIZE(bool, _onUnitFrame, OnUnitFrame);
	CC_SYNTHESIZE(bool, _onMapFrame, OnMapFrame);
	CC_SYNTHESIZE(bool, _onMenuFrame, OnMenuFrame);
	virtual bool init();
	void setUnit();
	void setTile(StageTile *tile);
	void setInfo(int x, int y);
	void resetOnFrame() { _onUnitFrame = _onMapFrame = _onMenuFrame = false; };
	bool checkUnitAction(cocos2d::Vec2 diff)
	{
		CCLOG("%d, %d -> (%f)", _onUnitFrame, isHided(FrameType::unit), diff.x);
		if (diff.x > 0 && !isHided(FrameType::unit) && _unit->numberOfRunningActions() == 0)
			_onUnitFrame = false;
		if (diff.x < 0 && isHided(FrameType::unit) && _unit->numberOfRunningActions() == 0)
			_onUnitFrame = false;
		return !_onUnitFrame;
	};
	bool checkMapAction(cocos2d::Vec2 diff)
	{
		if (diff.x > 0 && !isHided(FrameType::map) && _map->numberOfRunningActions() == 0)
			_onMapFrame = false;
		if (diff.x < 0 && isHided(FrameType::map) && _map->numberOfRunningActions() == 0)
			_onMapFrame = false;
		return !_onMapFrame;
	};
	bool checkMenuAction(cocos2d::Vec2 diff)
	{
		if (diff.x > 0 && !isHided(FrameType::menu) && _menu->numberOfRunningActions() == 0)
			_onMenuFrame = false;
		if (diff.x < 0 && isHided(FrameType::menu) && _menu->numberOfRunningActions() == 0)
			_onMenuFrame = false;
		return !_onMenuFrame;
	};
	bool checkAllAction(cocos2d::Vec2 diff)
	{
		return checkUnitAction(diff) && checkMapAction(diff) && checkMenuAction(diff);
	};
	bool isHided(FrameType type)
	{
		switch (type)
		{
		case FrameType::unit:	return _unit->getPosition().x < 0;
		case FrameType::map:	return _map->getPosition().x < 0;
		case FrameType::menu:	return _menu->getPosition().x > cocos2d::Director::getInstance()->getWinSize().width;
		case FrameType::info:	return _info->getPosition().x > cocos2d::Director::getInstance()->getWinSize().width;
		}
		return false;
	}
	bool isHidableBySwipe(FrameType type, cocos2d::Vec2 diff)
	{
		switch (type)
		{
		case FrameType::unit:	return diff.x < 0 && !isHided(type);
		case FrameType::map:	return diff.x < 0 && !isHided(type);
		case FrameType::menu:	return diff.x > 0 && !isHided(type);
		case FrameType::info:	return diff.x > 0 && !isHided(type);
		}
		return false;
	};
	bool isUnHidableBySwipe(FrameType type, cocos2d::Vec2 diff)
	{
		switch (type)
		{
		case FrameType::unit:	return diff.x > 0 && isHided(type);
		case FrameType::map:	return diff.x > 0 && isHided(type);
		case FrameType::menu:	return diff.x < 0 && isHided(type);
		case FrameType::info:	return diff.x < 0 && isHided(type);
		}
		return false;
	}
};

#endif // __MENU_H__