#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "DefaultScene.h"

class Splash : public DefaultScene
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(Splash);

};

#endif // __SPLASH_SCENE_H__
