/*
 *  panel_think.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "../ui/uipanel.h"
#include "../ui/uithinkpage.h"
#include "../frontend/layout_id.h"

class panel_think : public uipanel
{
    using PageView = cocos2d::ui::PageView;
    using Color3B = cocos2d::Color3B;
    template<class T> using Vector = cocos2d::Vector<T>;
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_think);
    
    void setTargetObject(mxid id);
    
protected:
    
    virtual void OnNotification( Ref* sender ) override;
    virtual void setObject(mxid id) override;

    void createPageView();
    void setupPages();
    void addPage( mxid id );
    
    void enableButtons();
    void enableButton(layoutid_t id, bool enabled);
    void showButton(layoutid_t id, bool enabled);
    void tintButton(layoutid_t id, Color3B colour);

protected:
    flags32                 flags;
    s64                     currentPage;
    mxid                    id;
    mxid                    objectId;
    int                     current_tab;
    PageView*               pageView;
    Vector<uithinkpage*>    pages;
};
