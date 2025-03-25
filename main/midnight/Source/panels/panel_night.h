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
    using Label = ax::Label;
    using ScrollView = ax::ui::ScrollView;
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_night);
    
protected:
    
    virtual void OnShown() override;
    virtual void OnNotification ( Ref* sender ) override ;
    virtual void OnNightNotification ( tme::callback_t* event ) override;
    
    virtual void update(float delta) override;
    
    void setNightText( const std::string& text );
    
#if defined(_FOCUS_ENABLED_)
    virtual std::vector<layoutid_t> getFocusControls() const override;
#endif

private:
    Label*                          lblDescription;
    ScrollView*                     scrollView;
    std::string                     message;
    bool                            complete;
    m_gameover_t                    gameover;
    mutable std::recursive_mutex    _mutex;
};
