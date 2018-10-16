//
//  uielement.cpp
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#include "uielement.h"


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
    return Node::init();
}
