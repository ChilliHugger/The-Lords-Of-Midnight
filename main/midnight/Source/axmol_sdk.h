//
//  axmol_sdk.h
//  midnight
//
//  Created by Chris Wild on 02/05/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef axmol_sdk
#define axmol_sdk

#include "axmol.h"
#include "ui/axmol-ui.h"

#define _USING_AXMOL_

#define ALPHA(x)                (f32)((x)*255.0)
#define FROM_ALPHA(x)           (f32)((x)/255.0)

#if !defined(USING_NS_AX)
#define USING_NS_AX             using namespace ax
#endif

#if !defined(USING_NS_AX_UI)
#define USING_NS_AX_UI          using namespace ax::ui
#endif

#define RUN_ON_UI_THREAD        ax::Director::getInstance()->getScheduler()->runOnAxmolThread

using Ref = ax::Object;

#define RUN_EVENT(x)            RUN_ON_UI_THREAD([=](){ x; } )

#endif /* axmol_sdk */
