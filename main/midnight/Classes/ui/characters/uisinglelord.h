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

#define SELECT_ELEMENT_WIDTH   DIS(128)
#define SELECT_ELEMENT_HEIGHT  DIS(128)
#define SELECT_IMAGE_HEIGHT    DIS(72)


class uisinglelord : public uilordselect
{
public:
    CREATE_FUNC(uisinglelord);
    virtual bool init() override;
    static uisinglelord* createWithLord ( mxid characterid );

    bool setLord( mxid characterid );
    void refreshStatus() override;

    
    void setStatusImageVisible( bool visible );

    
protected:
    uisinglelord();
    
     void updateStatus(character& c) override;
    
    cocos2d::Sprite* getStatusImage();
    cocos2d::Sprite* getFaceImage(character& c);

    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    virtual void onPressStateChangedToDisabled() override;
    
protected:
    Node*           buttonNode;
    Node*           statusNode;
    Node*           locationNode;
    Node*           selectedNode;
};
