/*
 *  panel_think.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "uipanel.h"
#include "uithinkpage.h"
#include "../frontend/layout_id.h"

class panel_think : public uipanel
{
	
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_think);
    
    void setTargetObject(mxid id);
    
protected:
    
    virtual void OnNotification( Ref* sender ) override;
    virtual void setObject(mxid id) override;

    void setupPages();
    void addPage( mxid id );
    
    void enableButtons();
    void enableButton(layoutid_t id, bool enabled);
    void showButton(layoutid_t id, bool enabled);
    void tintButton(layoutid_t id,cocos2d::Color3B colour);

protected:
    flags32         flags;
    //s32             page;
    //c_mxid          pages;
    cocos2d::Vector<uithinkpage*>    pages;
    s64             currentPage;
    mxid            id;
    mxid            objectId;
    
    cocos2d::ui::PageView*       pageView;
    int             current_tab;
};
