
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
//#include "../system/panelmanager.h"
#include "../ui/uihelper.h"
#include "../frontend/language.h"
#include "../ui/characters/uisinglelord.h"
#include "../ui/characters/uigrouplord.h"

USING_NS_CC;
using namespace tme;
using namespace cocos2d::ui;



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
    
    START_X = SELECT_GRID_X/2;
    START_Y = SELECT_GRID_Y/2;
    
    setBackground(BACKGROUND_COLOUR);
    
    auto backgroundColour = BACKGROUND_COLOUR;

    auto gradientB = uihelper::createVerticalGradient( backgroundColour, BOTTOM_STRIP_HEIGHT, BOTTOM_STRIP_HEIGHT/2, getContentSize().width, 1 );
    gradientB->setLocalZOrder(ZORDER_UI-1);
    addChild(gradientB);

    auto gradientR =  uihelper::createHorizontalGradient( backgroundColour, RIGHT_STRIP_WIDTH, RIGHT_STRIP_WIDTH/2, getContentSize().height, -1 );
    gradientR->setLocalZOrder(ZORDER_UI-1);
    uihelper::AddTopRight(this,gradientR);
    
    safeArea->setLocalZOrder(ZORDER_UI);
    
    
    
    int r = DIS(64) * scale;
    
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(16), RES(10) );
    
    // Look Icon
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, clickCallback);
    uihelper::AddBottomRight(safeArea, night, RES(0), RES(10) );
    
    int adjy=RES(32)*scale;

    createFilterButton(ID_FILTER_DAWN,          (r*1)-adjy, "lord_dawn",    filter_show_dawn);
    createFilterButton(ID_FILTER_NIGHT,         (r*2)-adjy, "lord_night",   filter_show_night);
    createFilterButton(ID_FILTER_BATTLE,        (r*3)-adjy, "lord_battle",  filter_show_battle);
    createFilterButton(ID_FILTER_DEAD,          (r*4)-adjy, "f_dead!",      filter_show_dead);
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*5)-adjy, "i_center",     filter_show_current);
    
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
    pageView->setDirection(ui::PageView::Direction::HORIZONTAL);
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

void panel_select::addNewPage(s32 ii) {
    auto page = Layout::create();
    page->setContentSize(pageView->getContentSize());
    page->setTag(ii+1);
    pages.pushBack(page);
    pageView->insertPage(page, ii);
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
    lord->setOpacity(0);
    lord->runAction(FadeIn::create(1.00f));
}

