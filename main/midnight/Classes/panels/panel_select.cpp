
/*
 *  panel_select.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_select.h"

#include "../system/moonring.h"
#include "../system/configmanager.h"
#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"
#include "../frontend/language.h"
#include "../ui/characters/uisinglelord.h"
#include "../ui/characters/uigrouplord.h"

USING_NS_CC;
USING_NS_CC_UI;
using namespace tme;

#if defined(_LOM_)
    #define BACKGROUND_COLOUR   _clrWhite
#endif
#if defined(_DDR_)
    #define BACKGROUND_COLOUR   _clrCyan
#endif

static const u32 MaxPages = 10;

bool panel_select::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    model = &mr->selectmodel;
    dropTarget = nullptr;
    draggedLord = nullptr;
    pageFlipAllowed = true;
    
    uishortcutkeys::init(safeArea, clickCallback);
    
    f32 scale = resolutionmanager::getInstance()->phoneScale() ;
    
    RIGHT_STRIP_WIDTH = RES(128);
    BOTTOM_STRIP_HEIGHT = RES(128);
    SELECT_GRID_X = SELECT_ELEMENT_WIDTH * scale;
    SELECT_GRID_Y = SELECT_ELEMENT_HEIGHT * scale;
    SELECT_GRID_Y_LEADING = RES(0);
    SELECT_GRID_X_LEADING = RES(0);
    MAX_COLUMNS = (getContentSize().width-RIGHT_STRIP_WIDTH)/SELECT_GRID_X;
    MAX_ROWS = (getContentSize().height-BOTTOM_STRIP_HEIGHT)/SELECT_GRID_Y;
    MAX_LORDS_PER_PAGE = MAX_ROWS * MAX_COLUMNS ;
    
    START_X = HALF(SELECT_GRID_X);
    START_Y = HALF(SELECT_GRID_Y);
    
    setBackground(BACKGROUND_COLOUR);
    
    auto backgroundColour = BACKGROUND_COLOUR;

    gradientB = uihelper::createVerticalGradient( backgroundColour, BOTTOM_STRIP_HEIGHT, HALF(BOTTOM_STRIP_HEIGHT), getContentSize().width, 1 );
    gradientB->setLocalZOrder(ZORDER_UI-1);
    safeArea->addChild(gradientB);

    gradientR =  uihelper::createHorizontalGradient( backgroundColour, RIGHT_STRIP_WIDTH, HALF(RIGHT_STRIP_WIDTH), getContentSize().height, -1 );
    gradientR->setLocalZOrder(ZORDER_UI-1);
    uihelper::AddTopRight(safeArea,gradientR);
    
    //safeArea->setLocalZOrder(ZORDER_UI);
    
    
    int r = DIS(64) * scale;
    
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(16), RES(10) );
    
    // Look Icon
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, clickCallback);
    uihelper::AddBottomRight(safeArea, night, RES(0), RES(10) );
    
    int adjy=RES(32)*scale;

    createFilterButton(ID_FILTER_DAWN,          (r*1)-adjy, "lord_dawn",    select_filters::show_dawn);
    createFilterButton(ID_FILTER_NIGHT,         (r*2)-adjy, "lord_night",   select_filters::show_night);
    createFilterButton(ID_FILTER_BATTLE,        (r*3)-adjy, "lord_battle",  select_filters::show_battle);
    createFilterButton(ID_FILTER_DEAD,          (r*4)-adjy, "f_dead!",      select_filters::show_dead);
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*5)-adjy, "i_center",     select_filters::show_current);
    
    auto cleanup = uihelper::CreateImageButton("i_cleanup", ID_CLEANUP_SELECT, clickCallback);
    uihelper::AddTopRight(safeArea, cleanup, RES(32), (r*6)-adjy );

    createPageView();
    
    getCharacters();
    
    addShortcutKey(ID_FILTER_DAWN,       KEYCODE(F1));
    addShortcutKey(ID_FILTER_NIGHT,      KEYCODE(F2));
    addShortcutKey(ID_FILTER_BATTLE,     KEYCODE(F3));
    addShortcutKey(ID_FILTER_DEAD,       KEYCODE(F4));
    addShortcutKey(ID_FILTER_CURRENT_LOC,KEYCODE(F5));
    
    addShortcutKey(ID_LOOK,              K_LOOK);
    addShortcutKey(ID_NIGHT,             K_NIGHT);
    
    // set current page
    
    pageView->setCurrentPageIndex(model->page);
    
    return true;
}

void panel_select::createPageView()
{
    // lets have a pageview
    pageView = PageView::create();
    pageView->setDirection(PageView::Direction::HORIZONTAL);
    pageView->setBounceEnabled(true);
    pageView->setIndicatorEnabled(true);
    pageView->setCurrentPageIndex(0);
    pageView->setIndicatorIndexNodesColor(_clrBlack);
    pageView->setIndicatorSelectedIndexColor(_clrBlue);
    
    pageView->setIndicatorIndexNodesScale(DIS(0.25f));
    pageView->setIndicatorSpaceBetweenIndexNodes(DIS(-5));
    
    uihelper::AddBottomLeft(safeArea, pageView);
    uihelper::FillParent(pageView);
    
    auto padding = resolutionmanager::getInstance()->getSafeArea();
    pageView->setIndicatorPosition( Vec2(pageView->getContentSize().width/2,padding.bottom + RES(10)) );
    
    pageView->addEventListener( [&]( Ref* sender, PageView::EventType e){
        if ( e == PageView::EventType::TURNING ) {
            UIDEBUG("Page turning");
        }
    });
    
    setupPages();
    
}

void panel_select::addNewPage(ssize_t page) {
    auto layout = Layout::create();
    layout->setContentSize(pageView->getContentSize());
    layout->setTag((s32)page+1);
    pages.pushBack(layout);
    pageView->insertPage(layout, (s32)page);
}

void panel_select::setupPages()
{
    for ( s32 ii=0; ii<MaxPages; ii++ ) {
        addNewPage(ii);
    }
}

void panel_select::enableUI()
{
    pageView->setTouchEnabled(true);
    for ( auto node : safeArea->getChildren() ) {
        CONTINUE_IF( node == pageView );
        node->setCascadeOpacityEnabled(true);
        node->runAction(FadeIn::create(0.25f));
    }
}

void panel_select::disableUI()
{
    pageView->setTouchEnabled(false);
    for ( auto node : safeArea->getChildren() ) {
        CONTINUE_IF( node == pageView );
        node->setCascadeOpacityEnabled(true);
        node->runAction(FadeOut::create(0.25f));
    }
}

void panel_select::showNewLordPosition( uilordselect* lord )
{
    lord->setCascadeOpacityEnabled(true);
    lord->setOpacity(alpha_zero);
    lord->runAction(FadeIn::create(1.00f));
}

void panel_select::getCharacters()
{
    s32 usedIndex=0;
    
    lords.clear();
    
    model->updateCharacters();
    
    // now place all the lords
    character c;
    for ( auto id : model->characters )
    {
        TME_GetCharacter(c,id);
        
        CONTINUE_IF ( c.following );
        
        layoutid_t tag = (layoutid_t) (ID_SELECT_CHAR+c.id);
        
        uilordselect* lord = nullptr;
        
        if ( c.followers ) {
            lord = uigrouplord::createWithLord(c.id);
        }else{
            lord = uisinglelord::createWithLord(c.id);
        }
        
        lord->setAnchorPoint(uihelper::AnchorCenter);
        lord->setIgnoreAnchorPointForPosition(false);
        lord->addClickEventListener(clickCallback);
        lord->setTag(tag);
        lord->setUserData(c.userdata);
        lord->enableDrag();
        lord->enableDrop();
        lord->drag_delegate = this;
        lords.pushBack(lord);
        
        auto userdata = lord->userData();
        
        if ( mr->config->keyboard_mode == CF_KEYBOARD_CLASSIC )
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_old));
        else
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_new));

        // only add positions for lords that already
        // have a position
        if ( model->upgraded ) {
            if ( userdata->select_loc != point::ZERO && userdata->select_page > InvalidPage ) {
                Vec2 pos;
                pos.x = userdata->select_loc.x;
                pos.y = userdata->select_loc.y;
                addToPage(lord, userdata->select_page, pos);
            }
        }
        
        if ( c.followers ) {
            static_cast<uigrouplord*>(lord)->createFollowers();
        }
        
        applyFilters( lord, c );
        
    }
    
    // now position all the lords that haven't already been added
    // to the scrollview
    if ( model->upgraded ) {
        for ( auto lord : lords )
        {
            auto userdata = lord->userData();
            
            if ( userdata->select_loc == point::ZERO ) {
            
                s32 index = usedIndex;
                auto page = calcPage(index);
                
                if ( IS_PARENT_PAGE(userdata->select_page) ) {
                    page = FROM_PARENT_PAGE(userdata->select_page);
                    index = ((s32)page-1) * MAX_LORDS_PER_PAGE;
                } else {
                    usedIndex++;
                }
                
                // add the lord to their page in their position
                addToPage(lord, page, calcGridLocation(index));
                
                // now check that is valid, and move them accordingly
                checkCollision(lord,index);
                
                // and store
                storeLordPosition(lord);
                
                // animate
                showNewLordPosition(lord);
            }
        }
    }else{
        resetPositions();
    }
    
    removeEmptyEndPages();
}

void panel_select::addToPage( uilordselect* lord, page_t page, Vec2 pos)
{
    lord->setPage(page);
    lord->setPosition(pos);
    pages.at(TO_PAGE_INDEX(page))->addChild(lord);
}

void panel_select::checkCollision ( uilordselect* lord, s32 start )
{
    uilordselect* overlap;
    
    auto page = lord->getPage();
    
    s32 index = start;
    bool looped = false;
    
    for(;;) {
    
        if ( (overlap=getOverlappingLord(page,lord)) == nullptr )
            break;
        
        page = calcPage(index);
        if ( page > pages.size() ) {
            if ( page < MaxPages ) {
                addNewPage(page);
            }else{
                if ( looped == true ) {
                    addToPage(lord, 0, calcGridLocation(0));
                    break;
                }
                page=1;
                index=0;
                continue;
            }
        }
        
        lord->removeFromParent();
        addToPage(lord, page, calcGridLocation(index));
        index++;
    }
    
}

uilordselect* panel_select::getOverlappingLord ( page_t page, uilordselect* source )
{
    character c;
    
    mxid id = getIdFromTag(source);
    TME_GetCharacter(c, id );

    int g2= HALF(SELECT_GRID_X);
    int g1 = ( c.followers ) ? SELECT_GRID_X : HALF(SELECT_GRID_X) ;
 
    auto pageNode = pages.at( TO_PAGE_INDEX(page) );
    
    for ( Node* node : pageNode->getChildren() )
    {
        auto target = dynamic_cast<uilordselect*>(node);
        
        CONTINUE_IF_NULL(target);
        CONTINUE_IF( target == source );
        CONTINUE_IF( target->getPage() != page);
        
        auto p2 = target->getPosition();
        auto p1 = source->getPosition();
        
        mxid id2 = getIdFromTag(target);
        
        TME_GetCharacter(c,id2 );
        CONTINUE_IF(c.following);
        
        g2 = ( c.followers ) ? SELECT_GRID_X : HALF(SELECT_GRID_X/2) ;
        
        int radius_added = RES(g1) + RES(g2) ;
        
        int dx = ABS(p1.x - p2.x);
        int dy = ABS(p1.y - p2.y);
        int distance = sqrt((dx*dx)+(dy*dy));
        
        if ( distance < radius_added )
            return target;
    }
    
    return nullptr ;
}

page_t panel_select::calcPage( u32 index )
{
    return (index / MAX_LORDS_PER_PAGE) +1 ;
}

// position from top left of canvas
Vec2 panel_select::calcGridLocation ( u32 index )
{
    Vec2 pos = Vec2::ZERO ;
    
    // find page
    auto page = calcPage(index);
    
    index = index - (((s32)page-1) * MAX_LORDS_PER_PAGE);
    
    int r = index / MAX_COLUMNS ;
    int c = index % MAX_COLUMNS ;
    
    pos.x = (c * (SELECT_GRID_X+SELECT_GRID_X_LEADING)) + START_X;
    pos.y = (r * (SELECT_GRID_Y+SELECT_GRID_Y_LEADING)) + START_Y;
    pos.y = this->getContentSize().height-pos.y;
    
    return pos;
}


void panel_select::updateFilters()
{
    character        c;
    for ( auto button : lords )
    {
        auto id = getIdFromTag(button);
        
        TME_GetCharacter(c,id);
        CONTINUE_IF( c.following );

        applyFilters( button, c );
    }
    
}

void panel_select::applyFilters ( uilordselect* e, character& c )
{
    auto filters = model->filters;
    
    BOOL show=true;
    if ( Character_IsNight(c) && !filters.Is(select_filters::show_night) )
        show=false;
    if ( Character_IsDawn(c) && !filters.Is(select_filters::show_dawn))
        show=false;
    if ( Character_IsInBattle(c) && !filters.Is(select_filters::show_battle) )
        show=false;
    if ( Character_IsDead(c) && !filters.Is(select_filters::show_dead))
        show=false;
    
#if defined(_DDR_)
    if ( Character_IsInTunnel(c) && !filters.Is(select_filters::show_intunnel))
        show=false;
    if ( Character_IsPreparingForBattle(c) && !filters.Is(select_filters::show_battle) )
        show=false;
#endif
    
    if ( !filters.Is(select_filters::show_current) && e->status.Is(LORD_STATUS::status_location) )
        show=false;
    
    e->setVisible(show);
}

uifilterbutton* panel_select::createFilterButton( layoutid_t id, s32 y, const std::string& image, select_filters flag )
{
    auto button = uifilterbutton::createWithImage(image);
    button->setTag(id);
    button->setScale(resolutionmanager::getInstance()->phoneScale());
    button->setSelected(model->filters.Is(flag));
    button->addEventListener(eventCallback);
    uihelper::AddTopRight(safeArea, button, RES(32), y );
    return button;
}

void panel_select::updateFilterButton(Ref* sender,select_filters flag)
{
    model->filters.Toggle(flag);
    auto button = dynamic_cast<uifilterbutton*>(sender);
    if ( button != nullptr ) {
        button->addEventListener(nullptr);
        button->setSelected(model->filters.Is(flag));
        button->addEventListener(eventCallback);
    }
    updateFilters();
}

void panel_select::OnNotification( Ref* sender )
{
    auto button = static_cast<Button*>(sender);
    if ( button == nullptr )
        return;
    
    layoutid_t id = static_cast<layoutid_t>(button->getTag());
    
    if ( id >= ID_SELECT_CHAR ) {
        mxid characterId = id-ID_SELECT_CHAR;
        mr->selectCharacter(characterId);
        return;
    }
    
    switch ( id  )
    {
        case ID_LOOK:
            model->page = (s32)pageView->getCurrentPageIndex();
            gradientB->setVisible(false);
            gradientR->setVisible(false);
            mr->look();
            break;
            
        case ID_FILTER_DAWN:
            updateFilterButton(sender,select_filters::show_dawn);
            break;
        case ID_FILTER_NIGHT:
            updateFilterButton(sender,select_filters::show_night);
            break;
        case ID_FILTER_BATTLE:
            updateFilterButton(sender,select_filters::show_battle);
            break;
        case ID_FILTER_DEAD:
            updateFilterButton(sender,select_filters::show_dead);
            break;
        case ID_FILTER_CURRENT_LOC:
            updateFilterButton(sender,select_filters::show_current);
            break;
            
        case ID_NIGHT:
        {
            AreYouSure(NIGHT_MSG, [&] {
                model->page = (s32)pageView->getCurrentPageIndex();
                mr->night();
            });
            break;
        }
        
        case ID_CLEANUP_SELECT:
            resetPositions();
            pageView->scrollToPage(0);
            break;
            
        case ID_GROUP_DISBAND:
        {
            auto parent = static_cast<Button*>(sender)->getParent();
            auto leader = static_cast<uigrouplord*>(parent);
            
            storeAllLordsPositions();
            
            layoutid_t id = static_cast<layoutid_t>(leader->getTag()-ID_SELECT_CHAR);
            mr->disbandGroup(id);
            
            refreshCharacters();
            break;
        }
            
        default:
            break;
    }
}

void panel_select::resetPositions()
{
    for( auto node : lords ) {
        node->removeFromParent();
    }
    
    s32 index = 0;
    for( auto button : lords ) {
        
        addToPage(button, calcPage(index), calcGridLocation(index));
        checkCollision(button,0);
        
        // adjust for big group circle
        if ( button->lordtype == lordwidget::group ) {
            auto pos = button->getPosition();
            f32 adjustX = HALF(SELECT_ELEMENT_WIDTH);
            if ( pos.x-adjustX <= 0 )
                pos.x += adjustX;
            
            pos.y -= HALF(SELECT_ELEMENT_HEIGHT);
            button->setPosition(pos);
        }

        storeLordPosition(button);
        index++;
    }
}

void panel_select::storeAllLordsPositions()
{
    showCharacterPositions();

    for ( auto lord : lords ) {
        storeLordPosition(lord);
    }
}

void panel_select::OnDeActivate()
{
    uipanel::OnDeActivate();
    
    storeAllLordsPositions();
    
}

void panel_select::showCharacterPositions()
{
    for( auto e : lords ) {
        
        auto ud = e->userData();
        UIDEBUG( "%d %s = (%f,%f) %d", ud->id, ud->symbol.c_str(), e->getPosition().x, e->getPosition().y, e->getPage());
        
        if ( e->lordtype == lordwidget::group ) {
            auto parent = static_cast<uigrouplord*>(e);
            for( auto c : parent->followers ) {
                auto u = c->userData();
                UIDEBUG( "    %d %s = (%f,%f) %d", u->id, u->symbol.c_str(), c->getPosition().x, c->getPosition().y, c->getPage());
            }
        }
        
    }
}

void panel_select::placeDraggedLordOnCurrentPage()
{
    auto currentPage = pageView->getCurrentPageIndex();
    pages.at(currentPage)->addChild(draggedLord);
    draggedLord->setPage( FROM_PAGE_INDEX(currentPage) );
}

void panel_select::draggedLordMovesOnPage()
{
    placeDraggedLordOnCurrentPage();
    storeLordPosition(draggedLord);
    draggedLord = nullptr;
}

void panel_select::draggedLordLeavesGroup( uidragevent *event)
{
    mxid draggedLordId = getIdFromTag(draggedLord);
    if ( mr->leaveGroup(draggedLordId) ) {
        draggedLord->setPosition(event->position);
        draggedLord->setPage( FROM_PAGE_INDEX(pageView->getCurrentPageIndex()) );
        storeLordPosition(draggedLord);
        refreshCharacters();
    }else{
        draggedLordReturnsToGroup();
    }
}

void panel_select::draggedLordMovesGroup()
{
    mxid draggedLordId = getIdFromTag(draggedLord);
    if ( mr->leaveGroup(draggedLordId) ) {
        draggedLord->setPage(InvalidPage);
        draggedLord->setPosition(Vec2::ZERO);
        if ( mr->joinGroup(draggedLordId, potentialLeader) ) {
            draggedLord->setPage( TO_PARENT_PAGE(dropTarget->getPage()) );
        }
        refreshCharacters();
    }else{
        draggedLordReturnsToGroup();
    }
}

void panel_select::draggedLordBecomesLeader()
{
    mxid draggedLordId = getIdFromTag(draggedLord);
    if ( mr->swapGroupLeader(draggedLordId) ) {
        draggedLord->setPage( FROM_PAGE_INDEX(pageView->getCurrentPageIndex()) );
        draggedLord->setPosition(dropTarget->getPosition());
        dropTarget->setPage( TO_PARENT_PAGE(draggedLord->getPage()) );
        dropTarget->setPosition(Vec2::ZERO);
        storeLordPosition(draggedLord);
        storeLordPosition(dropTarget);
        refreshCharacters();
    }else{
        draggedLordReturnsToGroup();
    }
}

void panel_select::draggedLordJoinsGroup()
{
    mxid draggedLordId = getIdFromTag(draggedLord);
    if ( mr->joinGroup(draggedLordId, potentialLeader) ) {
        refreshCharacters();
    }else{
        draggedLordReturnsToPage();
    }
}

void panel_select::OnDragDropNotification( uidragelement* sender, uidragevent* event )
{
    using DragEventType = chilli::ui::DragEvent::Type;
    
    auto lord = static_cast<uilordselect*>(event->element);

    mxid draggedLordId = getIdFromTag(lord);
    
    character        c;
    Vec2 droppedAt;

    switch (event->type) {
            
        case DragEventType::select:
            
            // get intial position
            startDragParent = lord->getParent();
            startDragPosition = lord->getPosition();
            startDragPage = lord->getPage();
            
            // disable scrolling while dragging
            disableUI();
            break;
            
        case DragEventType::start:
            // clear drop target
            dropTarget = nullptr;
            draggedLord = lord;
            TME_GetCharacter(c, draggedLordId );
            currentlyFollowing = c.following;
            potentialLeader = IDT_NONE;
            
            // remove from page and add to panel
            draggedLord->removeFromParent();
            this->addChild(draggedLord);
            if ( currentlyFollowing ) {
                draggedLord->setPosition(event->position);
                draggedLord->setScale(scale_normal);
            }
            
            break;

        case DragEventType::stop:
        {
            // enable scrolling after dragging
            enableUI();
        
            // remove lord from panel
            draggedLord->removeFromParent();
            
            // check if off page
            if ( !checkValidDropLocation() )
                return;
            
            // just moved
            if ( dropTarget == nullptr && currentlyFollowing == IDT_NONE ) {
                draggedLordMovesOnPage();
                return;
            }
            
            IF_NOT_NULL(dropTarget)->endDropTarget();

            // Are we following someone? - currentlyFollowing
            //  YES - Have we dropped on the person we are currentlyFollowing then we are swapping the leader
            //  YES - Have we dropped on nothing then we are unfollowing
            //  YES - Have we dropped on someone new then we are moving to a new group so unfollow then follow
            //
            //  NO - If we drop on nothing then we have just moved
            //  NO - if we drop on someone then follow them
            
            showCharacterPositions();
            
            if ( currentlyFollowing ) {
                
                if ( potentialLeader == IDT_NONE ) {
                    draggedLordLeavesGroup(event);
                }
                else if ( potentialLeader == currentlyFollowing ) {
                    
                    // check for swapping group lord
                    if ( lordDropResult == MOUSE_OVER_OUTER  ) {
                        draggedLordReturnsToGroup();
                    } else {
                        draggedLordBecomesLeader();
                    }
                }
                else {
                    draggedLordMovesGroup();
                }
            }
            else {
                if ( potentialLeader != IDT_NONE ) {
                    draggedLordJoinsGroup();
                }
            }
            
            dropTarget = nullptr;
            draggedLord = nullptr;
            break;
        }
            
        case DragEventType::drag:
        {
            checkValidDropTarget();
            
            checkPageFlip();
  
            break;
        }
            
        default:
            break;
    }
}

void panel_select::checkValidDropTarget()
{
    character c;
    
    if ( dropTarget && dropTarget->lordtype == lordwidget::group ) {
        static_cast<uigrouplord*>(dropTarget)->possibleSwap = false;
        dropTarget->refreshStatus();
    }
    
    auto newTarget = getDropTarget(draggedLord,MOUSE_OVER_OUTER,lordDropResult);
    
    if ( newTarget && newTarget->lordtype == lordwidget::group ) {
        if ( currentlyFollowing && currentlyFollowing == potentialLeader ) {
            static_cast<uigrouplord*>(newTarget)->possibleSwap = lordDropResult != MOUSE_OVER_OUTER;
            newTarget->refreshStatus();
        }
    }
    
    
    if ( newTarget != dropTarget ) {
        
        
        // clear old target
        if ( dropTarget != nullptr ) {
            dropTarget->endDropTarget();
        }
        
        if ( newTarget != NULL ) {
            
            potentialLeader = getIdFromTag(newTarget);
            
            mxid id = getIdFromTag(draggedLord);
            TME_GetCharacter(c,id );
            
            if ( Character_CanFollow(c,potentialLeader) ) {
                newTarget->startDropTarget();
            }else{
                newTarget=nullptr;
                potentialLeader = IDT_NONE;
            }
            
        } else {
            potentialLeader = IDT_NONE;
        }
        
        dropTarget = newTarget;
    }

}

void panel_select::draggedLordReturnsToGroup()
{
    draggedLord->setScale(GROUPED_LORD_SCALE);
    draggedLord->setPosition(startDragPosition);
    startDragParent->addChild(draggedLord);
    pageView->scrollToPage( TO_PAGE_INDEX(FROM_PARENT_PAGE(startDragPage)) );
    
}

void panel_select::draggedLordReturnsToPage()
{
    auto currentPage = TO_PAGE_INDEX(startDragPage);
    
    draggedLord->setPosition(startDragPosition);
    pages.at( currentPage )->addChild(draggedLord);
    draggedLord->setPage(startDragPage);
    pageView->scrollToPage( currentPage );
}

bool panel_select::checkValidDropLocation()
{
    bool valid = true;
    
    if ( dropTarget == nullptr
        && (draggedLord->getPosition().x > getContentSize().width - RIGHT_STRIP_WIDTH
            || draggedLord->getPosition().y < BOTTOM_STRIP_HEIGHT) ) {
            
            valid = false;
        }
    
    else if ( dropTarget == nullptr ) {
        UIMOUSEOVER result;
        if ( getDropTarget(draggedLord, MOUSE_OVER_OUTER, result) != nullptr) {
            valid = false;
        }
    }
    
    if ( !valid ) {
        if ( currentlyFollowing ) {
            draggedLordReturnsToGroup();
        }else{
            draggedLordReturnsToPage();
        }
    }
    
    return valid;
}

void panel_select::refreshCharacters() {

    this->scheduleOnce( [&](float) {
        for ( auto lord : lords ) {
            lord->removeFromParent();
            // store children
            if ( lord->lordtype == lordwidget::group ) {
                static_cast<uigrouplord*>(lord)->clearFollowers();
            }
        }
        getCharacters();
        
    }, 0, "delayed_refreshCharacters" );
    

}


mxid panel_select::getIdFromTag(uilordselect* lord)
{
    return lord->getTag() - ID_SELECT_CHAR ;
}

uilordselect* panel_select::getDropTarget( uilordselect* lord, UIMOUSEOVER where, UIMOUSEOVER& result )
{
    auto pos = lord->getCenter();
    auto currentPage = pageView->getCurrentPageIndex()+1;
    
    for ( auto target : lords ) {
        
        CONTINUE_IF(target==lord);
        CONTINUE_IF(target->getPage() != currentPage);
        
        result = target->MouseOverHotspot(pos, MOUSE_OVER_HINT_DROP);
        
        if ( where == MOUSE_OVER_INNER ) {
            if ( result == where || result == MOUSE_OVER_FACE )
                return target;
        }
        
        if ( where == MOUSE_OVER_OUTER ) {
            if ( result == where || result == MOUSE_OVER_INNER || result == MOUSE_OVER_FACE )
                return target;
        }
        
        if ( result == where ) {
            return target ;
        }
    }
    return nullptr;
}

void panel_select::storeLordPosition( uilordselect* lord )
{
    auto userdata = lord->userData() ;
    if ( userdata != nullptr ) {
        auto position = lord->getPosition();

        if ( lord->getPage() <= InvalidPage ) {
            userdata->select_page = lord->getPage();
            userdata->select_loc.x = 0;
            userdata->select_loc.y = 0;
        } else {
            userdata->select_page = lord->getPage();
            userdata->select_loc.x = position.x;
            userdata->select_loc.y = position.y;
        }
    }
    
    // store children
    if ( lord->lordtype == lordwidget::group ) {
        for( auto child : static_cast<uigrouplord*>(lord)->followers ) {
            storeLordPosition(child);
        }
    }
    
}

void panel_select::checkPageFlip()
    {
    
    if ( !pageFlipAllowed ) {
        return;
    }
    
    if ( draggedLord == nullptr )
        return;
    
    // check
    auto position = draggedLord->getPosition();
    auto size = getContentSize();
    auto index = pageView->getCurrentPageIndex();
    
    if (position.x > size.width - (RIGHT_STRIP_WIDTH*0.75) ) {
    
        if ( index == pages.size()-1 && index < MaxPages ) {
            addNewPage(index+1);
            pageView->setCurrentPageIndex(index+1);
        }
        else if ( index<pages.size() ) {
            pageView->scrollToPage(index+1);
        }
        pageFlipAllowed=false;
    }
    else if (position.x < (RIGHT_STRIP_WIDTH*0.50) && index>0 ) {
        pageView->scrollToPage(index-1);
        pageFlipAllowed=false;
    }
    
    if ( !pageFlipAllowed ) {
        this->scheduleOnce( [&](float) {
            pageFlipAllowed=true;
            this->checkPageFlip();
        }, 2, "freshPageFlip" );
        
    }
    
}

void panel_select::removeEmptyEndPages()
{
    if ( pages.size() == 0)
        return;
    
    for (;;) {
        BREAK_IF(pages.size() == 0);
        
        auto page = pages.at( pages.size()-1 );
        
        BREAK_IF ( page->getChildren().size() != 0 ) ;
        
        pages.popBack();
        pageView->removePage(page);
    }
}
