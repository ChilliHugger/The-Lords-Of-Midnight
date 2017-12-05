//
//  OptionsScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef __PANEL_OPTIONS_H__
#define __PANEL_OPTIONS_H__

#include "uipanel.h"

class panel_options : public uipanel
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(panel_options);
    
};



#endif /* __PANEL_OPTIONS_H__ */
