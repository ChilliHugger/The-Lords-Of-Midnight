//
//  CreditsScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef CreditsScene_h
#define CreditsScene_h

#include "DefaultScene.h"

class Credits : public DefaultScene
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(Credits);

};



#endif /* CreditsScene_h */
