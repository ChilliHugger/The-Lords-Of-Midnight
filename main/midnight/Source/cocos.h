//
//  cocos.h
//  midnight
//
//  Created by Chris Wild on 02/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef cocos_h
#define cocos_h

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#if !defined (_USE_COCOS_)
#if !defined (_USING_AXMOL_)
    #define _USING_AXMOL_
#endif
#endif


#define ALPHA(x)                (f32)((x)*255.0)
#define FROM_ALPHA(x)           (f32)((x)/255.0)

#if !defined(USING_NS_CC)
#define USING_NS_CC             using namespace cocos2d
#endif

#define USING_NS_CC_UI          using namespace cocos2d::ui

#if defined(_USING_AXMOL_)
#define RUN_ON_UI_THREAD        ax::Director::getInstance()->getScheduler()->runOnAxmolThread
using Ref = ax::Ref;
#else
#define RUN_ON_UI_THREAD        cocos::Director::getInstance()->getScheduler()->performFunctionInCocosThread
#endif

#define RUN_EVENT(x)            RUN_ON_UI_THREAD([=](){ x; } )

#endif /* cocos_h */
