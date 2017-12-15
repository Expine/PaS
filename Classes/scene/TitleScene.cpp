#include "TitleScene.h"
#include "SimulationScene.h"

USING_NS_CC;

/*
 * Initialize
 */
bool TitleScene::init()
{
	if (!Layer::init())
		return false;

	auto winSize = Director::getInstance()->getWinSize();

	// Logo
	auto logo = Sprite::create("image/logo.png");
	logo->setPosition(winSize.width / 2, winSize.height / 2 + 100);
	this->addChild(logo);

	// Set Listener
	auto lis = EventListenerTouchOneByOne::create();
	lis->onTouchBegan = [](Touch *touch, Event *event)
	{
		Director::getInstance()->replaceScene(SimulationScene::createScene());
		return true;
	};
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(lis, this);

	return true;
}