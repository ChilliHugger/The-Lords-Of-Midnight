
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
    minMapScale(MAP_SCALE_MIN),
    groupLordBackground(nullptr),
    groupDismissListener(nullptr),
    groupLordButton(nullptr),
#if defined(_MOUSE_ENABLED_)
    shiftZooming(false)
#else
    pinchActive(false),
    pinchLastDistance(0.0f)
#endif
{
}

panel_map_detailed::~panel_map_detailed()
{
    removeGroupDismissListener();
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
  
    createFilterButton(ID_FILTER_CURRENT_LOC,   "i_center", map_filters::centre_char);
    createFilterButton(ID_FILTER_CRITTERS,      "i_critters", map_filters::show_critters);
    createFilterButton(ID_FILTER_LORDS,         "i_filter_lords", map_filters::show_lords);

    if (scenario_flags.Is(SF_TUNNELS)) {
        createFilterButton(ID_FILTER_TUNNELS,   "i_filter_tunnel", map_filters::show_tunnels);
        addShortcutKey(ID_FILTER_TUNNELS,       KEYCODE(F4));
    }
    
    auto contentsize = getContentSize();
    
    mapBuilder =  new (std::nothrow) mapbuilder();
    mapBuilder->screenAspect = contentsize.width / contentsize.height;
    mapBuilder->screenTiles = size( (u32)ceil(contentsize.width/RES(64)), (u32)ceil(contentsize.height/RES(64)) );

    if ( CONFIG(debug_map) ) {
        mapBuilder->setFlags(mapflags::debug_map);
        mapBuilder->setFlags(mapflags::show_all_characters);
        mapBuilder->setFlags(mapflags::show_all_critters);
    }

    std::unique_ptr<TiledMapper> mapper( new TiledMapper );
    tmxMap = mapper->createTMXMap(mapBuilder->build());

    // the map is built to match the screen's aspect ratio and to have enough
    // tiles to cover the screen at scale 1.0 (see mapbuilder::build), so the
    // scale at which it exactly fills the screen - and thus the least we
    // should ever zoom out to - is simply the screen width divided by its width
    auto mapContentSize = tmxMap->getContentSize();
    if ( mapContentSize.width>0.0f ) {
        f32 fitScale = contentsize.width / mapContentSize.width;
        minMapScale = std::min(INITIAL_MAP_SCALE, MAX(MAP_SCALE_MIN, fitScale));
    }
    model->mapscale = MAX(minMapScale, model->mapscale);

    scrollView->addChild(tmxMap);
    scrollView->setInnerContainerSize( tmxMap->getContentSize() );
    scrollView->setDirection(ScrollView::Direction::BOTH);
    scrollView->setInnerContainerPosition(Vec2(model->oldoffset.x,model->oldoffset.y));
    scrollView->setSwallowTouches(false);

#if !defined(_MOUSE_ENABLED_)
    addPinchZoomListener();
#endif

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

#if defined(_CITADEL_)
    if ( mr->questmodel.picking ) {
        auto stringId = mr->questmodel.quest == QS_GUARD ? SS_QUEST_PICK_GUARD : SS_QUEST_PICK_GOTO;
        auto hint = Label::createWithTTF( uihelper::font_config_medium, TME_GetSystemString(TME_CurrentCharacter(), stringId) );
        hint->setTextColor(Color4B(_clrWhite));
        hint->enableOutline(Color4B(_clrBlack),RES(2));
        hint->getFontAtlas()->setAntiAliasTexParameters();
        hint->setAnchorPoint(uihelper::AnchorCenter);
        hint->setLocalZOrder(ZORDER_DEFAULT);
        uihelper::AddBottomCenter(safeArea, hint, RES(0), RES(48));
    }
#endif

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
    
#if defined(_CITADEL_)
    // choosing a place for a quest: a lord's shield stands on a square like any other
    if ( mr->questmodel.picking && id >= ID_SELECT_CHAR ) {
        character lord;
        TME_GetCharacter(lord, id-ID_SELECT_CHAR);
        pickQuestPlace(lord.location);
        return;
    }
    if ( mr->questmodel.picking && id == ID_SELECT_ALL ) {
        pickQuestPlace(static_cast<map_object*>(button->getUserData())->location);
        return;
    }
#endif

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
#if defined(_CITADEL_)
            mr->questmodel.picking = false;
#endif
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

        case ID_FILTER_TUNNELS:
            updateFilterButton(sender,map_filters::show_tunnels);
            break;

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
            if ( model->mapscale > minMapScale)
            {
                model->lastmapscale = model->mapscale;
                model->mapscale -= MAP_SCALE_CLICK_DELTA;
                model->mapscale = std::max(minMapScale, model->mapscale);
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
#if defined(_LOM_) || defined(_CITADEL_)
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
     
        auto grid = gridAt(touch->getLocation());

#if defined(_CITADEL_)
        if ( mr->questmodel.picking )
            return true;
#endif
  
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
            if (CONFIG(debug_map)) {
                tip += StringUtils::format(" - (%d,%d)", grid.x, grid.y);
            }
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
#if defined(_CITADEL_)
        if ( mr->questmodel.picking ) {
            if ( touch->getLocation().distance(touch->getStartLocation()) <= RES(16) )
                pickQuestPlace(gridAt(touch->getLocation()));
            return;
        }
#endif
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

mxgridref panel_map_detailed::gridAt( const Vec2& location ) const
{
    auto loc = tmxMap->convertToNodeSpace(location);
    loc.y = tmxMap->getContentSize().height - loc.y;
    loc.x /= RES(64);
    loc.y /= RES(64);
    return mxgridref(loc.x+mapBuilder->loc_start.x, loc.y+mapBuilder->loc_start.y);
}

#if defined(_CITADEL_)
// the place chosen for the quest the quest page is waiting on
void panel_map_detailed::pickQuestPlace( mxgridref place )
{
    if ( !Character_SetQuest(TME_CurrentCharacter(), mr->questmodel.quest, MAKE_LOCID(place.x, place.y)) )
        return;
    mr->questmodel.picking = false;
    mr->questmodel.view = questview::quests;
    TME_RefreshCurrentCharacter();
    mr->showPage(MODE_QUEST);
}
#endif

#if defined(_MOUSE_ENABLED_)
bool panel_map_detailed::OnMouseMove( Vec2 pos )
{
    bool shiftHeld = (mr->keyboard->getModifierKeys() & kf_shift) != 0;

    if ( !shiftHeld || !mouseButtonDown ) {
        shiftZooming = false;
        return uipanel::OnMouseMove(pos);
    }

    if ( !shiftZooming ) {
        // just started - record the baseline, don't jump the scale yet
        shiftZooming = true;
        shiftZoomLastPos = pos;
        return true;
    }

    f32 delta = (pos.y - shiftZoomLastPos.y) / RES(MAP_SCALE_MOUSE_SENSITIVITY);
    shiftZoomLastPos = pos;

    if ( delta != 0.0f ) {
        model->lastmapscale = model->mapscale;
        model->mapscale = std::min(MAP_SCALE_MAX, std::max(minMapScale, model->mapscale+delta));
        updateScale();
    }

    return true;
}
#else
void panel_map_detailed::addPinchZoomListener()
{
    auto listener = EventListenerTouchAllAtOnce::create();

    auto updatePinch = [this](const std::vector<Touch*>& touches) {

        for ( auto touch : touches )
            pinchTouches[touch->getID()] = touch->getLocation();

        if ( pinchTouches.size() != 2 ) {
            pinchActive = false;
            return;
        }

        auto it = pinchTouches.begin();
        auto p1 = it->second; ++it;
        auto p2 = it->second;
        f32 distance = p1.distance(p2);

        if ( pinchActive && pinchLastDistance>0.0f ) {
            f32 ratio = distance / pinchLastDistance;
            model->lastmapscale = model->mapscale;
            model->mapscale = std::min(MAP_SCALE_MAX, std::max(minMapScale, model->mapscale*ratio));
            updateScale();
        }

        pinchActive = true;
        pinchLastDistance = distance;
    };

    listener->onTouchesBegan = [=](const std::vector<Touch*>& touches, Event* event) {
        updatePinch(touches);
    };

    listener->onTouchesMoved = [=](const std::vector<Touch*>& touches, Event* event) {
        updatePinch(touches);
    };

    listener->onTouchesEnded = [this](const std::vector<Touch*>& touches, Event* event) {
        for ( auto touch : touches )
            pinchTouches.erase(touch->getID());
        pinchActive = false;
    };
    listener->onTouchesCancelled = listener->onTouchesEnded;

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
#endif

void panel_map_detailed::updateScale()
{
    tmxMap->setScale(model->mapscale);
    descriptions->setScale(model->mapscale);
    characters->setScale(model->mapscale);

    auto scaledSize = tmxMap->getContentSize() * model->mapscale;
    scrollView->setInnerContainerSize( scaledSize );

    // the scrollview's inner container never shrinks smaller than the viewport, so once
    // zoomed out far enough that the map no longer fills it, centre the map (rather than
    // leaving it pinned to the bottom-left, its scale anchor) so unmapped space shows evenly
    auto viewSize = getContentSize();
    auto offset = Vec2( MAX(0.0f, (viewSize.width-scaledSize.width)*0.5f),
                         MAX(0.0f, (viewSize.height-scaledSize.height)*0.5f) );
    tmxMap->setPosition(offset);
    descriptions->setPosition(offset);
    characters->setPosition(offset);

    if ( model->filters.Is(map_filters::centre_char))
        centreOnCurrentCharacter(false);

}

void panel_map_detailed::removeGroupDismissListener()
{
    if ( groupDismissListener != nullptr ) {
        _eventDispatcher->removeEventListener(groupDismissListener);
        groupDismissListener = nullptr;
    }
}

void panel_map_detailed::hideGroupLord()
{
    removeGroupDismissListener();
    
    const f32 fadeTime = 0.15f;
    
    // fade the dialog out, and then remove it
    auto fadeAndRemove = [&](Node* node) {
        uihelper::SetCascadeOpacityRecursive(node);
        node->runAction( Sequence::createWithTwoActions( FadeOut::create(fadeTime), RemoveSelf::create() ) );
    };
    
    grouplord->setTouchEnabled(false);
    for ( auto follower : grouplord->followers ) {
        follower->setTouchEnabled(false);
    }
    fadeAndRemove(grouplord);
    grouplord = nullptr;
    
    // the radial gradient ignores the node opacity, so fade its colours
    _eventDispatcher->removeEventListenersForTarget(groupLordBackground);
    auto gradient = static_cast<LayerRadialGradient*>(groupLordBackground);
    auto startOpacity = gradient->getStartOpacity();
    gradient->runAction( Sequence::createWithTwoActions(
        ActionFloat::create(fadeTime, 1.0f, 0.0f, [gradient,startOpacity](float value) {
            gradient->setStartOpacity( (u8)(startOpacity * value) );
        }),
        RemoveSelf::create()
    ));
    groupLordBackground = nullptr;
    
    // and the button fades back in
    uihelper::SetCascadeOpacityRecursive(groupLordButton);
    groupLordButton->setLocalZOrder(ZORDER_NEAR);
    groupLordButton->setOpacity(0);
    groupLordButton->setVisible(true);
    groupLordButton->runAction( FadeIn::create(fadeTime) );
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
    
    // a white disc that fades out towards the edge
    auto size = grouplord->getContentSize() * 1.5f;
    f32 radius = HALF(size.width);
    groupLordBackground = LayerRadialGradient::create(
        Color4B(255,255,255,ALPHA(0.9f)), Color4B(255,255,255,ALPHA(0.0f)),
        radius, Vec2(radius,HALF(size.height)), 0.6f );
    groupLordBackground->setContentSize( size );
    groupLordBackground->setAnchorPoint(uihelper::AnchorCenter);
    // layers ignore their anchor point by default
    groupLordBackground->setIgnoreAnchorPointForPosition(false);
    groupLordBackground->setPosition(Vec2(position.x,position.y));
    groupLordBackground->setVisible(true);
    groupLordBackground->setLocalZOrder(ZORDER_POPUP-1);
    characters->addChild(groupLordBackground);
    
    // swallow touches in the group area so that lords behind the dialog can't be selected
    auto background = groupLordBackground;
    auto swallow = EventListenerTouchOneByOne::create();
    swallow->setSwallowTouches(true);
    swallow->onTouchBegan = [background](Touch* touch, Event*) {
        auto local = background->convertToNodeSpace(touch->getLocation());
        auto size = background->getContentSize();
        return local.distance(Vec2(HALF(size.width),HALF(size.height))) <= HALF(size.width);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(swallow, groupLordBackground);
    
    // a touch anywhere outside the dialog dismisses it. The touch is swallowed, so that
    // it doesn't also select a lord or drag the map, unless it is on one of the panel's buttons.
    auto dismiss = EventListenerTouchOneByOne::create();
    dismiss->setSwallowTouches(true);
    dismiss->onTouchBegan = [this, background](Touch* touch, Event*) {
        if ( grouplord == nullptr || !isRunning() )
            return false;
        
        auto local = background->convertToNodeSpace(touch->getLocation());
        auto size = background->getContentSize();
        if ( local.distance(Vec2(HALF(size.width),HALF(size.height))) <= HALF(size.width) )
            return false;
        
        // the listener can't be removed while it is running
        scheduleOnce( [this](float) {
            if ( grouplord != nullptr )
                hideGroupLord();
        }, 0, "dismiss_group_lord" );
        
        for ( auto child : safeArea->getChildren() ) {
            auto widget = dynamic_cast<Widget*>(child);
            if ( widget != nullptr && widget->isVisible() && widget->isEnabled()
                 && widget->hitTest(touch->getLocation(), Camera::getDefaultCamera(), nullptr) )
                return false;
        }
        return true;
    };
    _eventDispatcher->addEventListenerWithFixedPriority(dismiss, -1);
    groupDismissListener = dismiss;
    
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


uifilterbutton* panel_map_detailed::createFilterButton( layoutid_t id, const std::string& image, map_filters flag )
{
    auto button = uifilterbutton::createWithImage(image);
    button->setTag(id);
    // #40 temporary fix
    button->setScale(phoneScale());
    button->setSelected(model->filters.Is(flag));
    button->addEventListener(eventCallback);
    safeArea->addChild(button);
    filterButtons.push_back( {flag, button} );
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
    
    updateFilters(true);
}

static bool layerHasTiles( FastTMXLayer* layer )
{
    if ( layer == nullptr )
        return false;
    
    auto size = layer->getLayerSize();
    for ( s32 y=0; y<(s32)size.height; y++ ) {
        for ( s32 x=0; x<(s32)size.width; x++ ) {
            if ( layer->getTileGIDAt(Vec2(x,y)) != 0 )
                return true;
        }
    }
    return false;
}

void panel_map_detailed::updateFilters( bool animate )
{
    auto fade = [&](Node* node, bool show) {
        if ( node != nullptr )
            uihelper::FadeVisible(node, show, animate);
    };
    
    if (scenario_flags.Is(SF_TUNNELS)) {
        fade( tmxMap->getLayer("Tunnels"), model->filters.Is(map_filters::show_tunnels) );
        fade( tmxMap->getLayer("Tunnel Critters"), model->filters.Is(map_filters::show_tunnels) && model->filters.Is(map_filters::show_critters) );
    }

    fade( tmxMap->getLayer("Critters"), model->filters.Is(map_filters::show_critters) );
    fade( characters, model->filters.Is(map_filters::show_lords) );
    
    updateFilterButtons(animate);
}

// A filter button is only shown if there is something on the map for it to show or hide.
// The buttons shown are stacked from the top.
void panel_map_detailed::updateFilterButtons( bool animate )
{
    const s32 startY = RES(16);
    const s32 stepY = RES(PHONE_SCALE(64));
    
    s32 slot = 0;
    for ( auto& item : filterButtons ) {
        bool useful = true;
        
        switch ( item.first ) {
            case map_filters::show_critters:
                useful = layerHasTiles(tmxMap->getLayer("Critters"))
                      || layerHasTiles(tmxMap->getLayer("Tunnel Critters"));
                break;
            case map_filters::show_lords:
                useful = characters->getChildrenCount() > 0;
                break;
            case map_filters::show_tunnels:
                useful = layerHasTiles(tmxMap->getLayer("Tunnels"));
                break;
            default:
                break;
        }
        
        auto button = item.second;
        if ( !useful ) {
            button->setVisible(false);
            continue;
        }
        
        auto old = button->getPosition();
        bool wasVisible = button->isVisible();
        button->setVisible(true);
        uihelper::PositionParentTopRight(button, RES(PHONE_SCALE(16)), startY + (stepY*slot) );
        slot++;
        
        if ( animate && wasVisible && old != button->getPosition() ) {
            auto end = button->getPosition();
            button->setPosition(old);
            button->stopAllActions();
            button->runAction( EaseSineInOut::create( MoveTo::create(0.25f, end) ) );
        }
    }
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
            // always above the single lords
            node->setLocalZOrder(ZORDER_NEAR);
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
        
        if ( m->here.empty() )
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
