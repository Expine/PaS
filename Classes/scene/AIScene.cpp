#include "AIScene.h"

USING_NS_CC;

/*
 * Create scene
 */
Scene * AIScene::createScene()
{
	CCLOG("AIScene is created");
	auto scene = Scene::create();
	auto layer = AIScene::create();
	scene->addChild(layer);
	return scene;
}
