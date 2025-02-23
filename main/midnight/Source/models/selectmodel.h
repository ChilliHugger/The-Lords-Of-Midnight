//
//  selectmodel.h
//  midnight
//
//  Created by Chris Wild on 09/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "panelmodel.h"

#include "../tme/tme_interface.h"


enum class select_filters : u32 {
    show_dawn    = MXBIT(0),
    show_night   = MXBIT(1),
    show_battle  = MXBIT(2),
    show_dead    = MXBIT(3),
    show_current = MXBIT(4),
    show_intunnel= MXBIT(5),
    all          = show_dawn
                 | show_night
                 | show_battle
                 | show_dead
                 | show_current
                 | show_intunnel
};


class selectmodel : public panelmodel
{
public:
    void serialize( u32 version, lib::archive& ar ) override;
    virtual void setDefaults() override;
    void updateCharacters();

public:
    c_mxid          characters;
    eflags<select_filters,u32>         filters;
    tme::loc_t      loc;
    s32             page;
    bool            upgraded;
};
