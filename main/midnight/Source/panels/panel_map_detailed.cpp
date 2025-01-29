
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


USING_NS_AX;
USING_NS_AX_UI;
USING_NS_TME;

panel_map_detailed::panel_map_detailed() :
    scrollView(nullptr),
    characters(nullptr),
    descriptions(nullptr),
    tmxMap(nullptr),
    mapBuilder(nullptr),
    grouplord(nullptr),
    model(nullptr),
    groupLordBackground(nullptr),
    groupLordButton(nullptr)
{
}

panel_map_detailed::~panel_map_detailed()
{
    AX_SAFE_RELEASE_NULL(mapBuilder);
}


bool panel_map_detailed::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    uishortcutkeys::registerCallback(safeArea, clickCallback);
    
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

    for( int ii=0; ii<3; ii++ ) {
        auto circle = Sprite::createWithSpriteFrameName("circle_selector");
        circle->setScale(PHONE_SCALE(0.25f));
        circle->setLocalZOrder(ZORDER_UI);
        uihelper::AddTopLeft(safeArea, circle, RES(PHONE_SCALE(48)), RES(PHONE_SCALE(48+(ii*64))) );
        circle->setAnchorPoint(uihelper::AnchorCenter);
    }

    auto map_down = uihelper::CreateImageButton("map_scale_down_button", ID_DOWN, clickCallback);
    uihelper::AddTopLeft(safeArea, map_down, RES(PHONE_SCALE(48)), RES(PHONE_SCALE(48)) );
    map_down->setLocalZOrder(ZORDER_UI+1);
    map_down->setAnchorPoint(uihelper::AnchorCenter);

    auto map_reset = uihelper::CreateImageButton("i_group_disband", ID_RESET, clickCallback);
    uihelper::AddTopLeft(safeArea, map_reset, RES(PHONE_SCALE(48)), RES(PHONE_SCALE(48+64)) );
    map_reset->setLocalZOrder(ZORDER_UI+1);
    map_reset->setAnchorPoint(uihelper::AnchorCenter);

    auto map_up = uihelper::CreateImageButton("map_scale_up_button", ID_UP, clickCallback);
    uihelper::AddTopLeft(safeArea, map_up, RES(PHONE_SCALE(48)), RES(PHONE_SCALE(48+128)) );
    map_up->setLocalZOrder(ZORDER_UI+1);
    map_up->setAnchorPoint(uihelper::AnchorCenter);
  
    int adjy=RES(-16); 
    int r = RES(PHONE_SCALE(64));
    
    createFilterButton(ID_FILTER_CURRENT_LOC,   (r*0)-adjy, "i_center", map_filters::centre_char);
    createFilterButton(ID_FILTER_CRITTERS,      (r*1)-adjy, "i_critters", map_filters::show_critters);
    createFilterButton(ID_FILTER_LORDS,         (r*2)-adjy, "i_filter_lords", map_filters::show_lords);
#if defined(_DDR_)
    createFilterButton(ID_FILTER_TUNNELS,       (r*3)-adjy, "i_filter_tunnel", map_filters::show_tunnels);
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
    scrollView->setSwallowTouches(false);
    
    descriptions = Node::create();
    descriptions->setContentSize(tmxMap->getContentSize());
    scrollView->addChild(descriptions);
    
    characters = Node::create();
    characters->setContentSize(tmxMap->getContentSize());
    scrollView->addChild(characters);
      
    setupTooltip();

    setupCharacterButtons();
    
    setupStrongholds();
    
    setupPlaceLabels();
      
    updateFilters();
    
    updateScale();
    
    addShortcutKey(ID_FILTER_CURRENT_LOC,   KEYCODE(F1));
    addShortcutKey(ID_FILTER_CRITTERS,      KEYCODE(F2));
    addShortcutKey(ID_FILTER_LORDS,         KEYCODE(F3));
    addShortcutKey(ID_FILTER_TUNNELS,       KEYCODE(F4));
    
    addShortcutKey(ID_LOOK,                 K_LOOK);
    addShortcutKey(ID_LOOK,                 K_ESC);
    addShortcutKey(ID_LOOK,                 K_MAP);
    
    addShortcutKey(ID_DOWN,                 KEYCODE(DOWN_ARROW));
    addShortcutKey(ID_UP,                   KEYCODE(UP_ARROW));
              
    showHelpWindow(HELP_DISCOVERY_MAP);
    
    mapBuilder->clearLayers();
    
    return true;
}

void panel_map_detailed::setupTooltip()
{
    toolTip = Label::createWithTTF( uihelper::font_config_medium, "" );
    toolTip->setName("title");
    toolTip->setTextColor(Color4B(_clrWhite));
    toolTip->enableOutline(Color4B(_clrBlack),RES(2));
    toolTip->setLineSpacing(RES(-2));
    toolTip->getFontAtlas()->setAntiAliasTexParameters();
    toolTip->setAnchorPoint(uihelper::AnchorCenter);
    toolTip->setVisible(false);
    toolTip->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopCenter(safeArea, toolTip, RES(0), RES(32));
    addTouchListener();
}

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
                showGroupLord( button );
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
            
        case ID_RESET:
            {
                model->lastmapscale = model->mapscale;
                model->mapscale = INITIAL_MAP_SCALE;
                updateScale();
            }
            break;

        
        default:
            break;
    }
}

