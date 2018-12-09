
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


USING_NS_CC;
using namespace tme;
using namespace cocos2d::ui;



#if defined(_LOM_)
    #define BACKGROUND_COLOUR   _clrWhite
#endif
#if defined(_DDR_)
    #define BACKGROUND_COLOUR   _clrCyan
#endif


bool panel_select::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    uishortcutkeys::init(safeArea, clickCallback);
    
    f32 scale = resolutionmanager::getInstance()->phoneScale() ;
    
    SELECT_GRID_X = SELECT_ELEMENT_WIDTH * scale;
    SELECT_GRID_Y = SELECT_ELEMENT_HEIGHT * scale;
    SELECT_GRID_Y_LEADING = RES(1);
    SELECT_GRID_X_LEADING = RES(1);
    MAX_COLUMNS = 7;

    START_X = RES(16);
    START_Y = RES(16);
    
    setBackground(BACKGROUND_COLOUR);
    
  
    int r = DIS(64) * scale;
    
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(10), RES(10) );
    
    // Look Icon
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, clickCallback);
    uihelper::AddBottomRight(safeArea, night, RES(10), RES(10) );
    
    int adjy=RES(32)*scale;

    createFilterButton(ID_FILTER_DAWN,          (r*1)-adjy, "lord_dawn",    filter_show_dawn);
    createFilterButton(ID_FILTER_NIGHT,         (r*2)-adjy, "lord_night",   filter_show_night);
    createFilterButton(ID_FILTER_BATTLE,        (r*3)-adjy, "lord_battle",  filter_show_battle);
    createFilterButton(ID_FILTER_DEAD,          (r*4)-adjy, "f_dead!",      filter_show_dead);
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*5)-adjy, "i_center",     filter_show_current);
    
    auto cleanup = uihelper::CreateImageButton("i_cleanup", ID_CLEANUP_SELECT, clickCallback);
    uihelper::AddTopRight(safeArea, cleanup, RES(16), (r*6)-adjy );

    scrollView = ui::ScrollView::create();
    scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
    scrollView->setBounceEnabled(true);
    scrollView->setScrollBarEnabled(true);
    scrollView->setScrollBarPositionFromCornerForVertical(Vec2(RES(8),0));
//    scrollView->setBackGroundColor(_clrYellow);
//    scrollView->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    scrollView->setLocalZOrder(ZORDER_FAR);
    uihelper::AddTopLeft(safeArea, scrollView, START_X, START_Y);
    
    // calc size of scrollview
    auto size = safeArea->getContentSize();
    size.height -= (START_Y*2) + RES(64);
    size.width -= (START_X*2) + RES(64) ;
    SELECT_GRID_X_LEADING = (size.width-(MAX_COLUMNS*SELECT_GRID_X))/MAX_COLUMNS;
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

void panel_select::getCharacters()
{
    // TODO: This should be a TME_ or mr->
    variant args[3];
    args[0] = &characters ;
    args[1] = CMDG_LOYAL ;
    args[2] = MAKE_LOCID(loc.x,loc.y) ;
    mxi->GetProperties( "CharsForCommand", args, 3 );
    
    // calc position of last item in the grid
    // so that we have a canvas height
    auto pos = calcGridLocation(characters.Count()-1);
    f32 height = pos.y + SELECT_GRID_Y + RES(0);
    
    // adjust the inner container of the scroll view
    auto size = safeArea->getContentSize();
    f32 width = scrollView->getContentSize().width;
    
    // scroll?
    if ( height <= size.height ) {
        scrollView->setBounceEnabled(false);
        scrollView->setScrollBarEnabled(false);
        //height = size.height;
        scrollView->setContentSize(Size(width,height));
    }
    
    scrollView->setInnerContainerSize(Size(width,height));
    
    // now place all the lords
    character c;
    for ( u32 ii=0; ii<characters.Count(); ii++ ) {
        
        TME_GetCharacter(c,characters[ii]);
        if ( c.following )
            continue;
        
        auto pos = calcGridLocation(ii);
        pos.y = height-pos.y;
        
        layoutid_t tag = (layoutid_t) (ID_SELECT_CHAR+c.id);
        
        auto lord = uisinglelord::createWithLord(c.id);
        lord->setAnchorPoint(uihelper::AnchorTopLeft);
        lord->setIgnoreAnchorPointForPosition(false);
        lord->setPosition( pos);
        lord->setTag(tag);
        lord->addClickEventListener(clickCallback);
        scrollView->addChild(lord);
        
        auto userdata = (char_data_t*)TME_GetEntityUserData( c.id );
        
        if ( mr->config->keyboard_mode == CF_KEYBOARD_CLASSIC )
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_old));
        else
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(userdata->shortcut_new));

        
    }
    
    //scrollView->setInnerContainerPosition(Vec2(0,height));
 
}

// position from top left of canvas
Vec2 panel_select::calcGridLocation ( u32 index )
{
    f32 x = 0 ;
    f32 y = 0 ;
    
    int r = index / MAX_COLUMNS ;
    int c = index % MAX_COLUMNS ;
    
    x += c * (SELECT_GRID_X+SELECT_GRID_X_LEADING) ;
    y += r * (SELECT_GRID_Y+SELECT_GRID_Y_LEADING) ;
    
    return Vec2(x,y);
}


void panel_select::updateFilters()
{

    // setup lords
    character        c;
    for ( u32 ii=0; ii<characters.Count(); ii++ ) {
        
        TME_GetCharacter(c,characters[ii]);
        
        auto button = static_cast<uilordselect*>(scrollView->getChildByTag(ID_SELECT_CHAR+c.id));
        if ( button == NULL )
            continue;
        
        if ( c.following )
            continue;
        
        applyFilters( button, c );
    }
    
}

void panel_select::applyFilters ( uilordselect* e, character& c )
{
    auto filters = mr->config->select_filters;
    
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
    
    // TODO: Current Location
    // TODO: Current Selection
    
   // e->isCurrentSelection=FALSE;
   // e->isCurrentLocation=FALSE;
    
//    if ( filters.Is(filter_show_current) ) {
//        if ( c.id == TME_CurrentCharacter().id )
//            e->isCurrentSelection=TRUE;
//
//        if ( (c.location.x == TME_CurrentCharacter().location.x) && (c.location.y == TME_CurrentCharacter().location.y) )
//            e->isCurrentLocation=TRUE;
//
//#if defined(_DDR_)
//        if ( Character_IsInTunnel(c) != Character_IsInTunnel(TME_CurrentCharacter()))
//            e->isCurrentLocation = FALSE ;
//#endif
//    }
    
    e->setVisible(show);
}





uifilterbutton* panel_select::createFilterButton( layoutid_t id, s32 y, const std::string& image, select_filters flag )
{
    auto button = uifilterbutton::createWithImage(image);
    button->setTag(id);
    button->setScale(resolutionmanager::getInstance()->phoneScale());
    button->setSelected(mr->config->select_filters.Is(flag));
    button->addEventListener(eventCallback);
    uihelper::AddTopRight(safeArea, button, RES(16), y );
    return button;
}

void panel_select::updateFilterButton(Ref* sender,select_filters flag)
{
    mr->config->select_filters.Toggle(flag);
    auto button = static_cast<uifilterbutton*>(sender);
    if ( button != nullptr ) {
        button->addEventListener(nullptr);
        button->setSelected(mr->config->select_filters.Is(flag));
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
            
        }
            
            
        default:
            break;
    }
}
