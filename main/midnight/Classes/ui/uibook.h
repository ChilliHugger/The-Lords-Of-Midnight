//
//  uibook.h
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//

#pragma once

#include "uielement.h"
#include "../system/storymanager.h"

class uibook : public cocos2d::Node 
{
public:
    CREATE_FUNC(uibook);
    static uibook* createWithChapter(storyheader_t* story);
    
protected:
    bool initWithChapter(storyheader_t* story);
};
