//
//  ScrollView.h
//  midnight
//
//  Created by Chris Wild on 15/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "../cocos.h"
#include "../library/inc/mxtypes.h"

namespace extensions {

    class ScrollView : public cocos2d::ui::ScrollView
    {
        using Vec2 = cocos2d::Vec2;
        
    public:
        static ScrollView * create();
        
        ScrollView();
     
        ScrollView* enablePichZoom(bool enabled);
        
        
        void scrollToPosition(const Vec2& des, float timeInSec, bool attenuated);
        
        bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
        void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
        void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;
        void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unusedEvent) override;

    protected:
        void keepPreviousPointAtSameLocationAfterScale();

        
    private:
        f32 startScale;
        f32 currentScale;
        f32 scaleAdjust;
        bool isDragging;
        bool isPinchZoomEnabled;
        Vec2 originalOffsetPosition;
        Vec2 originalTouchPosition;
        
    };

}
