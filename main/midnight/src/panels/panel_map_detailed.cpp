
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
#include "../system/configmanager.h"
#include "../ui/uihelper.h"

#include "../frontend/layout_id.h"

#include "../extensions/TMXTiledMap.h"
#include "../utils/mapbuilder.h"
#include "../utils/TiledMapper.h"

#include "../ui/characters/uisinglelord.h"
#include "../ui/characters/uigrouplord.h"

#include <cmath>

const f32 ScrollToLordTimeInSecs = 1.0f;


USING_NS_CC;
USING_NS_CC_UI;
using namespace tme;

panel_map_detailed::panel_map_detailed() :
    scrollView(nullptr),
    characters(nullptr),
    descriptions(nullptr),
    tmxMap(nullptr),
    mapBuilder(nullptr),
    grouplord(nullptr),
    model(nullptr)
{
}

panel_map_detailed::~panel_map_detailed()
{
    CC_SAFE_RELEASE_NULL(mapBuilder);
}


bool panel_map_detailed::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    model = &mr->mapmodel;
    
    model->currentMapPanel = mr->panels->currentmode;
    
    setBackground(_clrGrey);
    
    scrollView = ScrollView::create();
    scrollView->setContentSize(getContentSize());
    addChild(scrollView);
    
    // Command Window
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(10), RES(10) );
  
    auto map = uihelper::CreateImageButton("i_big_map", ID_MAP_OVERVIEW, clickCallback);
    uihelper::AddBottomRight(safeArea, map, RES(10), RES(10) );

    auto circle1 = Sprite::createWithSpriteFrameName("circle_selector");
    circle1->setScale(PHONE_SCALE(0.25f));
    circle1->setLocalZOrder(ZORDER_FAR);
    uihelper::AddTopLeft(safeArea, circle1, RES(48), RES(PHONE_SCALE(48)) );
    circle1->setAnchorPoint(uihelper::AnchorCenter);

    auto circle2 = Sprite::createWithSpriteFrameName("circle_selector");
    circle2->setScale(PHONE_SCALE(0.25f));
    circle2->setLocalZOrder(ZORDER_FAR);
    uihelper::AddTopLeft(safeArea, circle2, RES(48), RES(PHONE_SCALE(48+64)) );
    circle2->setAnchorPoint(uihelper::AnchorCenter);

    auto map_down = uihelper::CreateImageButton("map_scale_down_button", ID_DOWN, clickCallback);
    uihelper::AddTopLeft(safeArea, map_down, RES(48), RES(PHONE_SCALE(48)) );
    map_down->setAnchorPoint(uihelper::AnchorCenter);
  
    auto map_up = uihelper::CreateImageButton("map_scale_up_button", ID_UP, clickCallback);
    uihelper::AddTopLeft(safeArea, map_up, RES(48), RES(PHONE_SCALE(48+64)) );
    map_up->setAnchorPoint(uihelper::AnchorCenter);
  
    int adjy=RES(32); //*scale;
    int r = RES(64); // * scale;
    
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*1)-adjy, "i_center", map_filters::centre_char);
    createFilterButton(ID_FILTER_CRITTERS,      (r*2)-adjy, "i_critters", map_filters::show_critters);
    createFilterButton(ID_FILTER_LORDS,         (r*3)-adjy, "i_filter_lords", map_filters::show_lords);
#if defined(_DDR_)
    createFilterButton(ID_FILTER_TUNNELS,       (r*4)-adjy, "i_filter_tunnel", map_filters::show_tunnels);
#endif
    
    auto contentsize = getContentSize();
    
    mapBuilder =  new (std::nothrow) mapbuilder();
    mapBuilder->screensize = size(ceil(getContentSize().width/RES(64)),
                                ceil(getContentSize().height/RES(64)));
    
    if ( mr->config->debug_map ) {
        mapBuilder->setFlags(mapflags::debug_map);
        mapBuilder->setFlags(mapflags::show_all_characters);
        mapBuilder->setFlags(mapflags::show_all_critters);
    }

    std::unique_ptr<TiledMapper> mapper( new TiledMapper );
    tmxMap = mapper->createTMXMap(mapBuilder->build());
    
    scrollView->addChild(tmxMap);
    scrollView->setInnerContainerSize( tmxMap->getContentSize() );
    scrollView->setDirection(ScrollView::Direction::BOTH);
    scrollView->setInnerContainerPosition(Vec2(model->oldoffset.x,model->oldoffset.y));
    
    descriptions = Node::create();
    descriptions->setContentSize(tmxMap->getContentSize());
    scrollView->addChild(descriptions);
    
    characters = Node::create();
    characters->setContentSize(tmxMap->getContentSize());
    scrollView->addChild(characters);
    
