
/*
 *  panel_map_detailed.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_map_detailed.h"

#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"

#include "../frontend/layout_id.h"

#include "../Extensions/TMXTiledMap.h"
#include "../Utils/mapbuilder.h"
#include "../Utils/TiledMapper.h"

#include "../ui/characters/uisinglelord.h"

const f32 ScrollToLordTimeInSecs = 1.0f;


USING_NS_CC;
USING_NS_CC_UI;
using namespace tme;

panel_map_detailed::panel_map_detailed() :
    scrollView(nullptr),
    characters(nullptr),
    tmxMap(nullptr),
    mapBuilder(nullptr),
    model(nullptr)
{
}

bool panel_map_detailed::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    f32 scale = resolutionmanager::getInstance()->phoneScale() ;
    
    model = &mr->mapmodel;
    
    model->currentMapPanel = mr->panels->currentmode;
    
    setBackground(_clrGrey);
    
    scrollView = ScrollView::create();
    scrollView->setContentSize(getContentSize());
    addChild(scrollView);
    
    // Command Window
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(this, look, RES(10), RES(10) );
  
    auto map = uihelper::CreateImageButton("i_big_map", ID_MAP_OVERVIEW, clickCallback);
    uihelper::AddBottomRight(this, map, RES(10), RES(10) );
  
    
    int adjy=RES(32)*scale;
     int r = DIS(64) * scale;
    
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*1)-adjy, "i_center", map_filters::centre_char);
    createFilterButton(ID_FILTER_CRITTERS,      (r*2)-adjy, "i_critters", map_filters::show_critters);
#if defined(_DDR_)
    createFilterButton(ID_FILTER_TUNNELS,       (r*3)-adjy, "i_filter_tunnel", map_filters::show_tunnels);
#endif
    createFilterButton(ID_FILTER_LORDS,         (r*4)-adjy, "i_filter_lords", map_filters::show_lords);
    
    mapBuilder =  new (std::nothrow) mapbuilder();
    
    std::unique_ptr<TiledMapper> mapper( new TiledMapper );
    tmxMap = mapper->createTMXMap(mapBuilder->build());
    
    scrollView->addChild(tmxMap);
    scrollView->setInnerContainerSize( tmxMap->getContentSize() );
    scrollView->setDirection(ScrollView::Direction::BOTH);
    
    scrollView->setInnerContainerPosition(Vec2(model->oldoffset.x,model->oldoffset.y));
    
    characters = Node::create();
    characters->setContentSize(tmxMap->getContentSize());
    scrollView->addChild(characters);
    
    character c;
    
    for( auto m : mapBuilder->objects ) {
        
        TME_GetCharacter(c,m->id);
        auto pos = mapBuilder->convertToPosition(c.location);
        auto lord = uisinglelord::createWithLord(c.id);
        lord->setAnchorPoint(uihelper::AnchorCenter);
        lord->setPosition( Vec2(pos.x+RES(32),tmxMap->getContentSize().height-(pos.y+RES(32))) );
        lord->addClickEventListener(clickCallback);
        lord->setTag((layoutid_t) (ID_SELECT_CHAR+c.id));
        lord->setUserData(c.userdata);
        lord->status.Reset(LORD_STATUS::status_location);
        lord->refreshStatus();

        characters->addChild(lord);

    }
    
    mapBuilder->clearLayers();
    
    if ( model->filters.Is(map_filters::centre_char) )
        centreOnCurrentCharacter(false);
    
    updateFilters();
    
    showHelpWindow(HELP_DISCOVERY_MAP);
    
    return true;
}


void panel_map_detailed::OnNotification( Ref* sender )
{
    auto button = static_cast<Button*>(sender);
    if ( button == nullptr )
        return;

    auto pos = scrollView->getInnerContainerPosition();
    model->oldoffset = point( pos.x, pos.y);
    
    layoutid_t id = static_cast<layoutid_t>(button->getTag());
    
    switch ( id  )
    {
        case ID_LOOK:
            mr->settings->Save();
            mr->look();
            break;
            
        case ID_MAP_OVERVIEW:
        {
            mr->showPage(MODE_MAP_OVERVIEW);
            break;
        }
            
            
        case ID_FILTER_CURRENT_LOC:
            updateFilterButton(sender,map_filters::centre_char);
            if ( model->filters.Is(map_filters::centre_char))
                centreOnCurrentCharacter(true);
            break;
        case ID_FILTER_CRITTERS:
            updateFilterButton(sender,map_filters::show_critters);
            break;
        case ID_FILTER_TUNNELS:
            updateFilterButton(sender,map_filters::show_tunnels);
            break;
        case ID_FILTER_LORDS:
            updateFilterButton(sender,map_filters::show_lords);
            break;
 
            
        default:
            break;
    }
}

void panel_map_detailed::centreOnCharacter( character& c, bool animate )
{
    auto mapsize = tmxMap->getContentSize();
    auto size = getContentSize();
    
    // Calculate pixel position for centre of the lord
    // icon, and invert for y axis. This places the map
    // in the bottom left corner
    auto pos = mapBuilder->convertToPosition(c.location);
    auto offset = Vec2(((pos.x+RES(32))),(mapsize.height - (pos.y+RES(32)))) ;
    
    // Now pull the map up so that the middle of the icon is in
    // the centre of the screen
    auto x = offset.x - (size.width/2);
    auto y = offset.y - (size.height/2);
    
    // Check that the centre of the screen is valid and doesn't
    // leave us with the map not covering the screen
    if ( x+size.width > mapsize.width )
        x = mapsize.width - size.width;

    if ( x < 0 )
        x = 0;

    if ( y+size.height > mapsize.height )
        y = mapsize.height - size.height;

    if ( y < 0 )
        y = 0;
    
    offset = Vec2( -x, -y);
    
    // Show the result
    if ( animate ) {
        scrollView->scrollToPosition(offset, ScrollToLordTimeInSecs, true);
    }else{
        scrollView->setInnerContainerPosition(offset);
    }
}

void panel_map_detailed::centreOnCurrentCharacter(bool animate)
{
    centreOnCharacter( TME_CurrentCharacter(), animate );
}


uifilterbutton* panel_map_detailed::createFilterButton( layoutid_t id, s32 y, const std::string& image, map_filters flag )
{
    auto button = uifilterbutton::createWithImage(image);
    button->setTag(id);
    button->setScale(resolutionmanager::getInstance()->phoneScale());
    button->setSelected(model->filters.Is(flag));
    button->addEventListener(eventCallback);
    uihelper::AddTopRight(safeArea, button, RES(32), y );
    return button;
}

void panel_map_detailed::updateFilterButton(Ref* sender,map_filters flag)
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

void panel_map_detailed::updateFilters()
{
#if defined(_DDR_)
    tmxMap->getLayer("Tunnels")->setVisible( model->filters.Is(map_filters::show_tunnels) );
#endif

    tmxMap->getLayer("Critters")->setVisible( model->filters.Is(map_filters::show_critters) );
    
    characters->setVisible( model->filters.Is(map_filters::show_lords) );
}

