/*
 *  panel_night.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "../ui/uipanel.h"
#include "../tme/tme_interface.h"

class panel_night :
    public uipanel,
    protected NightNotificationDelegate
{
protected:
    using Label = cocos2d::Label;
    using ScrollView = cocos2d::ui::ScrollView;
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_night);
    
protected:
    
    virtual void OnShown() override;
    virtual void OnNotification ( Ref* sender ) override ;
    virtual void OnNightNotification ( tme::callback_t* event ) override;
    
    virtual void update(float delta) override;
    
    void setNightText( const std::string& text );
    
private:
    Label*                          lblDescription;
    ScrollView*                     scrollView;
    std::string                     message;
    bool                            complete;
    m_gameover_t                    gameover;
    mutable std::recursive_mutex    _mutex;
};
