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

#if defined (NS_AX)
#define _USING_AXMOL_
#else
#define _USING_COCOS_
#endif


#define ALPHA(x)                (f32)((x)*255.0)
#define FROM_ALPHA(x)           (f32)((x)/255.0)

#define USING_NS_CC_UI          using namespace cocos2d::ui

#if defined(_USING_AXMOL_)
#define RUN_ON_UI_THREAD        ax::Director::getInstance()->getScheduler()->runOnAxmolThread
#else
#define RUN_ON_UI_THREAD        cocos::Director::getInstance()->getScheduler()->performFunctionInCocosThread
#endif

#define RUN_EVENT(x)            RUN_ON_UI_THREAD([=](){ x; } )


#endif /* cocos_h */
