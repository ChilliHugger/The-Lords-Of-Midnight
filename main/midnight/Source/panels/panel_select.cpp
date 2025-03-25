
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
#include "../system/settingsmanager.h"
#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"
#include "../frontend/language.h"
#include "../ui/characters/uisinglelord.h"
#include "../ui/characters/uigrouplord.h"

USING_NS_AX;
USING_NS_AX_UI;
USING_NS_TME;

#if defined(_LOM_)
    #define BACKGROUND_COLOUR   _clrWhite
#endif
#if defined(_DDR_)
    #define BACKGROUND_COLOUR   _clrCyan
#endif

static const u32 MaxPages = 10;

mxid getIdFromTag(uilordselect* lord)
{
    return lord->getTag() - ID_SELECT_CHAR ;
}

void showNewLordPosition( uilordselect* lord )
{
    lord->setCascadeOpacityEnabled(true);
    lord->setOpacity(alpha_zero);
    lord->runAction(FadeIn::create(1.00f));
}


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
    
    uishortcutkeys::registerCallback(safeArea, clickCallback);
    
    f32 scale = mr->resolution->phoneScale() ;
    
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
    
    
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(16), RES(10) );
    
    // Look Icon
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, clickCallback);
    uihelper::AddBottomRight(safeArea, night, RES(0), RES(10) );
    
    int adjy=RES(-16);
    int r = RES(PHONE_SCALE(64));

    createFilterButton(ID_FILTER_DAWN,          (r*0)-adjy, "lord_dawn",    select_filters::show_dawn);
    createFilterButton(ID_FILTER_NIGHT,         (r*1)-adjy, "lord_night",   select_filters::show_night);
    createFilterButton(ID_FILTER_BATTLE,        (r*2)-adjy, "lord_battle",  select_filters::show_battle);
    createFilterButton(ID_FILTER_DEAD,          (r*3)-adjy, "f_dead!",      select_filters::show_dead);
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*4)-adjy, "i_center",     select_filters::show_current);
    
    auto cleanup = uihelper::CreateImageButton("i_cleanup", ID_CLEANUP_SELECT, clickCallback);
    //cleanup->setScale(scale_normal);
    uihelper::AddTopRight(safeArea, cleanup, RES(PHONE_SCALE(12)*0.9), (r*5)-adjy );

    createPageView();
    
    getCharacters();
    
    addShortcutKey(ID_FILTER_DAWN,       KEYCODE(F1));
    addShortcutKey(ID_FILTER_NIGHT,      KEYCODE(F2));
    addShortcutKey(ID_FILTER_BATTLE,     KEYCODE(F3));
    addShortcutKey(ID_FILTER_DEAD,       KEYCODE(F4));
    addShortcutKey(ID_FILTER_CURRENT_LOC,KEYCODE(F5));
    
    addShortcutKey(ID_LOOK,              K_LOOK);
    addShortcutKey(ID_NIGHT,             K_NIGHT);

    addShortcutKey(ID_LOOK,              K_ESC);

    
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
    
    
    f32 scale = phoneScale() ;
    pageView->setIndicatorIndexNodesScale(CONTENT_SCALE(0.5f));
    pageView->setIndicatorSpaceBetweenIndexNodes(CONTENT_SCALE(RES(-2)));
    pageView->setIndicatorIndexNodesColor(_clrBlack);
    pageView->setIndicatorSelectedIndexColor(_clrBlue);

    uihelper::AddBottomLeft(safeArea, pageView);
    uihelper::FillParent(pageView);
    
    auto padding = getSafeArea();
    pageView->setIndicatorPosition( Vec2(pageView->getContentSize().width/2,padding.bottom + RES(4)) );
    
    pageView->addEventListener( [&]( Ref* sender, PageView::EventType e){
        if ( e == PageView::EventType::TURNING ) {
            UIDEBUG("Page turning");
        }
    });
    
    auto backgroundColour = BACKGROUND_COLOUR;
    
    gradientB = uihelper::createVerticalGradient( backgroundColour, BOTTOM_STRIP_HEIGHT, HALF(BOTTOM_STRIP_HEIGHT), getContentSize().width, 1 );
    pageView->addProtectedChild(gradientB,ZORDER_UI);
    gradientB->setGlobalZOrder(ZORDER_UI);
    
    gradientR = uihelper::createHorizontalGradient( backgroundColour, RIGHT_STRIP_WIDTH, HALF(RIGHT_STRIP_WIDTH), getContentSize().height, -1 );
    gradientR->setLocalZOrder(ZORDER_UI-1);
    gradientR->setPosition(Vec2(pageView->getContentSize().width,pageView->getContentSize().height));
    gradientR->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
    pageView->addProtectedChild(gradientR,ZORDER_UI);

    setupPages();
    
}

