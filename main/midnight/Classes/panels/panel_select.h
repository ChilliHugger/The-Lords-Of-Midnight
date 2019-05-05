/*
 *  panel_select.h
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#pragma once
#include "../cocos.h"
#include "../ui/characters/uilordselect.h"
#include "../ui/uipanel.h"
#include "../ui/uifilterbutton.h"
#include "../frontend/layout_id.h"
#include "../system/configmanager.h"
#include "../models/selectmodel.h"

class panel_select : public uipanel, uidragdropdelegate
{
    using PageView = cocos2d::ui::PageView;
    using ScrollView = cocos2d::ui::ScrollView;
    using Layout = cocos2d::ui::Layout;
    
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

    Vec2 calcGridLocation ( u32 index );
    page_t calcPage( u32 index );
    void addToPage( uilordselect* lord, page_t page, Vec2 pos);
    void checkPageFlip() ;

    void updateFilters();
    void applyFilters ( uilordselect* e, character& c );
    void checkCollision ( uilordselect* lord, s32 index );
    void resetPositions();
    
    uilordselect* getOverlappingLord ( page_t page, uilordselect* source );
    uilordselect* getDropTarget( uilordselect* lord, UIMOUSEOVER where, UIMOUSEOVER& result );
    
    void showCharacterPositions();
    void refreshCharacters();

    mxid getIdFromTag( uilordselect* lord );
    void storeLordPosition( uilordselect* lord );

    void createPageView();
    void addNewPage(ssize_t page);
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

    selectmodel*            model;
    
    Node*                   gradientB;
    Node*                   gradientR;
    PageView*               pageView;
    ScrollView*             scrollView;
    Vector<uilordselect*>   lords;
    Vector<Layout*>         pages;
    
    // drag and drop
    uilordselect*           dropTarget;
    uilordselect*           draggedLord;
    mxid                    potentialLeader;
    mxid                    currentlyFollowing;
    Node*                   startDragParent;
    Vec2                    startDragPosition;
    page_t                  startDragPage;
    UIMOUSEOVER             lordDropResult;
    bool                    pageFlipAllowed;
};


