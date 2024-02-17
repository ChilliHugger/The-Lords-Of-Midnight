#ifndef WIN32
#include <__bit_reference>
#endif

#include "../cocos.h"

#include "panel_mainmenu.h"
#include "panel_credits.h"
#include "panel_options.h"

#include "../system/moonring.h"

#include "../system/keyboardmanager.h"
#include "../system/storymanager.h"
#include "../system/helpmanager.h"
#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../frontend/language.h"

#include "../ui/uitextmenu.h"
#include "../ui/uihelper.h"
#include "../ui/uioptionitem.h"

#include "../ui/uibook.h"
#include "../ui/uibookmenu.h"

USING_NS_CC;
using namespace cocos2d::ui;

//#define BOOKMARK_IMAGE          "bookmark"
//#define BOOKMARK_OVERLAY_IMAGE  "bookmark_overlay"
//
//static rgb_t bookmark_colours[] = {
//    rgb_t(0xde, 0x00, 0x00),
//    rgb_t(0xde, 0x7e, 0x00),
//    rgb_t(0xcc, 0xcc, 0x00),
//    rgb_t(0x69, 0xa9, 0x00),
//    rgb_t(0x00, 0xb2, 0xa4),
//    rgb_t(0x00, 0x7e, 0xcc),
//    rgb_t(0x84, 0x00, 0xdb),
//    rgb_t(0x96, 0x96, 0x96)
//};


static uitextmenuitem items[] = {
    { ID_NEW_STORY,         {MAINMENU_NEW_STORY},       KEYCODE(N), KEYBOARD_KEY_N },
    { ID_CONTINUE_STORY,    {MAINMENU_CONTINUE_STORY},  KEYCODE(C), KEYBOARD_KEY_C },
    { ID_END_STORY,         {MAINMENU_END_STORY},       KEYCODE(E), KEYBOARD_KEY_E },
    //#ifdef _USE_DEBUG_MENU_
    //    { ID_DEBUG_EMAIL, {"TO SDCARD"},              KEYCODE(Z), "Z"},
    //#else
    { ID_CREDITS,           {MAINMENU_CREDITS},         KEYCODE(Z), KEYBOARD_KEY_Z},
    //#endif
    { ID_OPTIONS,           {MAINMENU_OPTIONS},         KEYCODE(O), KEYBOARD_KEY_O},
#if defined(_OS_DESKTOP_)
    { ID_EXIT,              {MAINMENU_EXIT},            KEYCODE(ESCAPE), KEYBOARD_KEY_ESC},
#endif
#if defined(_USE_VERSION_CHECK_)
    { ID_UPDATE,            {MAINMENU_UPDATE},          KEYCODE(U), KEYBOARD_KEY_U},
#endif
    
};

panel_mainmenu::~panel_mainmenu() = default;

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
    uihelper::AddTopCenter(safeArea,logo,0,RES(32));
    
    //
    // Menu
    //
    menu = uitextmenu::create(RES(512), items, NUMELE(items) );
    uihelper::AddCenter(safeArea,menu);
    menu->setVisible(false);
    
    menu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
        this->OnMenuNotification( s, (menueventargs*)e );
    });

#if defined(_USE_VERSION_CHECK_)
    menu->showItem(ID_UPDATE, false);
    mr->getVersion([&](bool updateAvailable, std::string& url)
    {
        if(updateAvailable)
        {
            versionUrl = url;
            menu->showItem(ID_UPDATE, true);
        }
    });
#endif

    // Bookmark
    
//    if ( mr->stories->stories_used()== 1 ) {
//        auto slot = mr->stories->first_used_story()-1;
//
//        auto bookmarkImage = Sprite::createWithSpriteFrameName(BOOKMARK_IMAGE);
//        bookmarkImage->setColor(bookmark_colours[slot]);
//        uihelper::AddTopRight(menu, bookmarkImage, RES(22), RES(8));
//
//        auto bookmarkOverlayImage = Sprite::createWithSpriteFrameName(BOOKMARK_OVERLAY_IMAGE);
//        bookmarkOverlayImage->setColor(_clrBlack);
//        uihelper::AddCenter(bookmarkImage, bookmarkOverlayImage);
//
//        bookmarkImage->setRotation(16);
//        bookmarkImage->setLocalZOrder(ZORDER_FAR);
//    }
  

    //
    // Guide and Manual
    //    
    guide = uihelper::CreateImageButton("i_guide", ID_GUIDE, clickCallback);
    guide->setScale(scale_normal+scale_half);
    uihelper::AddBottomRight(safeArea,guide, RES(10), RES(10) );
    

    story = uihelper::CreateImageButton("i_story", ID_MANUAL, clickCallback);
    story->setScale(scale_normal+scale_half);
    uihelper::AddBottomLeft(safeArea,story, RES(10), RES(10) );
    
    
    
    
    //
    // Other
    //
    
    TME_DeInit();
    TME_Init(RF_DEFAULT,DF_NORMAL);

    refreshStories();
    
    //if(mr->settings->firsttime) {
        scheduleOnce( [&](float) {
            OnShowWelcomeMessage();
        }, 1.0, "delayed_welcome_message" );
    //}else{
    //    menu->setVisible(true);
    //}
    
    return true;
}

