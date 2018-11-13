//
//  uilordselect.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../uielement.h"


class uilordselect : public uielement
{
public:
    
    CREATE_FUNC(uilordselect);
    
    virtual bool init() override;

protected:
    uilordselect();
};
