//
//  uilordselect.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../uielement.h"


#include "../../tme_interface.h"

enum LORD_STATUS {
    status_normal=0,
    status_dawn     = MXBIT(1),
    status_night    = MXBIT(2),
    status_inbattle = MXBIT(3),
    status_dead     = MXBIT(4),
    status_tunnel   = MXBIT(5),
};


class uilordselect : public cocos2d::ui::Widget, public uidragmoveelement
{
public:
    
    CREATE_FUNC(uilordselect);
    
    virtual bool init() override;

protected:
    uilordselect();
    void enableDragAndDrop();
    void addTouchListener();

    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event* ) override;
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event* ) override;
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event* )  override;
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event* ) override;
    
    cocos2d::Vec2 mouse_down_pos;
    cocos2d::Vec2 mouse_last_position;;
    
};
