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

#define ALPHA(x)        (f32)((x)*255)
#define USING_NS_CC_UI  using namespace cocos2d::ui

#define RUN_ON_UI_THREAD		cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread
#define RUN_EVENT(x)			RUN_ON_UI_THREAD([=](){ x; } )


#endif /* cocos_h */
