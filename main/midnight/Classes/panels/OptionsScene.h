//
//  OptionsScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef OptionsScene_h
#define OptionsScene_h

#include "DefaultScene.h"

class Options : public DefaultScene
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(Options);
    
};



#endif /* OptionsScene_hpp */
