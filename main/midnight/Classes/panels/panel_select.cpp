
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

static const point PointZero(0,0);


bool panel_select::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    model = &mr->selectmodel;
    dropTarget = nullptr;
    draggedLord = nullptr;
    
    uishortcutkeys::init(safeArea, clickCallback);
    
    f32 scale = resolutionmanager::getInstance()->phoneScale() ;
    
    SELECT_GRID_X = SELECT_ELEMENT_WIDTH * scale;
    SELECT_GRID_Y = SELECT_ELEMENT_HEIGHT * scale;
    SELECT_GRID_Y_LEADING = RES(1);
    SELECT_GRID_X_LEADING = RES(1);
    MAX_COLUMNS = 6;

    START_X = SELECT_GRID_X/2;
    START_Y = SELECT_GRID_Y/2;
    
    setBackground(BACKGROUND_COLOUR);
    
  
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

    scrollView = ui::ScrollView::create();
    scrollView->setDirection(ui::ScrollView::Direction::BOTH);
    scrollView->setBounceEnabled(true);
    scrollView->setScrollBarEnabled(true);
    scrollView->setScrollBarPositionFromCornerForVertical(Vec2(RES(8),0));
scrollView->setBackGroundColor(_clrYellow);
scrollView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    scrollView->setLocalZOrder(ZORDER_FAR);
    uihelper::AddTopLeft(safeArea, scrollView, RES(128), 0);
    
    // calc size of scrollview
    auto size = safeArea->getContentSize();
    size.width -= DIS(256);
    //SELECT_GRID_X_LEADING = 0; //(size.width-START_X-(MAX_COLUMNS*SELECT_GRID_X))/MAX_COLUMNS;
    
    scrollView->setInnerContainerSize( size );
    scrollView->setContentSize( size );

    getCharacters();
    
    addShortcutKey(ID_FILTER_DAWN,       KEYCODE(F1));
    addShortcutKey(ID_FILTER_NIGHT,      KEYCODE(F2));
    addShortcutKey(ID_FILTER_BATTLE,     KEYCODE(F3));
    addShortcutKey(ID_FILTER_DEAD,       KEYCODE(F4));
    addShortcutKey(ID_FILTER_CURRENT_LOC,KEYCODE(F5));
    
    addShortcutKey(ID_LOOK,              K_LOOK);
    addShortcutKey(ID_NIGHT,             K_NIGHT);
    
    return true;
}

void panel_select::setInitialScrollViewHeight()
{
    //auto pos = calcGridLocation(model->characters.Count()-1);
    //f32 height = pos.y + SELECT_GRID_Y + RES(0);

    f32 height = 1024*4;
    f32 width = 1024*4;
    updateScrollViewSize(width,height);
}

void panel_select::resizeScrollView()
{
    f32 maxy=0;
    f32 miny=999999;
    f32 maxx=0;
    f32 minx=999999;
    
    f32 previousHeight = scrollView->getInnerContainerSize().height;
    f32 previousWidth = scrollView->getInnerContainerSize().width;
    
    for( auto node : scrollView->getChildren() ) {
        auto e = dynamic_cast<uilordselect*>(node);
        CONTINUE_IF_NULL(e);
        
        f32 y = e->getPosition().y;
        if ( y > maxy ) maxy = y;
        if ( y < miny ) miny = y;

        f32 x = e->getPosition().x;
        if ( x > maxx ) maxx = x;
        if ( x < minx ) minx = x;
    }
    
    f32 height = (maxy - miny) ; //+ SELECT_GRID_Y;
    f32 width = (maxx - minx) ; //+ SELECT_GRID_Y;
    
    updateScrollViewSize(width,height);
    
    height = scrollView->getInnerContainerSize().height;
    width = scrollView->getInnerContainerSize().width;

    
    f32 diffH =  previousHeight - height;
    f32 diffW =  previousWidth - width;
    
    for( auto node : scrollView->getChildren() ) {
        auto e = dynamic_cast<uilordselect*>(node);
        CONTINUE_IF_NULL(e);
        
        f32 y = e->getPosition().y;
        e->setPositionY(y-diffH);
        
        f32 x = e->getPosition().x;
        e->setPositionX(x-diffW);
    }
    
//    auto pos = scrollView->getInnerContainerPosition();
//    pos.y+=diff;
//    scrollView->setInnerContainerPosition(pos);

    updateScrollViewSize(width,height);
}

void panel_select::updateScrollViewSize( f32 width, f32 height )
{
    // adjust the inner container of the scroll view
    auto size = safeArea->getContentSize();
    
    //f32 width = scrollView->getContentSize().width;
 
    
    // scroll?
    if ( height <= size.height ) {
        height = size.height;
    }

    if ( width <= size.width ) {
        width = size.width;
    }
    
    if ( width == size.width && height == size.height ) {
        scrollView->setBounceEnabled(false);
        scrollView->setScrollBarEnabled(false);
    }else{
        scrollView->setBounceEnabled(true);
        scrollView->setScrollBarEnabled(true);
    }
    
    
    //scrollView->setContentSize(Size(size.width,size.height));
    scrollView->setInnerContainerSize(Size(width,height));
}