//    uihelper::font_config_debug.outlineSize = 10;
//    debug_label = Label::createWithTTF(uihelper::font_config_debug, "Debug Text");
//    debug_label->getFontAtlas()->setAntiAliasTexParameters();
//    debug_label->setTextColor(Color4B::BLACK);
//    debug_label->enableOutline(Color4B(255,255,255,255));\
//    debug_label->setLocalZOrder(ZORDER_DEFAULT);
//    uihelper::AddTopCenter(safeArea, debug_label, RES(0), RES(32));
    
    
    setupCharacterButtons();
    
    setupStrongholds();
    
    setupPlaceLabels();
      
    updateFilters();
    
    updateScale();
    
    showHelpWindow(HELP_DISCOVERY_MAP);
    
    mapBuilder->clearLayers();
    
    //scheduleUpdate();
    
    return true;
}

//void panel_map_detailed::update(float delta)
//{
//char debug_output[256];
//
//    auto pos = scrollView->getInnerContainerPosition();
//    sprintf(debug_output, "Offets (%f,%f)", pos.x,pos.y);
//    debug_label->setString(debug_output);
//
//    uipanel::update(delta);
//}

void panel_map_detailed::OnNotification( Ref* sender )
{
    auto button = dynamic_cast<Widget*>(sender);
    if ( button == nullptr )
        return;

    auto pos = scrollView->getInnerContainerPosition();
    model->oldoffset = point( (int)pos.x, (int)pos.y);
    
    auto id = static_cast<layoutid_t>(button->getTag());
    
    if ( id >= ID_SELECT_CHAR ) {
        mxid characterId = id-ID_SELECT_CHAR;
        mr->selectCharacter(characterId);
        return;
    }
    
    
    switch ( id  )
    {
        case ID_GROUP_DISBAND:
            hideGroupLord();
            break;
            
        case ID_SELECT_ALL:
        {
            if ( grouplord != nullptr )
                hideGroupLord();
            else
                showGroupLord( button->getPosition(), static_cast<map_object*>(button->getUserData()) );
            break;
        }
            
        case ID_LOOK:
            mr->settings->Save();
            mr->look();
            break;
            
        case ID_MAP_OVERVIEW:
            mr->showPage(MODE_MAP_OVERVIEW);
            break;
            
        case ID_FILTER_CURRENT_LOC:
            updateFilterButton(sender,map_filters::centre_char);
            if ( model->filters.Is(map_filters::centre_char))
                centreOnCurrentCharacter(true);
            break;
        case ID_FILTER_CRITTERS:
            updateFilterButton(sender,map_filters::show_critters);
            break;
#if defined(_DDR_)
        case ID_FILTER_TUNNELS:
            updateFilterButton(sender,map_filters::show_tunnels);
            break;
#endif
        case ID_FILTER_LORDS:
            updateFilterButton(sender,map_filters::show_lords);
            break;
    
        case ID_UP:
            if ( model->mapscale < MAP_SCALE_MAX)
            {
                model->lastmapscale = model->mapscale;
                model->mapscale += MAP_SCALE_CLICK_DELTA;
                model->mapscale = std::min(MAP_SCALE_MAX, model->mapscale);
                updateScale();
            }
            break;
            
        case ID_DOWN:
            if ( model->mapscale > MAP_SCALE_MIN)
            {
                model->lastmapscale = model->mapscale;
                model->mapscale -= MAP_SCALE_CLICK_DELTA;
                model->mapscale = std::max(MAP_SCALE_MIN, model->mapscale);
                updateScale();
            }
            break;
        
        
        default:
            break;
    }
}


void panel_map_detailed::updateScale()
{
    tmxMap->setScale(model->mapscale);
    descriptions->setScale(model->mapscale);
    characters->setScale(model->mapscale);
    scrollView->setInnerContainerSize( tmxMap->getContentSize() * model->mapscale );
    
    if ( model->filters.Is(map_filters::centre_char))
        centreOnCurrentCharacter(false);

}

