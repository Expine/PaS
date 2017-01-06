#ifndef __MULTI_LISTENER_H__
#define __MULTI_LISTENER_H__

#include "cocos2d.h"

USING_NS_CC;

constexpr float DefaultLongTapThreshold = 0.1f;
constexpr float DefaultDoubleTapThreshold = 0.15f;
constexpr float DefaultSwipeThresholdDistance = 10.0f;
constexpr float DefaultFlickThreshold = 0.1f;

constexpr float DefaultScrollMagnification = 1.2f;

/*
 * Multi touch event listener
 */
class MultiTouchListener : public cocos2d::EventListenerTouchAllAtOnce
{
public:
	enum class State 
	{
		check, tap, swipe, multi, longtap
	};

private:
	using cocos2d::EventListenerTouchAllAtOnce::LISTENER_ID;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesBegan;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesMoved;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesEnded;
	using cocos2d::EventListenerTouchAllAtOnce::onTouchesCancelled;

	int _numberOfTouch;
	std::vector<cocos2d::Vec2> _startPos;
	std::vector<struct timeval> _startTime;
	std::vector<cocos2d::Vec2> _movePos;
	std::vector<struct timeval> _moveTime;
	struct timeval _multiTime;
	cocos2d::Vec2 _multiPos;
	float _multiDiff;
	std::vector<State> _state;

protected:
	MultiTouchListener()
		: _numberOfTouch(0)
		, _longTapThreshold(DefaultLongTapThreshold), _doubleTapThreshold(DefaultDoubleTapThreshold)
		, _swipeThreshold(DefaultSwipeThresholdDistance), _flickThreshold(DefaultFlickThreshold)
		, onTap(nullptr), onLongTapBegan(nullptr), onLongTapEnd(nullptr), onDoubleTap(nullptr)
		, onSwipe(nullptr), pinchIn(nullptr), pinchOut(nullptr)
		, onSwipeCheck(nullptr), onFlickCheck(nullptr)
	{
		_startPos.resize(5);
		_startTime.resize(5);
		_movePos.resize(5);
		_moveTime.resize(5);
		_state.resize(5);
	};
	~MultiTouchListener() 
	{
		_numberOfTouch = 0;
		_longTapThreshold = _doubleTapThreshold = _swipeThreshold = _flickThreshold = 0;
		onTap = onLongTapBegan = onLongTapEnd = onDoubleTap = nullptr;
		onSwipe = nullptr;
		pinchIn = pinchOut = nullptr;
		onSwipeCheck = onFlickCheck = nullptr;
	};
	/*
	 * Initialize
	 * Parse listener process
	 */
	virtual bool init()
	{
		if (!EventListenerTouchAllAtOnce::init())
			return false;

		onTouchesBegan = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			//count up
			_numberOfTouch += touches.size();
			for (cocos2d::Touch* t : touches)
			{
				// The third doesn't correspond
				if (t->getID() > 1)
					return;
				// Initialize
				_startPos[t->getID()] = t->getLocation();
				gettimeofday(&_startTime[t->getID()], NULL);
				_movePos[t->getID()] = t->getLocation();
				gettimeofday(&_moveTime[t->getID()], NULL);
				_state[t->getID()] = MultiTouchListener::State::check;
				// Start process of checking long tap
				cocos2d::Director::getInstance()->getScheduler()->schedule(schedule_selector(MultiTouchListener::update), this, 0.05f, false);
			}
			// For multi tap
			if (_numberOfTouch > 1)
			{
				for (int i = 0; i < _numberOfTouch; i++)
					_state[i] = MultiTouchListener::State::multi;
				gettimeofday(&_multiTime, NULL);
				_multiPos = (_movePos[0] + _movePos[1]) / 2;
				_multiDiff = (_movePos[0] - _movePos[1]).lengthSquared();
			}

			// Process execute when touching
			if (onTouchBegan) onTouchBegan(touches[0]->getLocation());
		};
		onTouchesMoved = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			for (cocos2d::Touch* t : touches)
			{
				// The third doesn't correspond
				if (t->getID() > 1)
					return;
				// Move check
				if (_state[t->getID()] != MultiTouchListener::State::swipe && 
					_state[t->getID()] != MultiTouchListener::State::multi &&
					_state[t->getID()] != MultiTouchListener::State::longtap)
				{
					auto diff = (t->getLocation() - _movePos[t->getID()]).lengthSquared();
//					CCLOG("now (%f, %f)", t->getLocation().x, t->getLocation().y);
//					CCLOG("check diff %f : %d", diff, t->getID());
					if (diff > _swipeThreshold)
						_state[t->getID()] = MultiTouchListener::State::swipe;
					else
						_state[t->getID()] = MultiTouchListener::State::tap;
				}

				// Move process
				if (_state[t->getID()] == MultiTouchListener::State::swipe)
				{
					struct timeval time;
					gettimeofday(&time, NULL);
					auto diff = (t->getLocation() - _movePos[t->getID()]);
					auto diff_time = (float)(time.tv_sec - _moveTime[t->getID()].tv_sec) + (float)(time.tv_usec - _moveTime[t->getID()].tv_usec) / 1000000;
					if (onSwipe && (!onSwipeCheck || onSwipeCheck(t->getLocation(), diff, diff_time))) 
						onSwipe(t->getLocation(), diff, diff_time);
//					CCLOG("move diff %f : %d", diff.lengthSquared(), t->getID());
				}
				_movePos[t->getID()] = t->getLocation();
				gettimeofday(&_moveTime[t->getID()], NULL);
			}