void panel_select::getCharacters()
{
    lords.clear();
    
    model->updateCharacters();
    
    // calc position of last item in the grid
    // so that we have a canvas height
    setInitialScrollViewHeight();
    
    f32 height = 0;
    f32 width = 0;
    
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
        scrollView->addChild(lord);
        lords.pushBack(lord);
        
        auto userdata = static_cast<char_data_t*>(c.userdata);
        
        if ( mr->config->keyboard_mode == CF_KEYBOARD_CLASSIC )
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_old));
        else
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_new));

        // only add positions for lords that already
        // have a position
        if ( userdata->select_loc != PointZero ) {
            Vec2 pos;
            pos.x = userdata->select_loc.x;
            pos.y = userdata->select_loc.y;
            lord->setPosition( pos);
            if ( pos.y > height )
                height = pos.y;
            if ( pos.x > width )
                width = pos.x;
        }
        
        if ( c.followers ) {
            static_cast<uigrouplord*>(lord)->createFollowers();
        }
        
        applyFilters( lord, c );
        
    }

    updateScrollViewSize(width,height);
    
    // now position all the lords that haven't already been added
    // to the scrollview
    for ( u32 ii=0; ii<model->characters.Count(); ii++ )
    {
        TME_GetCharacter(c,model->characters[ii]);
        CONTINUE_IF ( c.following );
        
        auto lord = getLordElement(c.id);
        
        auto userdata = static_cast<char_data_t*>(c.userdata);
        if ( userdata->select_loc == PointZero ) {
            lord->setPosition(calcGridLocation(ii));
            checkCollision(lord);
            storeLordPosition(lord);
        }
    }
    
    resizeScrollView();
}

uilordselect* panel_select::getLordElement( mxid id )
{
    return static_cast<uilordselect*>(scrollView->getChildByTag(ID_SELECT_CHAR+id));
}

void panel_select::checkCollision ( uilordselect* lord )
{
    uilordselect* overlap;
    if ( (overlap=getOverlappingLord(lord)) != nullptr ) {
        for ( int ii=0; ii<500; ii++ ) {
            lord->setPosition( calcGridLocation(ii) );
            
            if ( (overlap=getOverlappingLord(lord)) == nullptr )
                break;
        }
    }
    
}

uilordselect* panel_select::getOverlappingLord ( uilordselect* source )
{
    character c;
    
    // TODO: ui frig to stop collision with look button

    mxid id = getIdFromTag(source);
    TME_GetCharacter(c, id );

    int g2=SELECT_GRID_X/2;
    int g1 = ( c.followers ) ? SELECT_GRID_X : SELECT_GRID_X/2 ;
    
    for ( auto id : model->characters )
    {
        auto target = getLordElement(id);
        
        CONTINUE_IF_NULL( target );
        CONTINUE_IF( target == source );
        CONTINUE_IF( !target->isEnabled() );
        CONTINUE_IF( target->getTag() == 0 );
        
        mxid id2 = getIdFromTag(target);
        
        TME_GetCharacter(c,id2 );
        CONTINUE_IF(c.following);
        
        g2 = ( c.followers ) ? SELECT_GRID_X : SELECT_GRID_X/2 ;
        
        auto p2 = target->getPosition();
        auto p1 = source->getPosition();
        
        int radius_added = RES(g1) + RES(g2) ;
        
        int dx = ABS(p1.x - p2.x);
        int dy = ABS(p1.y - p2.y);
        int distance = sqrt((dx*dx)+(dy*dy));
        
        if ( distance < radius_added )
            return target;
    }
    
    return nullptr ;
}

// position from top left of canvas
Vec2 panel_select::calcGridLocation ( u32 index )
{
    Vec2 pos = Vec2::ZERO ;
    
    int r = index / MAX_COLUMNS ;
    int c = index % MAX_COLUMNS ;
    
    pos.x = (c * (SELECT_GRID_X+SELECT_GRID_X_LEADING)) + START_X;
    pos.y = (r * (SELECT_GRID_Y+SELECT_GRID_Y_LEADING)) + START_Y;
    pos.y = scrollView->getInnerContainerSize().height-pos.y;
    
    return pos;
}


void panel_select::updateFilters()
{
    character        c;
    for ( auto id : model->characters )
    {
        auto button = getLordElement(id);
        CONTINUE_IF_NULL( button );
   
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
                mr->night();
            });
            break;
        }
        
        case ID_CLEANUP_SELECT:
            resetPositions();
            break;
            
        case ID_GROUP_DISBAND:
        {
            auto parent = static_cast<Button*>(sender)->getParent();
            auto leader = static_cast<uigrouplord*>(parent);
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
    setInitialScrollViewHeight();
    
    character c;
    for ( u32 ii=0; ii<model->characters.Count(); ii++ ) {
        
        TME_GetCharacter(c,model->characters[ii]);
        CONTINUE_IF(c.following);
        
        auto button = getLordElement(c.id);
        CONTINUE_IF_NULL(button);

        button->setPosition(calcGridLocation(ii));
        checkCollision(button);
        storeLordPosition(button);
    
    }
    resizeScrollView();
}