ActionInterval* createBounceAction()
{
    auto duration = 1.0f;
    auto distance = RES(50);
    return RepeatForever::create(
            Sequence::create(
                EaseSineOut::create(MoveBy::create(duration, Vec2(0,distance))),
                EaseBounceOut::create(MoveBy::create(duration, Vec2(0,-distance))),
                nullptr
            )
        );
}

void panel_mainmenu::OnShowWelcomeMessage()
{
    auto posy = guide->getPositionY();

    guide->stopAllActions();
    guide->runAction(createBounceAction());

    story->stopAllActions();
    story->runAction(
        Sequence::createWithTwoActions(
            DelayTime::create(0.5f),
            CallFunc::create( [this] { story->runAction(createBounceAction()); })
        )
    );

    AreYouSure(_WELCOME_PROMPT_,
    [&] {
        hideWelcomeMessage(posy);
        showNovella();
    },
    [&] {
        hideWelcomeMessage(posy);
    });
}

void panel_mainmenu::hideWelcomeMessage(f32 y)
{
    guide->stopAllActions();
    story->stopAllActions();
    
    guide->runAction(EaseBounceOut::create(MoveTo::create(1.0f, Vec2(guide->getPositionX(), y))));
    story->runAction(EaseBounceOut::create(MoveTo::create(1.0f, Vec2(story->getPositionX(), y))));
        
    auto scale = menu->getScale();
    menu->setScale(0);
    menu->setVisible(true);
    menu->runAction(ScaleTo::create(0.5f,scale));
    
    mr->settings->firsttime = false;
    mr->settings->Save();

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

void panel_mainmenu::OnMenuNotification(
        _UNUSED_ const uinotificationinterface* sender,
        menueventargs* args )
{
    
    switch (args->menuitem->id)
    {
#if defined(_OS_DESKTOP_)
        case ID_EXIT:
            OnExitGame();
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


bool panel_mainmenu::OnKeyboardEvent( uikeyboardevent* event )
{
    if ( !event->isUp() ) {
        return false;
    }

#if defined(__ANDROID__)
    if(event->getKey() == KEYCODE(ESCAPE) ) {
        OnExitGame();
        return true;
    }
#endif

    if ( menu != nullptr ) {
        if( menu->dispatchShortcutKey(event->getKey()) )
            return true;
    }

    if ( uipanel::OnKeyboardEvent(event) ) {
        return true;
    }

    if (menu!=nullptr) {
        menu->displayShortcuts();
    }
    displayShortcuts();
    return false;
}

void panel_mainmenu::OnShowManual()
{
    AreYouSure(_NOVELLA_PROMPT_, [&] {
        showNovella();
    });
}

void panel_mainmenu::showNovella()
{
    if ( mr->settings->novella_pdf ) {
        OpenPDF(_NOVELLA_DOCUMENT_PDF_);
    }else{
        OpenPDF(_NOVELLA_DOCUMENT_);
    }
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

void panel_mainmenu::OnExitGame()
{
    AreYouSure(EXIT_MSG, [&] {
        Director::getInstance()->end();
    });
    
}

#if defined(_USE_VERSION_CHECK_)
void panel_mainmenu::OnUpdate()
{
    AreYouSure(_UPDATE_PROMPT_, [&] {
        Application::getInstance()->openURL(versionUrl);
    });
}
#endif

void panel_mainmenu::OnNewStory()
{
    storyid_t id = mr->startNewStory();
    if(id == STORY_NONE)
    {
        return;
    }
    
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
    
    auto bookmenu = uibookmenu::createWithStory( this, mr->stories->getStoriesInfo() );
    if ( bookmenu == nullptr )
        return;
    
    bookmenu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {

        auto event = dynamic_cast<bookeventargs*>(e);
        if (event != nullptr) { // null == cancelled
            mr->continueStory(event->id);
        }

        RUN_ON_UI_THREAD([=]() {
            auto item = dynamic_cast<uibookmenu*>(s);
            item->Close();
        });
    });
    
    bookmenu->Show();
}

void panel_mainmenu::OnEndStory()
{
    //
    // Otherwise we need a choice
    //
    
    auto bookmenu = uibookmenu::createWithStory( this, mr->stories->getStoriesInfo() );
    if ( bookmenu == nullptr )
        return;
    
    bookmenu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
        
        auto event = dynamic_cast<bookeventargs*>(e);
        if (event != nullptr) { // null == cancelled
            deleteStory(event->id);
        }

        RUN_ON_UI_THREAD([=]() {
            auto item = dynamic_cast<uibookmenu*>(s);
            item->Close();
        });
    });
    
    bookmenu->Show();
}

void panel_mainmenu::deleteStory( storyid_t id )
{
    AreYouSure(CLOSE_STORY_MSG, [&,id] {
        mr->stories->destroy(id);
        refreshStories();
    });
}


void panel_mainmenu::refreshStories()
{
    int count = mr->stories->stories_used();
    
    storyid_t id = mr->stories->next_free_story();
    
    // add bookmark to menu
    
    menu->showItem(ID_NEW_STORY, id != STORY_NONE );
    menu->showItem(ID_CONTINUE_STORY, count > 0);
    menu->showItem(ID_END_STORY, count > 0);
    //menu->refresh();
}
