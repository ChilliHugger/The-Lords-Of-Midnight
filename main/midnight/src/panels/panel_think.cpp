
/*
 *  panel_think.cpp
 *  midnight
 *
 *  Created by Chris Wild on 10/10/2018.
 *  Copyright (c) 2018 Chilli Hugger Software. All rights reserved.
 *
 */

#include "../cocos.h"
#include "panel_think.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"
#include "../ui/uithinkpage.h"
#include "../frontend/layout_id.h"

USING_NS_CC;
USING_NS_CC_UI;
using namespace tme;

// think pages are
// person
//        think current character
//        think characters at location
//        think characters infront

// place
//      DDR: normal think
//        think stronghold
//        think stronghold infront
//        foe armies
//        foe armies in front

// army
//        think current character
//        think characters at location
//        think characters infront
//        think stronghold
//        think stronghold infront
//        foe armies
//        foe armies in front



bool panel_think::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    setBackground(_clrWhite);
    
    // Let's create a pageview
    createPageView();
    
    // Command Window
    // Look Icon
    auto look = uihelper::CreateImageButton("i_look", ID_LOOK, clickCallback);
    uihelper::AddBottomLeft(safeArea, look, RES(10), RES(10) );
    
    // TAB ICONS
    f32 scale = resolutionmanager::getInstance()->phoneScale() ;
    int x = RES(10) * scale ;
    int c = RES(100) * scale;
    
    // Person
    auto person = uihelper::CreateImageButton("think_person", ID_THINK_PERSON, clickCallback);
    uihelper::AddBottomRight(safeArea, person, x+c*0, RES(10) );
    
    // Place
    auto place = uihelper::CreateImageButton("think_place", ID_THINK_PLACE, clickCallback);
    uihelper::AddBottomRight(safeArea, place, x+c*1, RES(10) );
    
    // Army
    auto army = uihelper::CreateImageButton("think_army", ID_THINK_ARMY, clickCallback);
    uihelper::AddBottomRight(safeArea, army, x+c*2, RES(10) );
    
    // Battle
    auto battle = uihelper::CreateImageButton("think_battle", ID_THINK_BATTLE, clickCallback);
    uihelper::AddBottomRight(safeArea, battle, x+c*3, RES(10) );
    
    // Stronghold
    // Postmen
    // Recruitmen
    
    uishortcutkeys::init(safeArea, clickCallback);
    addShortcutKey(ID_LOOK          ,K_LOOK);
    addShortcutKey(ID_THINK_PERSON  ,K_PERSON);
    addShortcutKey(ID_THINK_PLACE   ,K_PLACE);
    addShortcutKey(ID_THINK_ARMY    ,K_ARMY);
    addShortcutKey(ID_THINK_BATTLE  ,K_BATTLE);
    
//    addShortcutKey(ID_APPROACH      ,K_APPROACH);
//#if defined(_LOM_)
//    addShortcutKey(ID_FIGHT         ,K_FIGHT);
//    addShortcutKey(ID_UNHIDE        ,K_UNHIDE);
//#endif
//    addShortcutKey(ID_GROUP_LEAVE   ,K_LEAVE);
//    addShortcutKey(ID_GROUP_DISBAND ,K_DISBAND);
    
    return true;
}

void panel_think::showButton(layoutid_t id, bool enabled)
{
    auto button = safeArea->getChildByTag<Button*>(id);
    if ( button )
        button->setVisible(enabled);
}

void panel_think::enableButton(layoutid_t id, bool enabled)
{
    auto button = safeArea->getChildByTag<Button*>(id);
    uihelper::setEnabled(button, enabled);
}

void panel_think::tintButton(layoutid_t id,Color3B colour)
{
    auto button = safeArea->getChildByTag<Button*>(id);
    if ( button ) {
        button->setEnabled(false);
        button->setBright(true);
        button->setColor(colour);
    }
}


void panel_think::enableButtons()
{
    auto index = pageView->getCurrentPageIndex();
    auto page = pages.at(index);
    
    showButton(ID_GROUP_DISBAND, page->disband );
    showButton(ID_GROUP_LEAVE, page->leave );
    
    // postmen
    // recruitmen
    
    
}

