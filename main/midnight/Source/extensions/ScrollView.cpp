//
//  ScrollView.c
//  midnight
//
//  Created by Chris Wild on 15/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "ScrollView.h"

USING_NS_AX;

namespace extensions {
    
    ScrollView * ScrollView::create()
    {
        ScrollView *ret = new (std::nothrow) ScrollView();
        if (ret->init())
        {
            ret->autorelease();
            return ret;
        }
        AX_SAFE_DELETE(ret);
        return nullptr;
    }
    
    void ScrollView::scrollToPosition(const Vec2& des, float timeInSec, bool attenuated)
    {
        startAutoScrollToDestination(des, timeInSec, attenuated);
    }
    
    ScrollingMenu* ScrollingMenu::create()
    {
        Vector<MenuItem*> items;
        auto ret = new (std::nothrow) ScrollingMenu();
        if (ret && ret->initWithArray(items))
        {
            ret->autorelease();
            ret->enableScrolling();
        }
        else
        {
            AX_SAFE_DELETE(ret);
        }
        
        return ret;
    }
    
    void ScrollingMenu::enableScrolling()
    {
        _eventDispatcher->removeEventListenersForTarget(this);
    
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->setSwallowTouches(false);
        
        touchListener->onTouchBegan = AX_CALLBACK_2(Menu::onTouchBegan, this);
        touchListener->onTouchMoved = AX_CALLBACK_2(Menu::onTouchMoved, this);
        touchListener->onTouchEnded = AX_CALLBACK_2(Menu::onTouchEnded, this);
        touchListener->onTouchCancelled = AX_CALLBACK_2(Menu::onTouchCancelled, this);
        
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    }
    
    
}
