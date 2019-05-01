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

#if defined(_DDR_)
#define SHIELD_OFFSET_Y 0
#else
#define SHIELD_OFFSET_Y RES(-6)
#endif


class uisinglelord : public uilordselect
{
public:
    CREATE_FUNC(uisinglelord);
    virtual bool init() override;
    static uisinglelord* createWithLord ( mxid characterid );

    virtual bool setLord( mxid characterid ) override;
    void refreshStatus() override;

    void setStatusImageVisible( bool visible );
    
protected:
    uisinglelord();
    
     void updateStatus(character& c) override;
    
    Sprite* getStatusImage();
    Sprite* getFaceImage(character& c);

    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    virtual void onPressStateChangedToDisabled() override;
    virtual void OnStopDrag(uidragevent* event) override;
    virtual void OnStartDrag(uidragevent* event) override;
    
protected:
    Node*       buttonNode;
    Node*       statusNode;
    Node*       locationNode;
    Node*       selectedNode;
    DrawNode*   innerCircle;
    LayerColor* bg;
    bool        statusVisible;
};