void panel_think::setObject(mxid id)
{
    // Add a page
    
    character& c = TME_CurrentCharacter();
    
    this->id = c.id;
    this->objectId = id;
    
    setupPages();
    
    // build this list
    u32 index=0;
    for( uithinkpage* page : pages ) {
        
        auto layout = Layout::create();
        layout->setContentSize(pageView->getContentSize());
        uihelper::AddCenter(layout, page);
        uihelper::FillParent(page);
        
        pageView->insertPage(layout, index++);
    }
    
    // first page
    enableButtons();
    
    // has a battle happend?
    enableButton(ID_THINK_BATTLE, Character_HasBattleInfo(c) );
    enableButton(ID_THINK_PLACE, true );

    // check for strongholds
    // for check place
    TME_GetCharacterLocationInfo ( c );
#if defined(_LOM_)
    if ( (location_strongholds.Count() == 0) && (location_infront_strongholds.Count() == 0) )
        enableButton(ID_THINK_PLACE, false );
#endif
    
    // dead or alive?
    if ( Character_IsDead(c) ) {
        showButton(ID_THINK_ARMY, false );
        showButton(ID_THINK_PERSON, false );
        showButton(ID_THINK_BATTLE, false );
        showButton(ID_THINK_PLACE, false );
    }
    
//    CLEARARRAY( enabled_tabs );
//    if ( i_army->isEnabled() )
//        enabled_tabs[1]=MODE_THINK_ARMY;
//    if ( i_person->isEnabled() )
//        enabled_tabs[3]=MODE_THINK_PERSON;
//    if ( i_place->isEnabled() )
//        enabled_tabs[2]=MODE_THINK_PLACE;
//    if ( i_battle->isEnabled() )
//        enabled_tabs[0]=MODE_THINK_BATTLE;

    current_tab=0;
 
    switch ( currentmode ) {
        case MODE_THINK_ARMY:
            current_tab=1;
            tintButton(ID_THINK_ARMY, _clrYellow);
            return;
        case MODE_THINK_PERSON:
        case MODE_THINK_APPROACH:
            current_tab=3;
            tintButton(ID_THINK_PERSON, _clrYellow);
            return;
        case MODE_THINK_PLACE:
            current_tab=2;
            tintButton(ID_THINK_PLACE, _clrYellow);
            return;
        case MODE_THINK_BATTLE:
            current_tab=0;
            tintButton(ID_THINK_BATTLE, _clrYellow);
            return;
        default:
            break;
    }
    
    
}

void panel_think::createPageView()
{
    // lets have a pageview
    pageView = PageView::create();
    pageView->setDirection(PageView::Direction::HORIZONTAL);
    pageView->setBounceEnabled(true);
    pageView->setIndicatorEnabled(true);
    pageView->setCurrentPageIndex(0);
    pageView->setIndicatorIndexNodesColor(_clrBlack);
    pageView->setIndicatorSelectedIndexColor(_clrBlue);
    
    pageView->setIndicatorIndexNodesScale(CONTENT_SCALE(0.25f));
    pageView->setIndicatorSpaceBetweenIndexNodes(CONTENT_SCALE(-5));
    
    uihelper::AddBottomLeft(this, pageView);
    uihelper::FillParent(pageView);
    
    auto padding = resolutionmanager::getInstance()->getSafeArea();
    pageView->setIndicatorPosition( Vec2(pageView->getContentSize().width/2,padding.bottom + RES(10)) );
    
    pageView->addEventListener( [&]( Ref* sender, PageView::EventType e){
        if ( e == PageView::EventType::TURNING ) {
            UIDEBUG("Page turning");
            this->enableButtons();
        }
    });

}

void panel_think::addPage( mxid id )
{
    uithinkpage* page = uithinkpage::create();
    page->setCallback(clickCallback);
    page->setObject(id, objectId, currentmode);
    pages.pushBack(page);
}

