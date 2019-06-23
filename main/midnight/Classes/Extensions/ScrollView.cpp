//
//  ScrollView.c
//  midnight
//
//  Created by Chris Wild on 15/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "ScrollView.h"

USING_NS_CC;

namespace extensions {
    
    ScrollView * ScrollView::create()
    {
        ScrollView *ret = new (std::nothrow) ScrollView();
        if (ret->init())
        {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    
    void ScrollView::scrollToPosition(const Vec2& des, float timeInSec, bool attenuated)
    {
        startAutoScrollToDestination(des, timeInSec, attenuated);
    }
}
