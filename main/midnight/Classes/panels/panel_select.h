/*
 *  panel_select.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once

#include "../ui/uipanel.h"
#include "../ui/uifilterbutton.h"
#include "../frontend/layout_id.h"
#include "../system/configmanager.h"
#include "../models/selectmodel.h"

FORWARD_REFERENCE(uilordselect);

class panel_select : public uipanel, uidragdropdelegate
{
	
public:
    virtual bool init() override;
    
    CREATE_FUNC(panel_select);
    
protected:
    uifilterbutton* createFilterButton( layoutid_t id, s32 y, const std::string& image, select_filters flag );
    void updateFilterButton(Ref* sender,select_filters flag);

    virtual void OnDragDropNotification( uidragelement* sender, uidragevent* event ) override ;
    virtual void OnNotification( Ref* sender ) override;
    virtual void OnDeActivate() override;
    
    chilli::types::f32 extracted();
    
    void getCharacters();
    void setInitialScrollViewHeight();
    void updateScrollViewSize( f32 width, f32 height );
    void resizeScrollView();
    void resizeCanvas( s32 left, s32 top, s32 right, s32 bottom );

    cocos2d::Vec2 calcGridLocation ( u32 index );
    
    void updateFilters();
    void applyFilters ( uilordselect* e, character& c );
    uilordselect* getLordElement( mxid id );
    void checkCollision ( uilordselect* lord );
    uilordselect* getOverlappingLord ( uilordselect* source );
    uilordselect* getDropTarget( uilordselect* lord, UIMOUSEOVER where, UIMOUSEOVER& result );
    void resetPositions();

    void showCharacterPositions();
    void refreshCharacters();

    mxid getIdFromTag( uilordselect* lord );
    void storeLordPosition( uilordselect* lord );

    
    
private:
    s32            SELECT_GRID_X ;
    s32            SELECT_GRID_Y ;
    s32            SELECT_GRID_X_LEADING ;
    s32            SELECT_GRID_Y_LEADING ;
    s32            MAX_COLUMNS ;
    s32            START_X ;
    s32            START_Y ;

    
    selectmodel*                        model;
    cocos2d::ui::ScrollView*            scrollView;
    cocos2d::Vector<Node*>      lords;

    uilordselect* dropTarget;
    mxid potentialLeader;
    
    uilordselect* draggedLord;
    mxid currentlyFollowing;
};
