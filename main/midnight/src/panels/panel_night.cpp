
/*
 *  panel_night.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "panel_night.h"

#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"

#include "../frontend/layout_id.h"

USING_NS_CC;
USING_NS_CC_UI;
using namespace tme;

constexpr s32 NIGHT_DISPLAY_SLOW_DELAY_SECONDS = 2;

bool panel_night::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackground(_clrBlack);
    
    // Look Icon
    auto look = uihelper::CreateImageButton("i_dawn", ID_DAWN, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(10), RES(10) );
    uihelper::setEnabled(look, false);

    auto size = safeArea->getContentSize();
    
    scrollView = ScrollView::create();
    scrollView->setDirection(ScrollView::Direction::VERTICAL);
    scrollView->setLocalZOrder(ZORDER_FAR+1);
    
    // Description
    lblDescription = Label::createWithTTF( uihelper::font_config_big, "" );
    lblDescription->getFontAtlas()->setAntiAliasTexParameters();
    lblDescription->setTextColor(Color4B(_clrYellow));
    lblDescription->setLocalZOrder(ZORDER_DEFAULT);
    lblDescription->setWidth(size.width-RES(64));
    lblDescription->setAlignment(TextHAlignment::LEFT);
    lblDescription->setLineSpacing(0);
    lblDescription->setMaxLineWidth(size.width-RES(64));
    lblDescription->enableWrap(true);

    scrollView->addChild(lblDescription);
    safeArea->addChild(scrollView);
    
    setBackgroundToWidth("screens/misc/night.png");
    
    uishortcutkeys::init(safeArea, clickCallback);
    addShortcutKey(ID_DAWN, K_DAWN );
    
    return true;
}

void panel_night::OnShown()
{
    variables::sv_collate_battle_areas = !mr->settings->night_battle_full ;
    
    // Initialise in a thread
    auto atp = AsyncTaskPool::getInstance();
    atp->enqueue(AsyncTaskPool::TaskType::TASK_OTHER, [&]() {

        TME_Night(this);
        std::string msg = TME_LastActionMsg();
        
        RUN_ON_UI_THREAD([&,msg](){
            setNightText( msg );
            
            auto button = safeArea->getChildByTag<Button*>(ID_DAWN);
            uihelper::setEnabled(button, true);
            
        });

    });
    
    
}

void panel_night::OnNotification( Ref* sender )
{
    auto button = dynamic_cast<Widget*>(sender);
    if ( button == nullptr )
        return;
    
    auto id = static_cast<layoutid_t>(button->getTag());
    
    if ( id == ID_DAWN ) {
        mr->dawn();
    }
}

void panel_night::OnNightNotification ( callback_t* event )
{
    if ( event == nullptr ) {

        std::string msg = TME_LastActionMsg();
 
        RUN_ON_UI_THREAD([&,msg](){
            setNightText( msg );
        });

        if ( !mr->settings->night_display_fast )
            std::this_thread::sleep_for(std::chrono::seconds(NIGHT_DISPLAY_SLOW_DELAY_SECONDS));

    }else{
        if ( event->type == callback_t::gameover ) {
            RUN_ON_UI_THREAD([&](){
                mr->stories->save(savemode_dawn);
                mr->checkGameOverConditions();
            });
            return;
        }
    }
}


void panel_night::setNightText( const std::string& text )
{
    lblDescription->setString(text);
    
    auto size = this->getContentSize();
    auto textSize = lblDescription->getContentSize();
    
    f32 padding = RES(64);
    f32 textY = RES(32);
    f32 height = textY+textSize.height;
    bool enabled;
    
    if ( height >= size.height ) {
        height+=padding;
        enabled=true;
    }else{
        height=size.height;
        enabled=false;
    }
    
    scrollView->setEnabled(enabled);
    scrollView->setBounceEnabled(enabled);
    scrollView->setScrollBarEnabled(enabled);
    
    scrollView->setInnerContainerSize( Size(size.width,height) );
    scrollView->setContentSize(size);

    lblDescription->setAnchorPoint(uihelper::AnchorTopLeft);
    lblDescription->setPosition(RES(32), height-textY);
}
