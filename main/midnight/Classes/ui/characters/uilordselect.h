//
//  uilordselect.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once

#include "../uielement.h"
#include "../../tme_interface.h"
#include "../../system/tmemanager.h"

enum LORD_STATUS {
    status_normal=0,
    status_dawn     = MXBIT(1),
    status_night    = MXBIT(2),
    status_inbattle = MXBIT(3),
    status_dead     = MXBIT(4),
    status_tunnel   = MXBIT(5),
    status_selected = MXBIT(6),
    status_location = MXBIT(7),
};

#define MOUSE_OVER_INNER        ((UIMOUSEOVER)(MOUSE_OVER_USER+0))
#define MOUSE_OVER_OUTER        ((UIMOUSEOVER)(MOUSE_OVER_USER+1))
#define MOUSE_OVER_FACE         ((UIMOUSEOVER)(MOUSE_OVER_USER+2))
#define MOUSE_OVER_FOLLOWER     ((UIMOUSEOVER)(MOUSE_OVER_USER+3))

#define DISTANCE_SHIELD         RES(40)
#define DISTANCE_INNER          RES(75)
#define DISTANCE_OUTER          RES(128)
#define SELECT_ELEMENT_WIDTH    DIS(128)
#define SELECT_ELEMENT_HEIGHT   DIS(128)
#define SELECT_IMAGE_HEIGHT     DIS(72)

enum class lordwidget {
    select = 0,
    single = 1,
    group = 2,
    grouped = 3
};



class uilordselect :
    public cocos2d::ui::Widget,
    public chilli::ui::DragMoveElement,
    public chilli::ui::DropTarget
{
protected:
    using Widget = cocos2d::ui::Widget;
    using Touch = cocos2d::Touch;
    using Event = cocos2d::Event;
    using Vec2 = cocos2d::Vec2;
    using uidragevent = chilli::ui::DragEvent;
    
    
public:
    
    CREATE_FUNC(uilordselect);
    
    virtual bool init() override;
    virtual void updateStatus(character& c);
    virtual bool setLord( mxid characterid );
    virtual void refreshStatus();

    virtual UIMOUSEOVER MouseOverHotspot( Vec2 pos, UIMOUSEOVERHINT hint ) const override ;

    char_data_t* userData()  { return static_cast<char_data_t*>(getUserData()); }
    
    
    Vec2 getCenter() const;
    
    virtual void setPage(page_t page) { this->page = page; }
    page_t getPage() const { return page;}
    
protected:
    uilordselect();
    void addTouchListener();

    bool onTouchBegan(Touch *touch, Event* ) override;
    void onTouchMoved(Touch *touch, Event* ) override;
    void onTouchEnded(Touch *touch, Event* )  override;
    void onTouchCancelled(Touch *touch, Event* ) override;
    
    virtual void OnStopDrag(uidragevent* event) override;
    virtual void OnStartDrag(uidragevent* event) override;
    virtual void OnDropStart() override;
    virtual void OnDropStop() override;
    
    f32 getDistanceFromCentre( Vec2 pos ) const;

    Vec2 mouse_down_pos;
    Vec2 mouse_last_position;

public:
    flags16         status;
    mxid            characterId;
    lordwidget      lordtype;
    page_t          page;
    
};