void panel_select::getCharacters()
{
    s32 usedIndex=0;
    
    lords.clear();
    
    model->updateCharacters();
    
    // now place all the lords
    character c;
    for ( u32 ii=0; ii<model->characters.Count(); ii++ )
    {
        TME_GetCharacter(c,model->characters[ii]);
        
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
        
        auto userdata = static_cast<char_data_t*>(c.userdata);
        
        if ( mr->config->keyboard_mode == CF_KEYBOARD_CLASSIC )
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_old));
        else
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_new));

        // only add positions for lords that already
        // have a position
        if ( userdata->select_loc != PointZero && userdata->select_page > InvalidPage ) {
            Vec2 pos;
            pos.x = userdata->select_loc.x;
            pos.y = userdata->select_loc.y;
            addToPage(lord, userdata->select_page, pos);
        }
        
        if ( c.followers ) {
            static_cast<uigrouplord*>(lord)->createFollowers();
        }
        
        applyFilters( lord, c );
        
    }
    
    // now position all the lords that haven't already been added
    // to the scrollview
    
    for ( auto node : lords )
    {
        auto lord = static_cast<uilordselect*>(node);
        
        auto userdata = static_cast<char_data_t*>(lord->getUserData());
        
        if ( userdata->select_loc == PointZero ) {
        
            s32 index = usedIndex;
            s32 page = calcPage(index);
            
            if ( userdata->select_page < InvalidPage ) {
                page = userdata->select_page * -1;
                index = (page-1) * MAX_LORDS_PER_PAGE;
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
    
    removeEmptyEndPages();
}

void panel_select::addToPage( uilordselect* lord, s32 page, Vec2 pos)
{
    lord->setPage(page);
    lord->setPosition(pos);
    pages.at(page-1)->addChild(lord);
}

uilordselect* panel_select::getLordElement( mxid id )
{
//    int tag = ID_SELECT_CHAR+id;
//    for (const auto child : lords)
//    {
//        if(child && child->getTag() == tag)
//            return static_cast<uilordselect*>(child);
//    }
    return nullptr;
}

s32 panel_select::getLordPage( uilordselect* lord  ) {

    if ( lord->lordtype == lordtype_grouped) {
        return InvalidPage;
    }
    
    return lord->getParent()->getTag();
}

void panel_select::checkCollision ( uilordselect* lord, s32 start )
{
    uilordselect* overlap;
    
    s32 page = lord->getPage(); //getLordPage(lord);
    
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

uilordselect* panel_select::getOverlappingLord ( s32 page, uilordselect* source )
{
    character c;
    
    mxid id = getIdFromTag(source);
    TME_GetCharacter(c, id );

    int g2=SELECT_GRID_X/2;
    int g1 = ( c.followers ) ? SELECT_GRID_X : SELECT_GRID_X/2 ;
 
    auto pageNode = pages.at(page-1);
    
    for ( Node* node : pageNode->getChildren() )
    {
        auto target = dynamic_cast<uilordselect*>(node);
        
        CONTINUE_IF_NULL(target);
        CONTINUE_IF( target == source );
        CONTINUE_IF( getLordPage(target) != page);
        
        auto p2 = target->getPosition();
        auto p1 = source->getPosition();
        
        mxid id2 = getIdFromTag(target);
        
        TME_GetCharacter(c,id2 );
        CONTINUE_IF(c.following);
        
        g2 = ( c.followers ) ? SELECT_GRID_X : SELECT_GRID_X/2 ;
        
        int radius_added = RES(g1) + RES(g2) ;
        
        int dx = ABS(p1.x - p2.x);
        int dy = ABS(p1.y - p2.y);
        int distance = sqrt((dx*dx)+(dy*dy));
        
        if ( distance < radius_added )
            return target;
    }
    
    return nullptr ;
}

s32 panel_select::calcPage( u32 index )
{
    return (index / MAX_LORDS_PER_PAGE) +1 ;
}

// position from top left of canvas
Vec2 panel_select::calcGridLocation ( u32 index )
{
    Vec2 pos = Vec2::ZERO ;
    
    // find page
    s32 page = calcPage(index);
    
    index = index - ((page-1) * MAX_LORDS_PER_PAGE);
    
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
    for ( auto node : lords )
    {
        auto button = static_cast<uilordselect*>(node);
        
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
    if ( Character_IsNight(c) && !filters.Is(filter_show_night) )
        show=false;
    if ( Character_IsDawn(c) && !filters.Is(filter_show_dawn))
        show=false;
    if ( Character_IsInBattle(c) && !filters.Is(filter_show_battle) )
        show=false;
    if ( Character_IsDead(c) && !filters.Is(filter_show_dead))
        show=false;
    
#if defined(_DDR_)
    if ( Character_IsInTunnel(c) && !filters.Is(filter_show_intunnel))
        show=false;
    if ( Character_IsPreparingForBattle(c) && !filters.Is(filter_show_battle) )
        show=false;
#endif
    
    if ( !filters.Is(filter_show_current) && e->status.Is(LORD_STATUS::status_location) )
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
    auto button = static_cast<uifilterbutton*>(sender);
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
            mr->look();
            break;
            
        case ID_FILTER_DAWN:
            updateFilterButton(sender,filter_show_dawn);
            break;
        case ID_FILTER_NIGHT:
            updateFilterButton(sender,filter_show_night);
            break;
        case ID_FILTER_BATTLE:
            updateFilterButton(sender,filter_show_battle);
            break;
        case ID_FILTER_DEAD:
            updateFilterButton(sender,filter_show_dead);
            break;
        case ID_FILTER_CURRENT_LOC:
            updateFilterButton(sender,filter_show_current);
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
    for( auto node : lords ) {
        auto button = static_cast<uilordselect*>(node);
        addToPage(button, calcPage(index), calcGridLocation(index));
        checkCollision(button,0);
        
        // adjust for big group circle
        if ( button->lordtype == lordtype_group ) {
            auto pos = button->getPosition();
            f32 adjustX = SELECT_ELEMENT_WIDTH/2;
            if ( pos.x-adjustX <= 0 )
                pos.x += adjustX;
            
            pos.y -= SELECT_ELEMENT_HEIGHT/2;
            button->setPosition(pos);
        }

        storeLordPosition(button);
        index++;
    }
}

void panel_select::storeAllLordsPositions()
{
    showCharacterPositions();

    for ( auto node : lords ) {
        auto lord = static_cast<uilordselect*>(node);
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
    for( auto node : lords ) {
        auto e = static_cast<uilordselect*>(node);
        auto ud = static_cast<char_data_t*>(e->getUserData());
        UIDEBUG( "%d %s = (%f,%f) %d", ud->id, ud->symbol.c_str(), e->getPosition().x, e->getPosition().y, e->getPage());
        
        if ( e->lordtype == lordtype_group ) {
            auto parent = static_cast<uigrouplord*>(e);
            for( auto child : parent->followers ) {
                auto c = static_cast<uilordselect*>(child);
                auto u = static_cast<char_data_t*>(c->getUserData());
                UIDEBUG( "    %d %s = (%f,%f) %d", u->id, u->symbol.c_str(), c->getPosition().x, c->getPosition().y, c->getPage());
            }
        }
        
    }
}

void panel_select::placeDraggedLordOnCurrentPage()
{
    s32 currentPage = (s32)pageView->getCurrentPageIndex();
    pages.at(currentPage)->addChild(draggedLord);
    draggedLord->setPage(currentPage+1);
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
        s32 currentPage = (s32)pageView->getCurrentPageIndex();
        draggedLord->setPosition(event->position);
        draggedLord->setPage(currentPage+1);
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
            draggedLord->setPage(dropTarget->getPage()*-1);
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
        s32 currentPage = (s32)pageView->getCurrentPageIndex();
        draggedLord->setPage(currentPage+1);
        draggedLord->setPosition(dropTarget->getPosition());
        dropTarget->setPage(draggedLord->getPage()*-1);
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
    auto lord = static_cast<uilordselect*>(event->element);

    mxid draggedLordId = getIdFromTag(lord);
    
    character        c;
    Vec2 droppedAt;

    switch (event->type) {
            
        case uidragevent::drageventtype::select:
            
            // get intial position
            startDragParent = lord->getParent();
            startDragPosition = lord->getPosition();
            startDragPage = lord->getPage();
            
            // disable scrolling while dragging
            disableUI();
            break;
            
        case uidragevent::drageventtype::start:
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
                draggedLord->setScale(1.0f);
            }
            
            break;

        case uidragevent::drageventtype::stop:
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
            
        case uidragevent::drageventtype::drag:
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
    
    if ( dropTarget && dropTarget->lordtype == lordtype_group ) {
        static_cast<uigrouplord*>(dropTarget)->possibleSwap = false;
        dropTarget->refreshStatus();
    }
    
    auto newTarget = getDropTarget(draggedLord,MOUSE_OVER_OUTER,lordDropResult);
    
    if ( newTarget && newTarget->lordtype == lordtype_group ) {
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
    startDragPage *= -1;
    pageView->scrollToPage(startDragPage-1);
    
}

void panel_select::draggedLordReturnsToPage()
{
    draggedLord->setPosition(startDragPosition);
    pages.at(startDragPage-1)->addChild(draggedLord);
    draggedLord->setPage(startDragPage);
    pageView->scrollToPage(startDragPage-1);
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
        for ( auto node : lords ) {
            auto lord = static_cast<uilordselect*>(node);
            lord->removeFromParent();
            // store children
            if ( lord->lordtype == lordtype_group ) {
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
    
    s32 currentPage = (s32)pageView->getCurrentPageIndex()+1;
    
    for ( auto element : lords ) {
        
        CONTINUE_IF(element==lord);

        auto target = static_cast<uilordselect*>(element);

        CONTINUE_IF( target->getPage() != currentPage );
        
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

void panel_select::storeLordPosition( uilordselect* lord ) {
    char_data_t* userdata = static_cast<char_data_t*>( lord->getUserData() );
    if ( userdata != nullptr ) {
        auto position = lord->getPosition();

        if ( lord->getPage() <= InvalidPage ) {
            //auto parent = static_cast<uilordselect*>(lord->getParent());
            //userdata->select_page = parent->getPage()*-1;
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
    if ( lord->lordtype == lordtype_group ) {
        for( auto child : static_cast<uigrouplord*>(lord)->followers ) {
            storeLordPosition(static_cast<uilordselect*>(child));
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
    auto size = this->getContentSize();
    auto index = pageView->getCurrentPageIndex();
    
    if (position.x > size.width - (RIGHT_STRIP_WIDTH*0.75) ) {
    
        if ( index == pages.size()-1 && index < MaxPages ) {
            addNewPage((s32)index+1);
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
    for (;;) {
        auto page = pages.at( pages.size()-1 );
        if ( page->getChildren().size() != 0 ) {
            break;
        }
        pages.popBack();
        pageView->removePage(page);
    }
}
