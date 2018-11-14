//
//  uielement.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "uielement.h"
#include "uihelper.h"

USING_NS_CC;
using namespace cocos2d::ui;

uielement::uielement()
{
}

uielement* uielement::create()
{
    uielement * ret = new (std::nothrow) uielement();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }
    return ret;
}

bool uielement::init()
{
    return Layer::init();
}
