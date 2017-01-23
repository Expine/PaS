#include "AIScene.h"

#include "ai/Owner.h"
#include "stage/Stage.h"

USING_NS_CC;

/*
* Create scene
*/
Scene * AIScene::createScene(Stage* stage, Owner owner)
{
	CCLOG("AIScene is created");
	auto scene = Scene::create();
	auto layer = AIScene::create(stage, owner);
	layer->setTag(0);
	scene->addChild(layer);
	return scene;
}

/*
 * Constructor
 */
AIScene::AIScene()
	: _stage(nullptr), _end_function(nullptr), _ai(nullptr)
{}

/*
 * Destructor
 */
AIScene::~AIScene()
{
	_stage = nullptr;
	_end_function = nullptr;
	CC_SAFE_RELEASE_NULL(_ai);
}

/*
 * Initialize
 */
bool AIScene::init(Stage * stage, Owner owner)
{
	if (!Layer::init())
		return false;

	stage->setScale(3.0f);
	stage->initStage(owner);
	this->addChild(stage);

	// Set cover listener
	auto cover = EventListenerTouchOneByOne::create();
	cover->setSwallowTouches(true);
	cover->onTouchBegan = [](Touch* touch, Event *event) { return true; };
	this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(cover, this);

	setAI(PlayerAI::create());
	getAI()->initialize(stage, owner);
	getAI()->execute();

	Vector<FiniteTimeAction*> acts;
	auto i = 0;
	for (auto i = 0; i < getAI()->getExecutesNumber(); i++)
	{
		acts.pushBack(CallFunc::create([this] { getAI()->nextExecute(); }));
		acts.pushBack(DelayTime::create(1.0f));
	}
	acts.pushBack(DelayTime::create(3.0f));
	acts.pushBack(CallFunc::create([this, stage] {
		getEndFunction()();
	}));

	runAction(Sequence::create(acts));

	return true;
}
