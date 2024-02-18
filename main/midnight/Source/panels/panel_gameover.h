/*
 *  panel_gameover.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "../ui/uipanel.h"

class panel_gameover : public uipanel
{
    
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_gameover);
    
protected:
    void OnNotification( Ref* sender ) override;
    void OnShown() override;
};
