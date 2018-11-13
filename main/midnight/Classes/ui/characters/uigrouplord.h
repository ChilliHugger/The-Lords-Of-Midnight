//
//  uigrouplord.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../uielement.h"
#include "uilordselect.h"

class uigrouplord : public uilordselect
{
public:
    CREATE_FUNC(uigrouplord);
    virtual bool init() override;
    
protected:
    uigrouplord();
};
