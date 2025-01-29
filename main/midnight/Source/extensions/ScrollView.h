//
//  ScrollView.h
//  midnight
//
//  Created by Chris Wild on 15/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "../axmol_sdk.h"

namespace extensions {

    class ScrollView : public ax::ui::ScrollView
    {
        using Vec2 = ax::Vec2;
        
    public:
        static ScrollView * create();
        
        void scrollToPosition(const Vec2& des, float timeInSec, bool attenuated);
        
    };
    

    class ScrollingMenu : public ax::Menu
    {
    public:
        static ScrollingMenu * create();
        
    protected:
        void enableScrolling();
    };

}