void panel_map_detailed::hideGroupLord()
{
    grouplord->removeFromParent();
    grouplord = nullptr;
}

void panel_map_detailed::showGroupLord(Vec2 position, map_object* object)
{
    grouplord = uigrouplord::create();
    grouplord->setPosition(position);
    grouplord->setTag(ID_SELECT_ALL);
    grouplord->setAnchorPoint(uihelper::AnchorCenter);
    grouplord->addClickEventListener(clickCallback);
    grouplord->setScale(scale_normal);
    characters->addChild(grouplord);
    
    c_mxid  lords;
    lords.Add(object->id);
    for( auto m : object->here ) {
        lords.Add(m->id);
    }
    
    grouplord->createFollowers(lords);
    
}

void panel_map_detailed::centreOnCharacter( character& c, bool animate )
{
    auto mapsize = tmxMap->getContentSize() * model->mapscale;
    auto size = getContentSize();
    
    f32 adjust = RES(32) * model->mapscale;
    
    // Calculate pixel position for centre of the lord
    // icon, and invert for y axis. This places the map
    // in the bottom left corner
    auto pos = mapBuilder->convertToPosition(c.location) * model->mapscale;
    auto offset = Vec2(((pos.x+adjust)),(mapsize.height - (pos.y+adjust))) ;
    
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
    // #40 temporary fix
    //button->setScale(phoneScale());
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
    IF_NOT_NULL(tmxMap->getLayer("Tunnels"))
        ->setVisible( model->filters.Is(map_filters::show_tunnels) );
#endif

    IF_NOT_NULL(tmxMap->getLayer("Critters"))
        ->setVisible( model->filters.Is(map_filters::show_critters) );
    
    characters->setVisible( model->filters.Is(map_filters::show_lords) );
}

void panel_map_detailed::setupCharacterButtons()
{
    character c;
    
    // clear the processed flag
    for( auto m : mapBuilder->characters ) {
        m->processed = false;
    }
    
    for( auto m : mapBuilder->characters ) {
        
        CONTINUE_IF(m->processed);
        
        TME_GetCharacter(c,m->id);
        auto pos = mapBuilder->convertToPosition(c.location);
        
        Widget* node;
        
        // others at this location?
        if ( !m->here.empty() ) {
            
            node = uihelper::CreateImageButton("map_lords_many", ID_SELECT_ALL, clickCallback);
            node->setUserData(m);
            node->setScale(scale_normal);
            for ( auto n : m->here ) {
                n->processed = true;
            }
            
        } else {
            auto lord = uisinglelord::createWithLord(c.id);
            lord->status.Reset(LORD_STATUS::status_location);
            lord->refreshStatus();
            lord->setUserData(c.userdata);
            lord->setTag((layoutid_t) (ID_SELECT_CHAR+c.id));
            lord->setScale(scale_normal);
            node = lord;
            
        }
        
        node->setAnchorPoint(uihelper::AnchorCenter);
        node->setPosition( Vec2(pos.x+RES(32),tmxMap->getContentSize().height-(pos.y+RES(32))) );
        node->addClickEventListener(clickCallback);


        m->processed = true;
        characters->addChild(node);
        
    }
}

void panel_map_detailed::setupStrongholds()
{
    
}

void panel_map_detailed::setupPlaceLabels()
{
    for( auto m : mapBuilder->places )
    {
        CONTINUE_IF( !m->here.empty() );
        
        auto pos = mapBuilder->convertToPosition(m->location);
        
        auto name = TME_GetLocationText(m->location);
        
        // set name label
        auto title = Label::createWithTTF( uihelper::font_config_small, name );
        title->setName("title");
        title->setTextColor(Color4B(_clrWhite));
        title->enableOutline(Color4B(_clrBlack),RES(1));
        title->setLineSpacing(RES(-2));
        title->setHeight(RES(32));
        title->getFontAtlas()->setAntiAliasTexParameters();
        title->setAnchorPoint(uihelper::AnchorCenter);
        title->setWidth(RES(128));
        title->setPosition( Vec2(pos.x+RES(32),tmxMap->getContentSize().height-(pos.y+RES(48))) );
        title->setHorizontalAlignment(TextHAlignment::CENTER);
        title->setVerticalAlignment(TextVAlignment::BOTTOM);
      
        descriptions->addChild(title);
        //title->setLocalZOrder(ZORDER_NEAR);
    }
        
}