			// Multi process
			if (_state[0] == MultiTouchListener::State::multi)
			{
				struct timeval time;
				gettimeofday(&time, NULL);
				auto diff = (_movePos[0] + _movePos[1]) / 2 - _multiPos;
				auto diff_time = (float)(time.tv_sec - _multiTime.tv_sec) + (float)(time.tv_usec - _multiTime.tv_usec) / 1000000;
				_multiPos = (_movePos[0] + _movePos[1]) / 2;
				_multiTime = time;
				if (onSwipe && (!onSwipeCheck || onSwipeCheck(_multiPos, diff, diff_time)))
					onSwipe(_multiPos, diff, diff_time);
				auto diff_multi = (_movePos[0] - _movePos[1]).lengthSquared();
				if (diff_multi > _multiDiff && pinchOut)
					pinchOut(_multiPos, diff_multi / _multiDiff);
				else if (diff_multi < _multiDiff && pinchIn)
					pinchIn(_multiPos, diff_multi / _multiDiff);
				_multiDiff = diff_multi;
			}
		};
		onTouchesCancelled = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			_numberOfTouch -= touches.size();
			if (_numberOfTouch < 0)
				_numberOfTouch = 0;
			if (_numberOfTouch == 0)
				cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
		};
		onTouchesEnded = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			// count down
			_numberOfTouch -= touches.size();
			if (_numberOfTouch < 0)
			{
				_numberOfTouch = 0;
				return;
			}
			for (cocos2d::Touch* t : touches)
			{
				// The third doesn't correspond
				if (t->getID() > 1)
					return;
//				CCLOG("end %d is num : %d", _numberOfTouch, t->getID());
				struct timeval time;
				gettimeofday(&time, NULL);
				auto diff_time = (float)(time.tv_sec - _startTime[0].tv_sec) + (float)(time.tv_usec - _startTime[0].tv_usec) / 1000000;
				auto locale = t->getLocation();
				Sequence* action;
				//
				switch (_state[t->getID()])
				{
				case MultiTouchListener::State::check:
				case MultiTouchListener::State::tap:
					if (Director::getInstance()->getRunningScene()->getActionByTag(1000))
					{
						Director::getInstance()->getRunningScene()->stopActionByTag(1000);
						if (onDoubleTap)
							onDoubleTap(t->getLocation());
						break;
					}
					action = Sequence::create(
						DelayTime::create(getDoubleTapThreshold()),
						CallFunc::create([this, locale] {
							if (onTap)
								onTap(locale);
						}),
					NULL);
					action->setTag(1000);
					Director::getInstance()->getRunningScene()->runAction(action);
					break;
				case MultiTouchListener::State::longtap:
					if (onLongTapEnd) onLongTapEnd(t->getLocation());
					break;
				case MultiTouchListener::State::swipe:
//					CCLOG("Time %f", diff_time);
					if (diff_time < _flickThreshold)
						if (onFlick && (!onFlickCheck || onFlickCheck(t->getLocation(), t->getLocation() - _startPos[t->getID()], diff_time)))
							onFlick(t->getLocation(), t->getLocation() - _startPos[t->getID()], diff_time);
					break;
				case MultiTouchListener::State::multi:
					break;
				}
			}
			if (_numberOfTouch == 0)
				cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
			else if (_numberOfTouch == 1)
				_state[0] = _state[1] = MultiTouchListener::State::swipe;
		};
		return true;
	};

	/*
	 * Check long tap
	 */
	void update(float t)
	{
		struct timeval time;
		gettimeofday(&time, NULL);
		auto diff_time = (float)(time.tv_sec - _startTime[0].tv_sec) + (float)(time.tv_usec - _startTime[0].tv_usec) / 1000000;
		switch (_state[0])
		{
		case MultiTouchListener::State::check:
		case MultiTouchListener::State::tap:
			if (diff_time > _longTapThreshold)
			{
				if (onLongTapBegan) onLongTapBegan(_startPos[0]);
				_state[0] = MultiTouchListener::State::longtap;
				cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
			}
			break;
		case MultiTouchListener::State::swipe:
		case MultiTouchListener::State::multi:
			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
			break;
		}
	};