void panel_think::setupPages()
{
    u32 ii;
    character   c;
    character   c1;
    c_mxid collection;

    TME_GetCharacter(c, id);
    
    currentPage=0;
    pages.clear();
    
    if ( Character_IsDead(c) ) {
        addPage(c.id);
        return;
    }
    
    TME_GetCharacterLocationInfo ( c );
    flags = location_flags;
    
    if ( currentmode == MODE_THINK_PERSON || currentmode == MODE_THINK_ARMY || currentmode == MODE_THINK_APPROACH ) {
        addPage(c.id);
        
        
        u32 temp;
        TME_GetCharacters ( c.id,collection,temp );
        
        // persons here
        for ( ii=0; ii<collection.Count(); ii++ ) {
            if ( collection[ii] != c.id ) {
                addPage(collection[ii]);
#if defined(_LOM_)
                if ( currentmode==MODE_THINK_APPROACH ) {
                    TME_GetCharacter(c1, collection[ii]);
                    if ( !Character_IsRecruited(c1) ) {
                        currentPage=pages.size()-1;
                    }
                }
#endif
            }
        }
        
        // persons ahead
        TME_GetCharacters ( location_infrontid, collection, temp );
        for ( ii=0; ii<collection.Count(); ii++ ) {
            addPage(collection[ii]);
#if defined(_DDR_)
            if ( currentmode==MODE_THINK_APPROACH ) {
                TME_GetCharacter(c1, collection[ii]);
                if ( !Character_IsRecruited(c1) ) {
                    currentPage=pages.size()-1;
                }
            }
#endif
            
        }
    }
    
    if ( currentmode == MODE_THINK_PLACE || currentmode == MODE_THINK_ARMY ) {
        
#if defined(_DDR_)
        if ( currentmode == MODE_THINK_PLACE )
            addPage(c.id);
#endif
        
        // current stronghold
        if ( location_strongholds.Count() )
            addPage(location_strongholds[0]);
        
        // stronghold ahead
        variant args[2];
        args[0] = &collection ;
        args[1] = location_infrontid ;
        mxi->GetProperties ( "STRONGHOLDS", args, 2 );
        if ( collection.Count() )
            addPage(collection[0]);
        
        // armies here
        if ( location_armies.regiment_warriors+location_armies.regiment_riders ) {
            addPage(1);
        }
        
        // armies ahead
        if ( location_infront_armies.regiment_warriors+location_infront_armies.regiment_riders ) {
            addPage(2);
        }
        
    }
    
    //SetupUIElements();
    if ( pages.size() == 0  ) {
        addPage(c.id);
    }
}

void panel_think::OnNotification( Ref* sender )
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
            
        case ID_GROUP_LEAVE:
        {
            if ( mr->leaveGroup(TME_CurrentCharacter().id) ) {
                mr->think();
            }
            break;
        }
            
        case ID_GROUP_DISBAND:
        {
            if ( mr->disbandGroup(TME_CurrentCharacter().id) ) {
                mr->think();
            }
            break;
        }
            
        case ID_RECRUITMEN:
        {
            if ( mr->recruitMen() )
                return;
            break;
        }
        case ID_POSTMEN:
        {
            if ( mr->postMen() )
                return;
            break;
        }
            
#if defined(_LOM_)
        case ID_UNHIDE:
        {
            mr->hideunhide();
        }
            
        case ID_FIGHT:
        {
            mr->fight();
            break;
        }
#endif
            
        case ID_APPROACH:
        {
            mr->approach();
            break;
        }
            
        case ID_THINK_PERSON:
        {
            mr->showPage(MODE_THINK_PERSON);
            break;
        }
            
        case ID_THINK_PLACE:
        {
            mr->showPage(MODE_THINK_PLACE);
            break;
        }
            
        case ID_THINK_ARMY:
        {
            mr->showPage(MODE_THINK_ARMY);
            break;
        }
            
        case ID_THINK_BATTLE:
        {
            mr->showPage(MODE_THINK_BATTLE);
            break;
        }
            
        default:
            break;
    }
    
    uipanel::OnNotification(sender);
    
}
