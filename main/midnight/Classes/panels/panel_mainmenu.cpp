
#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "panel_mainmenu.h"
#include "panel_credits.h"
#include "panel_options.h"

#include "../system/moonring.h"

#include "../system/keyboardmanager.h"
#include "../system/storymanager.h"
#include "../system/helpmanager.h"
#include "../frontend/resolutionmanager.h"
#include "../frontend/layout_id.h"
#include "../frontend/language.h"

#include "../ui/uitextmenu.h"
#include "../ui/uihelper.h"
#include "../ui/uioptionitem.h"

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
    SetBackground("screens/misc/main_menu.png");

    //
    // Logo
    //
    auto logo = Sprite::createWithSpriteFrameName("lom_logo");
    uihelper::AddTopLeft(this,logo);
    
    //
    // Menu
    //
    auto menu = new uitextmenu(RES(512), items, NUMELE(items) );
    uihelper::AddCenter(this,menu);
    
    menu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
        this->OnMenuNotification( s, (menueventargs*)e );
    });

    //
    // Guide and Manual
    //
    cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback callback = [&] (Ref* ref ) {
        this->OnNotification(ref);
    };
    
    auto guide = uihelper::CreateImageButton("i_guide", ID_GUIDE, callback);
    uihelper::AddBottomRight(this,guide, RES(10), RES(10) );
    
    auto story = uihelper::CreateImageButton("i_story", ID_MANUAL, callback);
    uihelper::AddBottomLeft(this,story, RES(10), RES(10) );
    
    //
    // Other
    //
    
    //auto button = new uioptionitem( RES(512), &items[0]);
    //addChild(button);
    //uihelper::PositionParentTopCenter(button, RES(0), RES(100) );
    
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
    //if ( gl->settings.novella_pdf ) {
    //    OpenPDF(_NOVELLA_DOCUMENT_PDF_);
    
    AreYouSure(_NOVELLA_PROMPT_, [&] {
        OpenPDF(_NOVELLA_DOCUMENT_);
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
    f32 duration = mr->config->screentransitions ? 1.0f : 0.0f;
        
    auto scene = panel_options::create();
    Director::getInstance()->pushScene(TransitionCrossFade::create(duration, scene) );
}

void panel_mainmenu::OnCredits()
{
    f32 duration = mr->config->screentransitions ? 1.0f : 0.0f;
    auto scene = panel_credits::create();
    Director::getInstance()->pushScene(TransitionCrossFade::create(duration, scene) );

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
    //mr->NewStory();
    
    if (!ShowHelpWindow(HELP_GAME_WORKS, false, []() {}) )
        return;
    
    // start game
    //gl->SetPanelMode(MODE_LOOK);
    return;
}

void panel_mainmenu::OnContinueStory()
{
    if (!ShowHelpWindow(HELP_LOOKING_AROUND, false, []() {}) )
        return;
}

void panel_mainmenu::OnEndStory()
{
    if (!ShowHelpWindow(HELP_DAY1, false, []() {}) )
        return;
}