public:
	CREATE_FUNC(MultiTouchListener);
	CC_SYNTHESIZE(float, _longTapThreshold, LongTapThreshold);
	CC_SYNTHESIZE(float, _doubleTapThreshold, DoubleTapThreshold);
	CC_SYNTHESIZE(float, _swipeThreshold, SwipeThreshold);
	CC_SYNTHESIZE(float, _flickThreshold, FlickThreshold);

	std::function<void(cocos2d::Vec2)> onTouchBegan;
	std::function<void(cocos2d::Vec2)> onTap;
	std::function<void(cocos2d::Vec2)> onLongTapBegan;
	std::function<void(cocos2d::Vec2)> onLongTapEnd;
	std::function<void(cocos2d::Vec2)> onDoubleTap;
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipe;
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onFlick;
	std::function<void(cocos2d::Vec2, float)> pinchIn;
	std::function<void(cocos2d::Vec2, float)> pinchOut;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipeCheck;
	std::function<bool(cocos2d::Vec2, cocos2d::Vec2, float)> onFlickCheck;
};

/*
* Single touch event listener
*/
class SingleTouchListener : public cocos2d::EventListenerTouchOneByOne
{
public:
	enum class State
	{
		check, tap, swipe, longtap
	};
private:
	using cocos2d::EventListenerTouchOneByOne::onTouchBegan;
	using cocos2d::EventListenerTouchOneByOne::onTouchMoved;
	using cocos2d::EventListenerTouchOneByOne::onTouchEnded;
	using cocos2d::EventListenerTouchOneByOne::onTouchCancelled;

	cocos2d::Vec2 _startPos;
	struct timeval _startTime;
	cocos2d::Vec2 _movePos;
	struct timeval _moveTime;
	State _state;
protected:
	SingleTouchListener()
		: _longTapThreshold(DefaultLongTapThreshold), _doubleTapThreshold(DefaultDoubleTapThreshold)
		, _swipeThreshold(DefaultSwipeThresholdDistance), _flickThreshold(DefaultFlickThreshold)
		, onTap(nullptr), onLongTapBegan(nullptr), onLongTapEnd(nullptr), onDoubleTap(nullptr)
		, onSwipe(nullptr)
		, onTouchBeganChecking(nullptr), onTouchEndedChecking(nullptr)
	{
	};
	~SingleTouchListener() 
	{
		_longTapThreshold = _doubleTapThreshold = _swipeThreshold = _flickThreshold = 0;
		onTap = onLongTapEnd = onDoubleTap = nullptr;
		onLongTapBegan = nullptr;
		onSwipe = nullptr;
		onTouchBeganChecking = nullptr;
		onTouchEndedChecking = nullptr;
	};
	/*
	 * Initialize
	 * Parse listener process
	 */
	virtual bool init()
	{
		if (!EventListenerTouchOneByOne::init())
			return false;

		onTouchBegan = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			// Checking
			if (onTouchBeganChecking && !onTouchBeganChecking(touch, event))
				return false;

			// Initialize
			_startPos = touch->getLocation();
			gettimeofday(&_startTime, NULL);
			_movePos = touch->getLocation();
			gettimeofday(&_moveTime, NULL);
			_state = SingleTouchListener::State::check;
			// Start process of checking long tap
			cocos2d::Director::getInstance()->getScheduler()->schedule(schedule_selector(SingleTouchListener::update), this, 0.05f, false);
			return true;
		};
		onTouchMoved = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			// Move check
			if (_state != SingleTouchListener::State::swipe &&
				_state != SingleTouchListener::State::longtap)
			{
				auto diff = (touch->getLocation() - _movePos).lengthSquared();
				if (diff > _swipeThreshold)
					_state = SingleTouchListener::State::swipe;
				else
					_state = SingleTouchListener::State::tap;
			}
			// Move process
			if (_state == SingleTouchListener::State::swipe)
			{
				struct timeval time;
				gettimeofday(&time, NULL);
				auto diff = (touch->getLocation() - _movePos);
				auto diff_time = (float)(time.tv_sec - _moveTime.tv_sec) + (float)(time.tv_usec - _moveTime.tv_usec) / 1000000;
				if (onSwipe) onSwipe(touch->getLocation(), diff, diff_time);
			}
			_movePos = touch->getLocation();
			gettimeofday(&_moveTime, NULL);
		};
		onTouchCancelled = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(SingleTouchListener::update), this);
		};
		onTouchEnded = [this](cocos2d::Touch *touch, cocos2d::Event *event)
		{
			struct timeval time;
			gettimeofday(&time, NULL);
			auto diff_time = (float)(time.tv_sec - _startTime.tv_sec) + (float)(time.tv_usec - _startTime.tv_usec) / 1000000;
			//
			switch (_state)
			{
			case SingleTouchListener::State::check:
			case SingleTouchListener::State::tap:
				if (onTap) onTap(touch, event);
				break;
			case SingleTouchListener::State::longtap:
				if (onLongTapEnd) onLongTapEnd(touch, event);
				break;
			case SingleTouchListener::State::swipe:
				if (diff_time < _flickThreshold)
					if (onFlick) onFlick(touch->getLocation(), touch->getLocation() - _startPos, diff_time);
				break;
			}
			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(SingleTouchListener::update), this);

			if (onTouchEndedChecking) onTouchEndedChecking(touch, event);
		};
		return true;
	};

	/*
	 * Check long tap
	 */
	void update(float)
	{
		struct timeval time;
		gettimeofday(&time, NULL);
		auto diff_time = (float)(time.tv_sec - _startTime.tv_sec) + (float)(time.tv_usec - _startTime.tv_usec) / 1000000;
		switch (_state)
		{
		case SingleTouchListener::State::check:
		case SingleTouchListener::State::tap:
			if (diff_time > _longTapThreshold)
			{
				if (onLongTapBegan) onLongTapBegan(_startPos);
				_state = SingleTouchListener::State::longtap;
				cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(SingleTouchListener::update), this);
			}
			break;
		case SingleTouchListener::State::swipe:
			cocos2d::Director::getInstance()->getScheduler()->unschedule(schedule_selector(SingleTouchListener::update), this);
			break;
		}
	};
