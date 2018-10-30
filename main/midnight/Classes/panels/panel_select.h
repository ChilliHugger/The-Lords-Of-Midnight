/*
 *  panel_select.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "uipanel.h"
#include "../ui/uifilterbutton.h"
#include "../frontend/layout_id.h"
#include "../system/configmanager.h"

class panel_select : public uipanel
{
	
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_select);
    
protected:
    uifilterbutton* createFilterButton( layoutid_t id, s32 y, const std::string& image, select_filters flag );
    
    virtual void OnNotification( Ref* sender ) override;
    void updateFilters( void );

};
