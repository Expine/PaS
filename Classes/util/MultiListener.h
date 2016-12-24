#ifndef __MULTI_LISTENER_H__
#define __MULTI_LISTENER_H__

#include "cocos2d.h"

#include <time.h>

constexpr float DefaultLongTapThreshold = 0.1f;
constexpr float DefaultDoubleTapThreshold = 0.1f;
constexpr float DefaultSwipeThresholdDistance = 10.0f;
constexpr float DefaultAllowableRange = 5.0f;

constexpr float DefaultScrollMagnification = 1.2f;

class MultiTouchListener : public cocos2d::EventListenerTouchAllAtOnce
{
public:
	enum class State 
	{
		check, tap, swipe, multi, longtap
	};

private:
	using EventListenerTouchAllAtOnce::LISTENER_ID;
	using EventListenerTouchAllAtOnce::onTouchesBegan;
	using EventListenerTouchAllAtOnce::onTouchesMoved;
	using EventListenerTouchAllAtOnce::onTouchesEnded;
	using EventListenerTouchAllAtOnce::onTouchesCancelled;

	int _numberOfTouch;
	std::vector<cocos2d::Vec2> _startPos;
	std::vector<timeval> _startTime;
	std::vector<cocos2d::Vec2> _movePos;
	std::vector<timeval> _moveTime;
	timeval _multiTime;
	Vec2 _multiPos;
	float _multiDiff;
	std::vector<State> _state;

protected:
	MultiTouchListener()
		: _numberOfTouch(0)
		, _longTapThreshold(DefaultLongTapThreshold)
		, _doubleTapThreshold(DefaultDoubleTapThreshold)
		, _swipeThreshold(DefaultSwipeThresholdDistance)
		, _allowableRange(DefaultAllowableRange)
		, onTap(nullptr)
		, onLongTapBegan(nullptr)
		, onLongTapEnd(nullptr)
		, onDoubleTap(nullptr)
		, onSwipe(nullptr)
		, pinchIn(nullptr)
		, pinchOut(nullptr)
	{
		_startPos.resize(5);
		_startTime.resize(5);
		_movePos.resize(5);
		_moveTime.resize(5);
		_state.resize(5);
	};
	~MultiTouchListener() {};

	virtual bool init()
	{
		if (!EventListenerTouchAllAtOnce::init())
			return false;

		onTouchesBegan = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			//カウントアップ
			_numberOfTouch += touches.size();
			for (cocos2d::Touch* t : touches)
			{
				// 三本目以降は未対応
				if (t->getID() > 1)
					return;
				// 初期化
				_startPos[t->getID()] = t->getLocation();
				gettimeofday(&_startTime[t->getID()], NULL);
				_movePos[t->getID()] = t->getLocation();
				gettimeofday(&_moveTime[t->getID()], NULL);
				_state[t->getID()] = MultiTouchListener::State::check;
//				CCLOG("start (%f, %f)", t->getLocation().x, t->getLocation().y);
//				CCLOG("start %d is num : %d", _numberOfTouch, t->getID());
//				CCLOG("Time is %ld - %ld", _startTime[t->getID()].tv_sec, _startTime[t->getID()].tv_usec);
				Director::getInstance()->getScheduler()->schedule(schedule_selector(MultiTouchListener::update), this, 0.05f, false);
			}
			// マルチタッチの場合
			if (_numberOfTouch > 1)
			{
				for (int i = 0; i < _numberOfTouch; i++)
					_state[i] = MultiTouchListener::State::multi;
				gettimeofday(&_multiTime, NULL);
				_multiPos = (_movePos[0] + _movePos[1]) / 2;
				_multiDiff = (_movePos[0] - _movePos[1]).lengthSquared();
			}

		};
		onTouchesMoved = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			for (cocos2d::Touch* t : touches)
			{
				// 三本目以降は未対応
				if (t->getID() > 1)
					return;
				// Move check
				if (_state[t->getID()] != MultiTouchListener::State::swipe && 
					_state[t->getID()] != MultiTouchListener::State::multi &&
					_state[t->getID()] != MultiTouchListener::State::longtap)
				{
//					CCLOG("now (%f, %f)", t->getLocation().x, t->getLocation().y);
					auto diff = (t->getLocation() - _movePos[t->getID()]).lengthSquared();
//					CCLOG("check diff %f : %d", diff, t->getID());
					if (diff > _allowableRange)
						_state[t->getID()] = MultiTouchListener::State::swipe;
					else
						_state[t->getID()] = MultiTouchListener::State::tap;
				}

				// Move process
				if (_state[t->getID()] == MultiTouchListener::State::swipe)
				{
					timeval time;
					gettimeofday(&time, NULL);
					auto diff = (t->getLocation() - _movePos[t->getID()]);
					auto diff_time = (float)(time.tv_sec - _moveTime[t->getID()].tv_sec) + (float)(time.tv_usec - _moveTime[t->getID()].tv_usec) / 1000000;
					if (onSwipe) onSwipe(t->getLocation(), diff, diff_time);
//					CCLOG("move diff %f : %d", diff.lengthSquared(), t->getID());
				}
				_movePos[t->getID()] = t->getLocation();
				gettimeofday(&_startTime[t->getID()], NULL);
			}

