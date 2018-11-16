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
    uibook(storyheader_t* story);
};
