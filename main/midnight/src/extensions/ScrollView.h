//
//  ScrollView.h
//  midnight
//
//  Created by Chris Wild on 15/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "../cocos.h"

namespace extensions {

    class ScrollView : public cocos2d::ui::ScrollView
    {
        using Vec2 = cocos2d::Vec2;
        
    public:
        static ScrollView * create();
        
        void scrollToPosition(const Vec2& des, float timeInSec, bool attenuated);
        
    };

}