void panel_select::addNewPage(ssize_t page) {
    auto layout = Layout::create();
    layout->setContentSize(pageView->getContentSize());
    layout->setTag((s32)page+1);
    pages.pushBack(layout);
    pageView->addPage(layout);
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
        
        auto tag = (layoutid_t) (ID_SELECT_CHAR+c.id);
        
        uilordselect* lord;
        
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
        lord->setScale(phoneScale());
        lords.pushBack(lord);
        
        auto userdata = lord->userData();
        
        if ( mr->settings->keyboard_mode == CF_KEYBOARD::CLASSIC )
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
            dynamic_cast<uigrouplord*>(lord)->createFollowers();
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
    auto page = lord->getPage();
    
    s32 index = start;

    for(;;) {
    
        if ( getOverlappingLord(page,lord) == nullptr )
            break;
        
        page = calcPage(index);
        if ( page > pages.size() ) {
            if ( page < MaxPages ) {
                addNewPage(page);
            }else{
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

        g2 = ( c.followers ) ? SELECT_GRID_X : HALF(SELECT_GRID_X) ;

        int radius_added = g1 + g2 ;

        int dx = ABS(p1.x - p2.x);
        int dy = ABS(p1.y - p2.y);
        int distance = sqrt((dx*dx)+(dy*dy));

        if ( distance < radius_added )
            return target;
    }
    
    return nullptr ;
}

page_t panel_select::calcPage( u32 index ) const
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

    u32 r = index / MAX_COLUMNS ;
    u32 c = index % MAX_COLUMNS ;
    
    pos.x = (f32)(c * (SELECT_GRID_X+SELECT_GRID_X_LEADING)) + START_X;
    pos.y = (f32)(r * (SELECT_GRID_Y+SELECT_GRID_Y_LEADING)) + START_Y;
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
    
    if ( Character_IsInTunnel(c) && !filters.Is(select_filters::show_intunnel))
        show=false;

#if defined(_DDR_)
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
    // #40 Temporary fix
    button->setScale(phoneScale());
    button->setSelected(model->filters.Is(flag));
    button->addEventListener(eventCallback);
    uihelper::AddTopRight(safeArea, button, RES(PHONE_SCALE(16)), y );
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
    layoutid_t id = uihelper::getIdFromSender(sender);
     
    if ( id >= ID_SELECT_CHAR ) {
        gradientB->setVisible(false);
        gradientR->setVisible(false);
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
            auto parent = dynamic_cast<Widget*>(sender)->getParent();
            auto leader = dynamic_cast<uigrouplord*>(parent);
            
            storeAllLordsPositions();
            
            auto selectId = static_cast<layoutid_t>(leader->getTag()-ID_SELECT_CHAR);
            mr->disbandGroup(selectId);
            
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
#if _SHOW_CHARACTER_POSITIONS_
    for( auto e : lords ) {
        auto ud = e->userData();
        UIDEBUG( "%d %s = (%f,%f) %d", ud->id, ud->symbol.c_str(), e->getPosition().x, e->getPosition().y, e->getPage());
        
        if ( e->lordtype == lordwidget::group ) {
            auto parent = dynamic_cast<uigrouplord*>(e);
            for( auto c : parent->followers ) {
                auto u = c->userData();
                UIDEBUG( "    %d %s = (%f,%f) %d", u->id, u->symbol.c_str(), c->getPosition().x, c->getPosition().y, c->getPage());
            }
        }
    }
#endif
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
    RETURN_IF_NULL(dropTarget);
    
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
    RETURN_IF_NULL(dropTarget);
    
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
    
    f32 scale = phoneScale() ;
    
    auto lord = dynamic_cast<uilordselect*>(event->element);

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
            }
            
            draggedLord->setScale(scale_normal*scale);
            
            
            break;

        case DragEventType::stop:
        {
            // enable scrolling after dragging
            enableUI();
        
            draggedLord->setScale(scale_normal*scale);
            
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
            if(draggedLord!=nullptr)
            {
                checkValidDropTarget();
            
                checkPageFlip();
            }
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
        dynamic_cast<uigrouplord*>(dropTarget)->possibleSwap = false;
        dropTarget->refreshStatus();
    }
    
    auto newTarget = getDropTarget(draggedLord,MOUSE_OVER_OUTER,lordDropResult);
    
    if ( newTarget && newTarget->lordtype == lordwidget::group ) {
        if ( currentlyFollowing && currentlyFollowing == potentialLeader ) {
            dynamic_cast<uigrouplord*>(newTarget)->possibleSwap = lordDropResult != MOUSE_OVER_OUTER;
            newTarget->refreshStatus();
        }
    }
    
    
    if ( newTarget != dropTarget ) {
        
        
        // clear old target
        if ( dropTarget != nullptr ) {
            dropTarget->endDropTarget();
        }
        
        if ( newTarget != nullptr ) {
            
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
    
    f32 scale = phoneScale() ;
  
    if ( dropTarget == nullptr
        && (draggedLord->getPosition().x > getContentSize().width - (RIGHT_STRIP_WIDTH*scale)
            || draggedLord->getPosition().y < (BOTTOM_STRIP_HEIGHT*scale)) ) {
            
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
                dynamic_cast<uigrouplord*>(lord)->clearFollowers();
            }
        }
        getCharacters();
        
    }, 0, "delayed_refreshCharacters" );
    

}

uilordselect* panel_select::getDropTarget( uilordselect* lord, UIMOUSEOVER where, UIMOUSEOVER& result )
{
    if(lord==nullptr)
    {
        return nullptr;
    }

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
        for( auto child : dynamic_cast<uigrouplord*>(lord)->followers ) {
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
    f32 scale = phoneScale();
    
    if (position.x > size.width - ((RIGHT_STRIP_WIDTH*0.75)*scale) ) {
    
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
    if ( pages.empty())
        return;
    
    for (;;) {
        BREAK_IF(pages.empty());
        
        auto page = pages.at( pages.size()-1 );
        
        BREAK_IF ( !page->getChildren().empty() ) ;
        
        pages.popBack();
        pageView->removePage(page);
    }
}

#if defined(_FOCUS_ENABLED_)
std::vector<layoutid_t> panel_select::getFocusControls() const
{
    std::vector<layoutid_t> controls = {
        ID_LOOK,
        ID_NIGHT,
        ID_FILTER_DAWN,
        ID_FILTER_NIGHT,
        ID_FILTER_BATTLE,
        ID_FILTER_DEAD,
        ID_FILTER_CURRENT_LOC,
        ID_CLEANUP_SELECT
    };
    return controls;
}
#endif
