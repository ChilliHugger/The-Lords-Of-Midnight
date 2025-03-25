/*
 *  panel_map_overview.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "../ui/uipanel.h"

class panel_map_overview : public uipanel
{
    
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_map_overview);
    
protected:
    
    void OnNotification( Ref* sender ) override;
    
#if defined(_FOCUS_ENABLED_)
    virtual std::vector<layoutid_t> getFocusControls() const override;
#endif


};
