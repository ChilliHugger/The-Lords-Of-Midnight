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
#include "../../system/tmemanager.h"

#define SELECT_ELEMENT_WIDTH   RES(128)
#define SELECT_ELEMENT_HEIGHT  RES(110)
#define SELECT_IMAGE_HEIGHT    RES(72)


class uisinglelord : public uilordselect
{
public:
    CREATE_FUNC(uisinglelord);
    virtual bool init() override;
    static uisinglelord* createWithLord ( mxid characterid );

    bool setLord( mxid characterid );
    
protected:
    uisinglelord();
    
    void updateStatus(character& c);
    
    cocos2d::Sprite* getStatusImage();
    cocos2d::Sprite* getFaceImage();

    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    virtual void onPressStateChangedToDisabled() override;
    
protected:
    flags16         status;
    char_data_t*    userdata;
    Node*           buttonNode;
};
