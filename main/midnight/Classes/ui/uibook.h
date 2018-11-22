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

class uibook : public cocos2d::Node /*cocos2d::ui::Scale9Sprite*/
{
public:
    CREATE_FUNC(uibook);
    static uibook* createWithChapter(storyheader_t* story);
    
protected:
    bool initWithChapter(storyheader_t* story);
};
