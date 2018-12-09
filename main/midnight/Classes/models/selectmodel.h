//
//  selectmodel.h
//  midnight
//
//  Created by Chris Wild on 09/12/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#pragma once
#include "panelmodel.h"

#include "../tme_interface.h"

class selectmodel : public panelmodel
{
public:
    void serialize( u32 version, lib::archive& ar ) override;
    virtual void setDefaults() override;
    
public:
    c_mxid characters;
    flags32 filters;
    
    
};
