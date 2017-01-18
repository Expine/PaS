#include "AIScene.h"

#include "ai/Owner.h"
#include "stage/Stage.h"

USING_NS_CC;

/*
* Create scene
*/
Scene * AIScene::createScene(Stage* stage)
{
	CCLOG("AIScene is created");
	auto scene = Scene::create();
	auto layer = AIScene::create(stage);
	layer->setTag(0);
	scene->addChild(layer);
	return scene;
}

/*
 * Constructor
 */
AIScene::AIScene()
	: _stage(nullptr), _end_function(nullptr)
{}

/*
 * Destructor
 */
AIScene::~AIScene()
{
	_stage = nullptr;
	_end_function = nullptr;
}

/*
 * Initialize
 */
bool AIScene::init(Stage * stage)
{
	if (!Layer::init())
		return false;

	stage->setScale(1.0f);
	stage->initStage(Owner::enemy);
	this->addChild(stage);

	// Set cover listener
	auto cover = EventListenerTouchOneByOne::create();
	cover->setSwallowTouches(true);
	cover->onTouchBegan = [](Touch* touch, Event *event) { return true; };
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(cover, this);

	runAction(Sequence::create(
		DelayTime::create(10.0f),
		CallFunc::create([this, stage] {
			getEndFunction()();
		}),
	NULL));

	return true;
}
