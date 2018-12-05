
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "panel_mainmenu.h"
#include "panel_credits.h"
#include "panel_options.h"

#include "../system/moonring.h"

#include "../system/keyboardmanager.h"
#include "../system/storymanager.h"
#include "../system/helpmanager.h"
#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/language.h"

#include "../ui/uitextmenu.h"
#include "../ui/uihelper.h"
#include "../ui/uioptionitem.h"

#include "../ui/uibook.h"
#include "../ui/uibookmenu.h"

USING_NS_CC;
using namespace cocos2d::ui;



static uitextmenuitem items[] = {
    { ID_NEW_STORY,         {"NEW STORY"},     KEYCODE(N), "N" },
    { ID_CONTINUE_STORY,    {"CONTINUE STORY"},KEYCODE(C), "C" },
    { ID_END_STORY,         {"END STORY"},     KEYCODE(E), "E" },
    //#ifdef _USE_DEBUG_MENU_
    //    { ID_DEBUG_EMAIL,         {"TO SDCARD"},        KEYCODE(Z), "Z"},
    //#else
    { ID_CREDITS,           {"CREDITS"},        KEYCODE(Z), "Z"},
    //#endif
 
    { ID_OPTIONS,           {"OPTIONS"},        KEYCODE(O), "O"},

#if defined(_OS_DESKTOP_)
    { ID_EXIT,              {"EXIT" },          KEYCODE(ESCAPE), "ESC"},
#endif
    
#if defined(_USE_VERSION_CHECK_)
    { ID_UPDATE,            {"UPDATE" },        KEYCODE(U), "U"},
#endif
    
};

bool panel_mainmenu::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    //
    // Background
    //
    setBackgroundToHeight("screens/misc/main_menu.png");

    //
    // Logo
    //
    auto logo = ImageView::create(IMAGE_LOGO,Widget::TextureResType::LOCAL);
#if defined(_LOM_)
    uihelper::AddTopLeft(safeArea,logo);
#endif
#if defined(_DDR_)
    uihelper::AddTopCenter(safeArea,logo,0,RES(32));
#endif
    
    //
    // Menu
    //
    menu = uitextmenu::create(RES(512), items, NUMELE(items) );
    uihelper::AddCenter(safeArea,menu);
    
    menu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
        this->OnMenuNotification( s, (menueventargs*)e );
    });

    //
    // Guide and Manual
    //    
    auto guide = uihelper::CreateImageButton("i_guide", ID_GUIDE, clickCallback);
    uihelper::AddBottomRight(safeArea,guide, RES(10), RES(10) );

    auto story = uihelper::CreateImageButton("i_story", ID_MANUAL, clickCallback);
    uihelper::AddBottomLeft(safeArea,story, RES(10), RES(10) );
    
    //
    // Other
    //
    
    TME_Init();

    refreshStories();
    
    return true;
}

void panel_mainmenu::OnNotification( Ref* sender )
{
    switch ( ((Button*)sender)->getTag() ) {
        case ID_MANUAL:
            OnShowManual();
            break;
        case ID_GUIDE:
            OnShowGuide();
            break;
    }
}

void panel_mainmenu::OnMenuNotification( uinotificationinterface* sender, menueventargs* args )
{
    
    switch (args->menuitem->id)
    {
#if defined(_OS_DESKTOP_)
        case ID_EXIT:
            OnExit();
            break;
#endif
#ifdef _USE_DEBUG_MENU_
        case ID_DEBUG_EMAIL:
            break;
#endif
#if defined(_USE_VERSION_CHECK_)
        case ID_UPDATE:
            OnUpdate();
            break;
#endif
        case ID_CREDITS:
            OnCredits();
            break;
        case ID_OPTIONS:
            OnOptions();
            break;
        case ID_HELP_CLOSE:
            OnHelpClose();
            break;
        case ID_NEW_STORY:
            OnNewStory();
            break;
        case ID_CONTINUE_STORY:
            OnContinueStory();
            break;
        case ID_END_STORY:
            OnEndStory();
            break;
        default:
            break;
    }
    
//    if ( id > ID_STORIES ) {
//        
//    }
    
}

void panel_mainmenu::OnShowManual()
{
    AreYouSure(_NOVELLA_PROMPT_, [&] {
        if ( mr->config->novella_pdf ) {
            OpenPDF(_NOVELLA_DOCUMENT_PDF_);
        }else{
            OpenPDF(_NOVELLA_DOCUMENT_);
        }
    });
}

void panel_mainmenu::OnShowGuide()
{
    AreYouSure(_GUIDE_PROMPT_, [&] {
        OpenPDF(_GUIDE_DOCUMENT_);
    });
}

void panel_mainmenu::OnHelpClose()
{
}

void panel_mainmenu::OnOptions()
{
    mr->panels->setPanelMode(MODE_OPTIONS, TRANSITION_FADEIN, true);
}

void panel_mainmenu::OnCredits()
{
    mr->panels->setPanelMode(MODE_CREDITS, TRANSITION_FADEIN, true);
}

void panel_mainmenu::OnExit()
{
    AreYouSure(EXIT_MSG, [&] {
        Director::getInstance()->popScene();
    });
    
}

void panel_mainmenu::OnUpdate()
{
}

void panel_mainmenu::OnNewStory()
{
    storyid_t id = mr->startNewStory();
    
    // if the help window is shown, then we must wait
    // for the user to close it
    // before continuing. Otherwise continue straight away.
    showHelpWindow(HELP_GAME_WORKS, false, [&,id]() {
        mr->continueStory( id );
    });

}

void panel_mainmenu::OnContinueStory()
{
    // only one story, so lets load that
    if ( mr->stories->stories_used()== 1 ) {
        mr->continueStory( mr->stories->first_used_story() );
        return;
    }
    
    //
    // Otherwise we need a chouce
    //
    
    auto bookmenu = uibookmenu::createWithStory( mr->stories->getStoriesInfo() );
    if ( bookmenu == nullptr )
        return;
    
    bookmenu->setLocalZOrder(ZORDER_POPUP);
    uihelper::AddCenter(this,bookmenu);
 
    bookmenu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
        uibookmenu* menu = static_cast<uibookmenu*>(s);
        menu->removeFromParent();
        
        bookeventargs* event = static_cast<bookeventargs*>(e);
        if ( event != nullptr ) { // null == cancelled
            mr->continueStory( event->id );
        }
    });
    
}

void panel_mainmenu::OnEndStory()
{
    //
    // Otherwise we need a choice
    //
    
    auto bookmenu = uibookmenu::createWithStory( mr->stories->getStoriesInfo() );
    if ( bookmenu == nullptr )
        return;
    
    bookmenu->setLocalZOrder(ZORDER_POPUP);
    uihelper::AddCenter(this,bookmenu);
    
    bookmenu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
        
        uibookmenu* menu = static_cast<uibookmenu*>(s);
        menu->removeFromParent();
        
        bookeventargs* event = static_cast<bookeventargs*>(e);
        if ( event != nullptr ) { // null == cancelled
            deleteStory( event->id );
        }
    });
}

void panel_mainmenu::deleteStory( storyid_t id )
{
    AreYouSure(CLOSE_STORY_MSG, [&,id] {
        mr->stories->destroy(id);
        refreshStories();
    });
}

void panel_mainmenu::refreshStories( void )
{
    int count = mr->stories->stories_used();
    
    storyid_t id = mr->stories->next_free_story();
    // add bookmark to menu
    
    menu->EnableItem(ID_CONTINUE_STORY, count > 0);
    menu->EnableItem(ID_END_STORY, count > 0);

}
