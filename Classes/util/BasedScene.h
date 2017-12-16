#ifndef __BASED_SCENE_H__
#define __BASED_SCENE_H__

#include "cocos2d.h"

namespace NarUtils
{
	/*
	 * Generalized generation
	 */
	template<class BaseScene>
	struct create_func {
		/*
		 * Generate instance with initialization
		 */
		template<class... Args>
		static BaseScene* create(Args&&... args) {
			auto p = new BaseScene();
			if (p->init(std::forward<Args>(args)...)) {
				p->autorelease();
				return p;
			} else {
				delete p;
				return nullptr;
			}
		}

		/*
		 * Generate scene with instance
		 */
		template<class... Args>
		static cocos2d::Scene* createScene(Args&&... args) {
			auto scene = cocos2d::Scene::create();
			BaseScene* layer = BaseScene::create();
			scene->addChild(layer);
			return scene;
		}
	};

	/*
	 * Base scene
	 * Has functions of create and createScene
	 * Should keep constructor public
	 */
	template<class SceneName>
	class IBasedScene : public cocos2d::Layer, create_func<SceneName>
	{
	public:
		/*
		 * Initialize
		 */
		virtual bool init() = 0;
		using create_func<SceneName>::create;
		using create_func<SceneName>::createScene;
	};
}

#endif // __BASED_SCENE_H__