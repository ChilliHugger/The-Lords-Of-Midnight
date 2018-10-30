
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


USING_NS_CC;
using namespace tme;
using namespace cocos2d::ui;

s32            MAX_CHAR_PER_PAGE ;
s32            SELECT_GRID_X ;
s32            SELECT_GRID_Y ;
s32            SELECT_GRID_LEADING ;
s32            MAX_COLUMNS ;
s32            MAX_ROWS ;
s32            START_X ;
s32            START_Y ;

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
    
    setBackground(BACKGROUND_COLOUR);
    
    int c = RES(48);
    int r = RES(64);
    int h = RES(32);
    
    
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(this, look, RES(10), RES(10) );
    
    // Look Icon
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, clickCallback);
    uihelper::AddBottomRight(this, night, RES(10), RES(10) );
    
    int adjy=RES(32);
    //int adjx=RES(12);
    //int cx = (Width()-c)+adjx ;
    
    createFilterButton(ID_FILTER_DAWN,          (r*1)-adjy, "lord_dawn",    filter_show_dawn);
    createFilterButton(ID_FILTER_NIGHT,         (r*2)-adjy, "lord_night",   filter_show_night);
    createFilterButton(ID_FILTER_BATTLE,        (r*3)-adjy, "lord_battle",  filter_show_battle);
    createFilterButton(ID_FILTER_DEAD,          (r*4)-adjy, "f_dead!",      filter_show_dead);
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*5)-adjy, "i_center",     filter_show_current);
    
    auto cleanup = uihelper::CreateImageButton("i_cleanup", ID_CLEANUP_SELECT, clickCallback);
    uihelper::AddTopRight(this, cleanup, RES(10), (r*6)-adjy );

    
    return true;
}

uifilterbutton* panel_select::createFilterButton( layoutid_t id, s32 y, const std::string& image, select_filters flag )
{
    auto button = uifilterbutton::createWithImage(image);
    button->setTag(id);
    button->setSelected(mr->config->select_filters.Is(flag));
    button->addEventListener(eventCallback);
    uihelper::AddTopRight(this, button, RES(10), y );
    return button;
}

void panel_select::OnNotification( Ref* sender )
{
    auto button = static_cast<Button*>(sender);
    if ( button == nullptr )
        return;
    
    layoutid_t id = static_cast<layoutid_t>(button->getTag());
    
    switch ( id  )
    {
        case ID_LOOK:
            mr->look();
            break;

        case ID_FILTER_DAWN:
            mr->config->select_filters.Toggle(filter_show_dawn);
            updateFilters();
            break;
        case ID_FILTER_NIGHT:
            mr->config->select_filters.Toggle(filter_show_night);
            updateFilters();
            break;
        case ID_FILTER_BATTLE:
            mr->config->select_filters.Toggle(filter_show_battle);
            updateFilters();
            break;
        case ID_FILTER_DEAD:
            mr->config->select_filters.Toggle(filter_show_dead);
            updateFilters();
            break;
        case ID_FILTER_CURRENT_LOC:
            mr->config->select_filters.Toggle(filter_show_current);
            updateFilters();
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


void panel_select::updateFilters( void )
{
//    if ( i_lords )
//        i_lords->updateFilters( gl->settings.select_filters );
//
//    setFilterButton(i_filter_dawn, gl->settings.select_filters.Is(config::filter_show_dawn) );
//    setFilterButton(i_filter_night, gl->settings.select_filters.Is(config::filter_show_night) );
//    setFilterButton(i_filter_battle, gl->settings.select_filters.Is(config::filter_show_battle) );
//    setFilterButton(i_filter_dead, gl->settings.select_filters.Is(config::filter_show_dead) );
//    setFilterButton(i_filter_current_loc, gl->settings.select_filters.Is(config::filter_show_current) );
}
