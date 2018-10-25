/*
 *  panel_night.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "uipanel.h"
#include "../tme_interface.h"

class panel_night : public uipanel
                    , protected NightNotificationDelegate
{
	
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_night);
    
protected:
    
    virtual void OnShown() override;
    virtual void OnNotification ( Ref* sender ) override ;
    virtual void OnNightNotification ( tme::callback_t* event ) override;
    
    void setNightText( const std::string& text );
    
private:
    cocos2d::Label*     lblDescription;
    cocos2d::ui::ScrollView* scrollView;
};
