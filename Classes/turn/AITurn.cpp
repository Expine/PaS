#include "AITurn.h"

#include "ai/Owner.h"
#include "stage/Stage.h"

USING_NS_CC;

/*
* Create scene
*/
Scene * AITurn::createScene(Stage* stage, Owner owner)
{
	CCLOG("AITurn is created");
	auto scene = Scene::create();
	auto layer = AITurn::create(stage, owner);
	layer->setTag(0);
	scene->addChild(layer);
	return scene;
}

/*
 * Constructor
 */
AITurn::AITurn()
	: _stage(nullptr), _ai(nullptr)
{}

/*
 * Destructor
 */
AITurn::~AITurn()
{
	_stage = nullptr;
	CC_SAFE_RELEASE_NULL(_ai);
}

/*
 * Initialize
 */
bool AITurn::init(Stage * stage, Owner owner)
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
		_nextTurn();
	}));

	runAction(Sequence::create(acts));

	return true;
}
