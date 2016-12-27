#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "cocos2d.h"

/*
 * SLG Game scene
 * For AI
 */
class TitleScene : public cocos2d::Layer
{
protected:
	TitleScene()
	{};
	virtual bool init();
public:
	CREATE_FUNC(TitleScene);
	static cocos2d::Scene* createScene();
};

#endif // __TITLE_SCENE_H__