			// Multi process
			if (_state[0] == MultiTouchListener::State::multi)
			{
				timeval time;
				gettimeofday(&time, NULL);
				auto diff = (_movePos[0] + _movePos[1]) / 2 - _multiPos;
				auto diff_time = (float)(time.tv_sec - _multiTime.tv_sec) + (float)(time.tv_usec - _multiTime.tv_usec) / 1000000;
				_multiPos = (_movePos[0] + _movePos[1]) / 2;
				_multiTime = time;
				if (onSwipe) onSwipe(_multiPos, diff, diff_time);
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
		};
		onTouchesEnded = [this](const std::vector<cocos2d::Touch*> &touches, cocos2d::Event* event)
		{
			//カウントを減らす
			_numberOfTouch -= touches.size();
			for (cocos2d::Touch* t : touches)
			{
				// 三本目以降は未対応
				if (t->getID() > 1)
					return;
				CCLOG("end %d is num : %d", _numberOfTouch, t->getID());
				switch (_state[t->getID()])
				{
				case MultiTouchListener::State::check:
				case MultiTouchListener::State::tap:
					if (onTap) onTap(t->getLocation());
					break;
				case MultiTouchListener::State::longtap:
					if (onLongTapEnd) onLongTapEnd(t->getLocation());
					break;
				case MultiTouchListener::State::swipe:
				case MultiTouchListener::State::multi:
					break;
				}
			}
			if (_numberOfTouch == 0)
				Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
		};
		return true;
	};
	void update(float)
	{
		timeval time;
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
				Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
			}
			break;
		case MultiTouchListener::State::swipe:
		case MultiTouchListener::State::multi:
			Director::getInstance()->getScheduler()->unschedule(schedule_selector(MultiTouchListener::update), this);
			break;
		}
	};
public:
	CREATE_FUNC(MultiTouchListener);
	CC_SYNTHESIZE(float, _longTapThreshold, LongTapThreshold);
	CC_SYNTHESIZE(float, _doubleTapThreshold, DoubleTapThreshold);
	CC_SYNTHESIZE(float, _swipeThreshold, SwipeThreshold);
	CC_SYNTHESIZE(float, _allowableRange, AllowableRange);

	std::function<void(cocos2d::Vec2)> onTap;
	std::function<void(cocos2d::Vec2)> onLongTapBegan;
	std::function<void(cocos2d::Vec2)> onLongTapEnd;
	std::function<void(cocos2d::Vec2)> onDoubleTap;
	std::function<void(cocos2d::Vec2, cocos2d::Vec2, float)> onSwipe;
	std::function<void(cocos2d::Vec2, float)> pinchIn;
	std::function<void(cocos2d::Vec2, float)> pinchOut;
};


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
	};
public:
	CC_SYNTHESIZE_RETAIN(MultiTouchListener*, _touchListener, TouchListener);
	CC_SYNTHESIZE(float, _scrollMagnification, ScrollMagnification);
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
	virtual bool init(MultiTouchListener *lis)
	{
		if (!EventListenerMouse::init())
			return false;

		setTouchListener(lis);

		onMouseScroll = [this] (cocos2d::EventMouse *event)
		{
			CCLOG("SCROLL (%f, %f)", event->getScrollX(), event->getScrollY());
			CCLOG("POS (%f, %f)", event->getLocation().x, event->getLocation().y);
			auto scroll = event->getScrollY() * getScrollMagnification();
			scroll *= (scroll < 0) ? -1 : 1;
//			auto scroll = cocos2d::Vec2(event->getScrollX(), event->getScrollY()).length() * getScrollMagnification();
			if (scroll < 1.0)
				return true;

			if (event->getScrollY() > 0)
			{
				if (getTouchListener()->pinchIn)	getTouchListener()->pinchIn(Vec2(event->getCursorX(), event->getCursorY()), scroll);
			}
			else
			{
				if(getTouchListener()->pinchOut)	getTouchListener()->pinchOut(Vec2(event->getCursorX(), event->getCursorY()), 1.0f / scroll);
			}
		};

	
		return true;
	};
};

#endif // __MULTI_LISTENER_H__