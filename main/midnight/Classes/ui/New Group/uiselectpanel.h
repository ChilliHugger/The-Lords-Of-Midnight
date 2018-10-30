//
//  uiselectpanel.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../uielement.h"



class uiselectpanel : public uielement
{
public:
    CREATE_FUNC(uiselectpanel);
    virtual bool init() override;
    
protected:
    uiselectpanel();
};
