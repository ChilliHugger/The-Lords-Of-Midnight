/*
 *  panel_select.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "../ui/uipanel.h"
#include "../ui/uifilterbutton.h"
#include "../frontend/layout_id.h"
#include "../system/configmanager.h"

FORWARD_REFERENCE(uilordselect);

class panel_select : public uipanel
{
	
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_select);
    
protected:
    uifilterbutton* createFilterButton( layoutid_t id, s32 y, const std::string& image, select_filters flag );
    
    virtual void OnNotification( Ref* sender ) override;
    

    void getCharacters();
    cocos2d::Vec2 calcGridLocation ( u32 index );
    
    void updateFilters();
    void applyFilters ( uilordselect* e, character& c );
    
private:
    s32            SELECT_GRID_X ;
    s32            SELECT_GRID_Y ;
    s32            SELECT_GRID_X_LEADING ;
    s32            SELECT_GRID_Y_LEADING ;
    s32            MAX_COLUMNS ;
    s32            START_X ;
    s32            START_Y ;
    c_mxid         characters;
    tme::loc_t     loc;
    
    cocos2d::ui::ScrollView* scrollView;

};
