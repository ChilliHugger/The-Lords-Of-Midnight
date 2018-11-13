//
//  uielement.h
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

class uisinglelord : public uilordselect
{
public:
    CREATE_FUNC(uisinglelord);
    virtual bool init() override;
    static uisinglelord* createWithLord ( mxid characterid );

    bool setLord( mxid characterid );
    
protected:
    uisinglelord();
};