void panel_select::OnDeActivate()
{
    uipanel::OnDeActivate();
    
    for( auto node : scrollView->getChildren() ) {
        auto lord = dynamic_cast<uilordselect*>(node);
        CONTINUE_IF_NULL(lord);
        storeLordPosition(lord);
    }
    
}

void panel_select::showCharacterPositions()
{
    for( auto node : scrollView->getChildren() ) {
        auto e = dynamic_cast<uilordselect*>(node);
        CONTINUE_IF_NULL(e);
        auto ud = static_cast<char_data_t*>(e->getUserData());
        UIDEBUG( "%s = (%f,%f)", ud->symbol.c_str(), e->getPosition().x, e->getPosition().y);
    }
}

void panel_select::OnDragDropNotification( uidragelement* sender, uidragevent* event )
{
    auto lord = static_cast<uilordselect*>(event->element);

    mxid draggedLordId = getIdFromTag(lord);
    
    character        c;
    Vec2 droppedAt;

    switch (event->type) {
        case uidragevent::drageventtype::start:
            // clear drop target
            dropTarget = nullptr;
            draggedLord = lord;
            TME_GetCharacter(c, draggedLordId );
            currentlyFollowing = c.following;
            potentialLeader = IDT_NONE;
            
            // turn off canvas scrolling
            scrollView->setTouchEnabled(false);
            
            break;

        case uidragevent::drageventtype::stop:
        {
            // turn on scrolling
            scrollView->setTouchEnabled(true);
            
            if ( dropTarget == nullptr && currentlyFollowing == IDT_NONE ) {
                draggedLord = nullptr;
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
            

            if ( currentlyFollowing ) {
                
                if ( potentialLeader == IDT_NONE ) {
                    mr->leaveGroup(draggedLordId);
                    draggedLord->setPosition(event->position);
                    storeLordPosition(draggedLord);
                    refreshCharacters();
                }
                else if ( potentialLeader == currentlyFollowing ) {
                    mr->swapGroupLeader(draggedLordId);
                    refreshCharacters();
                }
                else {
                    if ( mr->leaveGroup(draggedLordId) ) {
                        mr->joinGroup(draggedLordId, potentialLeader);
                        refreshCharacters();
                    }
                }
            }
            else {
                if ( potentialLeader != IDT_NONE ) {
                    if ( mr->joinGroup(draggedLordId, potentialLeader) ) {
                        //mr->look();
                        refreshCharacters();
                    }
                }
                
            }
            
            dropTarget = nullptr;
            draggedLord = nullptr;
            break;
        }
            
        case uidragevent::drageventtype::drag:
        {
            UIMOUSEOVER result;
            
            auto newTarget = getDropTarget(lord,MOUSE_OVER_INNER,result);
            
            if ( newTarget != dropTarget ) {
                
                // clear old target
                if ( dropTarget != nullptr ) {
                    dropTarget->endDropTarget();
                    potentialLeader = IDT_NONE;
                }
                
                if ( newTarget != NULL ) {
                    
                    potentialLeader = getIdFromTag(newTarget);
                    
                    mxid id = getIdFromTag(lord);
                    TME_GetCharacter(c,id );
                
                    if ( Character_CanFollow(c,potentialLeader) ) {
                        newTarget->startDropTarget();
                    }else{
                        newTarget=nullptr;
                    }

                }
            }
            
            dropTarget = newTarget;
            
            // check
            auto position = draggedLord->getPosition();
            auto size = scrollView->getContentSize();
            if (position.x > size.width - RES(16) ) {
                resizeCanvas(0,0,RES(16),0);
            }
            else if (position.x < RES(16) ) {
                resizeCanvas(RES(16),0,0,0);
            }
            
            break;
        }
            
        default:
            break;
    }
}

void panel_select::refreshCharacters() {

    this->scheduleOnce( [&](float) {

        for ( auto element : lords ) {
            scrollView->removeChild(element);
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
    
    if ( lord->lordtype == lordtype_grouped ) {
        pos = lord->getWorldPosition();
    }
    
    for ( auto element : lords ) {
        
        CONTINUE_IF(element==lord);
        
        auto target = static_cast<uilordselect*>(element);
        
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
    auto position = lord->getPosition();
    if ( userdata != nullptr ) {
        userdata->select_loc.x = position.x;
        userdata->select_loc.y = position.y;
    }
}


void panel_select::resizeCanvas( s32 left, s32 top, s32 right, s32 bottom ) {
    
    auto size = scrollView->getInnerContainerSize();
    
    s32 xAdj=0;
    s32 yAdj=0;
    
    if ( left ) {
        size.width += left;
        xAdj = left ;
    }
    if ( right ) {
        size.width += right;
        xAdj = 0 ;
    }
    
    for( auto node : lords ) {
        CONTINUE_IF(node == draggedLord);
        
        auto p = node->getPosition();
        p.x += xAdj;
        p.y += yAdj;
        node->setPosition(p);
    }
    
    scrollView->setInnerContainerSize(size);
    //resizeScrollView();
}
