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
    
    void getCharacters();

    cocos2d::Vec2 calcGridLocation ( u32 index );
    s32 calcPage( u32 index );
    s32 getLordPage( uilordselect* lord  );
    void addToPage( uilordselect* lord, s32 page, cocos2d::Vec2 pos);
    void checkPageFlip() ;

    
    void updateFilters();
    void applyFilters ( uilordselect* e, character& c );
    uilordselect* getLordElement( mxid id );
    void checkCollision ( uilordselect* lord, s32 index );
    uilordselect* getOverlappingLord ( s32 page, uilordselect* source );
    uilordselect* getDropTarget( uilordselect* lord, UIMOUSEOVER where, UIMOUSEOVER& result );
    void resetPositions();

    void showCharacterPositions();
    void refreshCharacters();

    mxid getIdFromTag( uilordselect* lord );
    void storeLordPosition( uilordselect* lord );

    void createPageView();
    void addNewPage(s32 ii);
    void setupPages();
    void removeEmptyEndPages();

    
    void storeAllLordsPositions();

    void disableUI();
    void enableUI();
    void showNewLordPosition( uilordselect* lord );

    void checkValidDropTarget();
    void placeDraggedLordOnCurrentPage();
    void draggedLordMovesOnPage();
    void draggedLordReturnsToGroup();
    void draggedLordReturnsToPage();
    void draggedLordLeavesGroup(uidragevent *event);
    void draggedLordMovesGroup();
    void draggedLordBecomesLeader();
    void draggedLordJoinsGroup();
    bool checkValidDropLocation();
    
private:
    s32 SELECT_GRID_X ;
    s32 SELECT_GRID_Y ;
    s32 SELECT_GRID_X_LEADING ;
    s32 SELECT_GRID_Y_LEADING ;
    s32 MAX_COLUMNS ;
    s32 MAX_ROWS ;
    s32 MAX_LORDS_PER_PAGE ;
    s32 START_X ;
    s32 START_Y ;
    s32 BOTTOM_STRIP_HEIGHT;
    s32 RIGHT_STRIP_WIDTH;

    cocos2d::ui::PageView*          pageView;
    cocos2d::Vector<cocos2d::ui::Layout*>     pages;

    selectmodel*                        model;
    cocos2d::ui::ScrollView*            scrollView;
    cocos2d::Vector<Node*>      lords;

    uilordselect* dropTarget;
    mxid potentialLeader;
    
    uilordselect* draggedLord;
    mxid currentlyFollowing;
    
    cocos2d::Node* startDragParent;
    cocos2d::Vec2 startDragPosition;
    s32 startDragPage;
    UIMOUSEOVER lordDropResult;
    
    
    bool pageFlipAllowed;
};