public:
	CREATE_FUNC(SingleTouchListener);
	CC_SYNTHESIZE(float, _longTapThreshold, LongTapThreshold);
	CC_SYNTHESIZE(float, _doubleTapThreshold, DoubleTapThreshold);
	CC_SYNTHESIZE(float, _swipeThreshold, SwipeThreshold);
	CC_SYNTHESIZE(float, _flickThreshold, FlickThreshold);

	std::function<bool(cocos2d::Touch*, cocos2d::Event*)> onTouchBeganChecking;
	std::function<void(cocos2d::Touch*, cocos2d::Event*)> onTap;
	std::function<void(cocos2d::Vec2)> onLongTapBegan;
	std::function<void(cocos2d::Touch*, cocos2d::Event*)> onLongTapEnd;
	std::function<void(cocos2d::Touch*, cocos2d::Event*)> onDoubleTap;
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipe;
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onFlick;
	std::function<void(cocos2d::Touch*, cocos2d::Event*)> onTouchEndedChecking;
};

/*
 * Mouse listener
 * Parse mouseListener to touchListener
 */
class MultiMouseListener : public cocos2d::EventListenerMouse
{
protected:
	MultiMouseListener()
		: _touchListener(nullptr)
		, _scrollMagnification(DefaultScrollMagnification)
	{
	};
	~MultiMouseListener()
	{
		CC_SAFE_RELEASE_NULL(_touchListener);
		_scrollMagnification = 0;
	};
public:
	CC_SYNTHESIZE_RETAIN(MultiTouchListener*, _touchListener, TouchListener);
	CC_SYNTHESIZE(float, _scrollMagnification, ScrollMagnification);
	/*
	 * Create mouse listener by touch listener
	 */
	static MultiMouseListener* create(MultiTouchListener *lis)
	{
		MultiMouseListener *pRet = new(std::nothrow) MultiMouseListener();
		if (pRet && pRet->init(lis))
		{
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
	/*
	 * Initialize
	 */
	virtual bool init(MultiTouchListener *lis)
	{
		if (!EventListenerMouse::init())
			return false;

		setTouchListener(lis);

		// Scroll event equals pinch in or pinch out
		onMouseScroll = [this] (cocos2d::EventMouse *event)
		{
			auto scroll = event->getScrollY() * getScrollMagnification();
			scroll *= (scroll < 0) ? -1 : 1;
//			auto scroll = cocos2d::Vec2(event->getScrollX(), event->getScrollY()).length() * getScrollMagnification();
			if (scroll < 1.0)
				return;

			if (event->getScrollY() > 0)
			{
				if (getTouchListener()->pinchIn)	getTouchListener()->pinchIn(cocos2d::Vec2(event->getCursorX(), event->getCursorY()), scroll);
			}
			else
			{
				if(getTouchListener()->pinchOut)	getTouchListener()->pinchOut(cocos2d::Vec2(event->getCursorX(), event->getCursorY()), 1.0f / scroll);
			}
		};
		return true;
	};
};

#endif // __MULTI_LISTENER_H__