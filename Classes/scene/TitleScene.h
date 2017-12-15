#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__

#include "cocos2d.h"
#include "util/BasedScene.h"

/*
 * Title Scene
 */
class TitleScene final : public NarUtils::BasedScene<TitleScene>
{
public:
	virtual bool init();
};

#endif // __TITLE_SCENE_H__
