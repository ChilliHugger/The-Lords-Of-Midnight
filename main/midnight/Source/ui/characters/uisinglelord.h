//
//  uielement.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once
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
protected:
    using Sprite = ax::Sprite;
    using LayerColor = ax::LayerColor;
    using uidragevent = chilli::ui::DragEvent;
    
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
    
    std::string statusImageName() const;
    void renderLord();

    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    virtual void onPressStateChangedToDisabled() override;
    virtual void OnStopDrag(uidragevent* event) override;
    virtual void OnStartDrag(uidragevent* event) override;
    
protected:
    Node*       buttonNode;
    LayerColor* bg{};
    bool        statusVisible;
    bool        dimmed{};
    std::string faceArt;
};