bool IsSingularTerrain(mxterrain_t terrain)
{
    switch (terrain) {
#if defined(_LOM_)
        case TN_CAVERN:
        case TN_CITADEL:
        case TN_HENGE:
        case TN_KEEP:
        case TN_LAKE:
        case TN_LITH:
        case TN_RUIN:
        case TN_SNOWHALL:
        case TN_TOWER:
            return true;
#endif
        
#if defined(_DDR_)
        case TN_CITY:
        case TN_FORTRESS:
        case TN_FOUNTAIN:
        case TN_GATE:
        case TN_HALL:
        case TN_HUT:
        case TN_PALACE:
        case TN_PIT:
        case TN_STONES:
        case TN_TEMPLE:
        case TN_WATCHTOWER:
            return true;
#endif
            
        default:
            return false;
    }
}

void panel_map_detailed::addTouchListener()
{
    // mouse events
    auto touchListener = EventListenerTouchOneByOne::create();
    
    // trigger when you push down
    touchListener->onTouchBegan = [=, this](Touch* touch, Event* event){
     
        auto loc = tmxMap->convertToNodeSpace(touch->getLocation());
        loc.y = tmxMap->getContentSize().height - loc.y;
        loc.x /= RES(64);
        loc.y /= RES(64);
        
        auto grid = mxgridref(loc.x+mapBuilder->loc_start.x, loc.y+mapBuilder->loc_start.y);
  
        tme::scenarios::exports::location_t l;
        TME_GetLocation(l, grid);
        
        
        bool showTooltip = false;
        
        if (IsSingularTerrain(l.terrain)) {
            showTooltip = l.flags.Is(lf_looked_at) || l.flags.Is(lf_visited)
                          || l.discovery_flags.Is(lf_looked_at) || l.discovery_flags.Is(lf_visited);
        }else{
            showTooltip = l.flags.Is(lf_looked_at) || l.flags.Is(lf_visited) || l.flags.Is(lf_seen)
              || l.discovery_flags.Is(lf_looked_at) || l.discovery_flags.Is(lf_visited) || l.discovery_flags.Is(lf_seen);
        }
        
        if ( showTooltip )
        {
            toolTip->setOpacity(ALPHA(alpha_zero));

            auto tip = StringExtensions::toUpper(TME_GetLocationText(grid));
            toolTip->setString(tip);

            if(!toolTip->isVisible()) {
                toolTip->setVisible(true);
                toolTip->stopAllActions();
                toolTip->runAction(FadeIn::create( 1.0f ));
            }
            
            return true;
        }
        
        return false;
    };
    
    // trigger when moving touch
    touchListener->onTouchMoved = [=](Touch* touch, Event* event){
    };
    
    // trigger when you let up
    touchListener->onTouchEnded = [=, this](Touch* touch, Event* event){
        if(toolTip->isVisible()) {
            toolTip->stopAllActions();
            toolTip->runAction(Sequence::create(
                                              FadeOut::create( 1.0f ),
                                              CallFunc::create( [this] { toolTip->setVisible(false); }),
                                              nullptr
                                              ));
        }
    };
    
    // Add listener
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, tmxMap);
    
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
    
    groupLordBackground->removeFromParent();
    groupLordBackground = nullptr;
    
    groupLordButton->setLocalZOrder(ZORDER_DEFAULT);
    groupLordButton->setVisible(true);
    groupLordButton = nullptr;
}

void panel_map_detailed::showGroupLord(Widget* button)
{
    groupLordButton = button;
    
    auto position = button->getPosition();
    auto object = static_cast<map_object*>(button->getUserData()) ;
    
    grouplord = uigrouplord::create();
    grouplord->setPosition(position);
    grouplord->setTag(ID_SELECT_ALL);
    grouplord->setAnchorPoint(uihelper::AnchorCenter);
    grouplord->addClickEventListener(clickCallback);
    grouplord->setScale(PHONE_SCALE(scale_normal));
    characters->addChild(grouplord);
    
    c_mxid  lords;
    lords.Add(object->id);
    for( auto m : object->here ) {
        lords.Add(m->id);
    }
    
    grouplord->createFollowers(lords);
    grouplord->setLocalZOrder(ZORDER_POPUP);
    
    groupLordBackground = DrawNode::create();
    auto size = grouplord->getContentSize();
    groupLordBackground->setContentSize( grouplord->getContentSize() );
    groupLordBackground->drawSolidCircle(Vec2(size.width/2,size.height/2), size.width/2 , 0, 64, 1.25f, 1.25f, Color4F(1.0f,1.0f,1.0f,0.9f));
    groupLordBackground->setAnchorPoint(uihelper::AnchorCenter);
    groupLordBackground->setPosition(Vec2(position.x,position.y));
    groupLordBackground->setVisible(true);
    groupLordBackground->setLocalZOrder(ZORDER_POPUP-1);
    characters->addChild(groupLordBackground);
    
    groupLordButton->setLocalZOrder(ZORDER_POPUP+1);
    groupLordButton->setVisible(false);
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
    button->setScale(phoneScale());
    button->setSelected(model->filters.Is(flag));
    button->addEventListener(eventCallback);
    uihelper::AddTopRight(safeArea, button, RES(PHONE_SCALE(16)), y );
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

    IF_NOT_NULL(tmxMap->getLayer("Tunnel Critters"))
        ->setVisible( model->filters.Is(map_filters::show_tunnels) && model->filters.Is(map_filters::show_critters) );
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
        
        node->setLocalZOrder(ZORDER_DEFAULT);
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
        auto opacity =  m->visible ? ALPHA(alpha_normal) : ALPHA(alpha_1qtr);
        
        
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
        title->setOpacity(opacity);
      
        descriptions->addChild(title);
        //title->setLocalZOrder(ZORDER_NEAR);
    }
        
}
