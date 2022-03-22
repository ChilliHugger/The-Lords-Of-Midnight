//
//  panel_options
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../extensions/CustomDirector.h"
#include "../cocos.h"

#include "panel_options.h"
#include "panel_mainmenu.h"


#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/keyboardmanager.h"
#include "../system/panelmanager.h"

#include "../ui/uihelper.h"
#include "../ui/uitextmenuitem.h"
#include "../ui/uitextmenu.h"
#include "../ui/uioptionitem.h"

#include "../frontend/language.h"

USING_NS_CC;
USING_NS_CC_UI;

static const char* values_onoff[]               = {OPTIONS_ONOFF_OFF,OPTIONS_ONOFF_ON};
static const char* values_yesno[]               = {OPTIONS_YESNO_NO,OPTIONS_YESNO_YES};
static const char* values_movement[]            = {OPTIONS_MOVEMENT_SWIPE_PUSH,OPTIONS_MOVEMENT_PUSH,OPTIONS_MOVEMENT_SWIPE,OPTIONS_MOVEMENT_PRESS_SWIPE};
//static const char* values_think[]               = {OPTIONS_THINK_SWIPE,OPTIONS_THINK_SWIPE,OPTIONS_THINK_ARROWS};
static const char* values_compass_delay[]       = {OPTIONS_COMPASS_DELAY_OFF,OPTIONS_COMPASS_DELAY_NORMAL,OPTIONS_COMPASS_DELAY_SHORT,OPTIONS_COMPASS_DELAY_LONG};
static const char* values_compass_feedback[]    = {OPTIONS_COMPASS_FEEDBACK_OFF,OPTIONS_COMPASS_FEEDBACK_LOW,OPTIONS_COMPASS_FEEDBACK_MEDIUM,OPTIONS_COMPASS_FEEDBACK_HIGH};
static const char* values_slowfast[]            = {OPTIONS_SLOW,OPTIONS_FAST};
static const char* values_fullbrief[]           = {OPTIONS_BRIEF,OPTIONS_FULL};
static const char* values_keyboard[]            = {OPTIONS_KEYBOARD_CLASSIC,OPTIONS_KEYBOARD_NEW};
static const char* values_novella[]             = {OPTIONS_NOVELLA_EBOOK,OPTIONS_NOVELLA_PDF};
static const char* values_screen[]              = {OPTIONS_SCREEN_FULLSCREEN,OPTIONS_SCREEN_SMALL,OPTIONS_SCREEN_MEDIUM,OPTIONS_SCREEN_LARGE};
static const char* values_screen2[]             = {OPTIONS_SCREEN_NOTSUPPORTED,OPTIONS_SCREEN_SMALL,OPTIONS_SCREEN_MEDIUM,OPTIONS_SCREEN_LARGE};

#if defined(_MOUSE_ENABLED_)
static const char* values_cursor[]              = {OPTIONS_CURSOR_SMALL,OPTIONS_CURSOR_MEDIUM,OPTIONS_CURSOR_LARGE};
#endif

/*
 DISPLAY
    MODE                FULL, Window1, Window2, Window3
    TRANSITIONS         OFF, ON
 GAME
    AUTO FIGHT          ON, OFF
    AUTO HIDE           ON, OFF
    BATTLE REPORT       SLOW, FAST
    NIGHT DISPLAY       BRIEF, FULL
    NIGHT ARE YOU SURE  ON, OFF
 CONTROL
    COMPASS DELAY
    COMPASS VIBRATE
    THINK PAGING
    NAVIGATION STYLE    SWIPE

 
 HELP
    TUTORIAL
    MOVEMENT INDICATORS
    NOVELLA
 
 MAIN MENU
 
 */
static uitextmenuitem items_main[] = {
    { ID_MENU_DISPLAY,                  {OPTIONS_SCREEN_DISPLAY},               KEYCODE(D), KEYBOARD_KEY_D },
    { ID_MENU_GAME,                     {OPTIONS_SCREEN_GAME},                  KEYCODE(G), KEYBOARD_KEY_G },
    { ID_MENU_CONTROL,                  {OPTIONS_SCREEN_CONTROL},               KEYCODE(C), KEYBOARD_KEY_C },
    { ID_MENU_HELP,                     {OPTIONS_SCREEN_HELP},                  KEYCODE(H), KEYBOARD_KEY_H },
    { ID_HOME,                          {OPTIONS_SCREEN_MAINMENU},              KEYCODE(ESCAPE), KEYBOARD_KEY_ESC }
};

static uitextmenuitem items_display[] = {
#if defined(_OS_DESKTOP_)
    { ID_OPTION_SCREENMODE,             {OPTIONS_SCREEN_SCREENMODE},            KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE, },
#endif
    { ID_OPTION_TRANSITIONS,            {OPTIONS_SCREEN_TRANSITIONS},           KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
    { ID_OPTION_FLIPSCREEN,             {OPTIONS_SCREEN_FLIPSCREEN},            KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
#if defined(_MOUSE_ENABLED_)
    { ID_OPTION_CURSOR_SIZE,            {OPTIONS_SCREEN_CURSORSIZE},            KEYCODE(4), KEYBOARD_KEY_4, TB_DOUBLE },
#endif
};
    
static uitextmenuitem items_game[] = {
    { ID_OPTION_AUTO_FIGHT,             {OPTIONS_SCREEN_AUTOFIGHT},             KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE },
    { ID_OPTION_AUTO_UNHIDE,            {OPTIONS_SCREEN_AUTOUNHIDE},            KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
    { ID_OPTION_NIGHT_DISPLAY,          {OPTIONS_SCREEN_NIGHTDISPLAY},          KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
    { ID_OPTION_BATTLE_FULL,            {OPTIONS_SCREEN_BATTLEREPORT},          KEYCODE(4), KEYBOARD_KEY_4, TB_DOUBLE },
    { ID_OPTION_NIGHT_CONFIRM,          {OPTIONS_SCREEN_NIGHTCONFIRM},          KEYCODE(5), KEYBOARD_KEY_5, TB_DOUBLE },
};

static uitextmenuitem items_control[] = {
    { ID_OPTION_COMPASS_DELAY,          {OPTIONS_SCREEN_COMPASSDELAY},          KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE },
    { ID_OPTION_COMPASS_FEEDBACK,       {OPTIONS_SCREEN_COMPASSVIBRATE},        KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
    { ID_OPTION_NAVIGATION,             {OPTIONS_SCREEN_NAVIGATIONSTYLE},       KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
    { ID_OPTION_KEYBOARD_STYLE,         {OPTIONS_SCREEN_KEYBOARDSTYLE},         KEYCODE(4), KEYBOARD_KEY_4, TB_DOUBLE },
};

static uitextmenuitem items_help[] = {
    { ID_OPTION_TUTORIAL,               {OPTIONS_SCREEN_TUTORIAL},              KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE },
#if !defined(_OS_DESKTOP_)
    { ID_OPTION_MOVE_INDICATORS,        {OPTIONS_SCREEN_MOVEMENTINDICATORS},    KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
#endif
#if defined(_OS_IOS_) || defined(_OS_OSX_)
    { ID_OPTION_NOVELLA,                {OPTIONS_SCREEN_NOVELLA},               KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
#endif
};

static option_t options[] = {
    
    {   ID_OPTION_AUTO_FIGHT,       OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_AUTO_UNHIDE,      OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_MOVE_INDICATORS,  OPT_BOOL,    0, values_yesno,               nullptr, false },
    {   ID_OPTION_TUTORIAL,         OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_NAVIGATION,       OPT_NUMBER,  4, values_movement,            nullptr, false },
    {   ID_OPTION_TRANSITIONS,      OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_FLIPSCREEN,       OPT_BOOL,    0, values_onoff,               nullptr, false },
    
    {   ID_OPTION_NOVELLA,          OPT_BOOL,    0, values_novella,             nullptr, false },
    
    {   ID_OPTION_COMPASS_DELAY,    OPT_NUMBER,  4, values_compass_delay,       nullptr, false },
    {   ID_OPTION_COMPASS_FEEDBACK, OPT_NUMBER,  4, values_compass_feedback,    nullptr, false },
    {   ID_OPTION_NIGHT_DISPLAY,    OPT_BOOL,    0, values_slowfast,            nullptr, false },
    {   ID_OPTION_BATTLE_FULL,      OPT_BOOL,    0, values_fullbrief,           nullptr, false },
    {   ID_OPTION_NIGHT_CONFIRM,    OPT_BOOL,    0, values_onoff,               nullptr, false },
   

    {   ID_OPTION_SCREENMODE,       OPT_NUMBER,  4, values_screen,              nullptr, false },

    {   ID_OPTION_KEYBOARD_STYLE,   OPT_NUMBER,  2, values_keyboard,            nullptr, false },
#if defined(_MOUSE_ENABLED_)
    {   ID_OPTION_CURSOR_SIZE,      OPT_NUMBER,  3, values_cursor,              nullptr, false },
#endif
    {   ID_HOME,                    OPT_NONE,    0, nullptr,                    nullptr, false },
};


option_t* findOption(int id)
{
    for (auto & option : options){
        if ( option.id == id ){
            return &option;
        }
    }
    return nullptr;
}

void disableOption(int id)
{
    auto option = findOption(id);
    if(option!=nullptr)
    {
        option->disabled = true;
    }
}


bool panel_options::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }

    menu2_background = nullptr;
    fields.clear();
    
    setBackgroundToHeight("screens/misc/main_menu.png");

    auto logo = ImageView::create(IMAGE_LOGO,Widget::TextureResType::LOCAL);
    logo->setLocalZOrder(ZORDER_FAR+1);
    uihelper::AddTopCenter(safeArea,logo,0,RES(32));
         
    CreateMainMenu();
    
    SET_OPTION(ID_OPTION_AUTO_FIGHT,autofight)
    SET_OPTION(ID_OPTION_AUTO_UNHIDE,autounhide)
    SET_OPTION(ID_OPTION_MOVE_INDICATORS,showmovementindicators)
    SET_OPTION(ID_OPTION_TUTORIAL,tutorial)
    SET_OPTION(ID_OPTION_NAVIGATION,nav_mode)
    SET_OPTION(ID_OPTION_TRANSITIONS,screentransitions)
    SET_OPTION(ID_OPTION_FLIPSCREEN,flipscreen)
    
    SET_OPTION(ID_OPTION_NOVELLA,novella_pdf)
    
    SET_OPTION(ID_OPTION_COMPASS_DELAY,compass_delay)
    SET_OPTION(ID_OPTION_COMPASS_FEEDBACK,compass_feedback)
    SET_OPTION(ID_OPTION_NIGHT_DISPLAY,night_display_fast)
    SET_OPTION(ID_OPTION_BATTLE_FULL,night_battle_full)
    SET_OPTION(ID_OPTION_NIGHT_CONFIRM,night_confirm)
      
    SET_OPTION(ID_OPTION_SCREENMODE, screen_mode)
    SET_OPTION(ID_OPTION_KEYBOARD_STYLE, keyboard_mode)
    
#if defined(_MOUSE_ENABLED_)
    SET_OPTION(ID_OPTION_CURSOR_SIZE, cursor_size)
#endif
    
    if ( !mr->settings->fullscreensupported )
    {
        findOption(ID_OPTION_SCREENMODE)->text = values_screen2;
    }
    
    SetMenu(ID_MENU_DISPLAY);
    
    initialScreenMode = mr->settings->screen_mode ;
  
    return true;
}

void panel_options::OnMenuNotification(
    _UNUSED_ const uinotificationinterface* sender,
    menueventargs* args )
{
    int tag = args->menuitem->id;
    
    switch (tag){
        case ID_HOME:
        {
#if defined(_OS_DESKTOP_)
            if(initialScreenMode != mr->settings->screen_mode)
            {
                changeDisplayMode();
                return;
            }
#endif
            Exit();
            mr->settings->Save();
            return;
        }

        case ID_MENU_GAME:
        case ID_MENU_DISPLAY:
        case ID_MENU_CONTROL:
        case ID_MENU_HELP:
            SetMenu(tag);
            return;

        case ID_CLOSE:
        case ID_HELP_CLOSE:
            return;

        default:
            break;
    }
    
    auto option = findOption(tag);
    if(option==nullptr)
        return;

    if ( option->type == OPT_BOOL ) {
        BOOL* value = (BOOL*) option->var;
        *value = ! *value;
    }
    
    else if ( option->type == OPT_NUMBER ) {
        int* value = (int*) option->var;
        *value = *value +1;
        if ( *value >= option->max )
            *value=0;
    }
    
    SetValues();
    
    if ( option->id == ID_OPTION_TUTORIAL ) {
        if ( mr->settings->tutorial )
            showHelpWindow(HELP_TUTORIAL_ON);
        else
            showHelpWindow(HELP_TUTORIAL_OFF);
    }
    
#if defined(_MOUSE_ENABLED_)
    if ( option->id == ID_OPTION_CURSOR_SIZE ) {
        setCursor(MOUSE_NORMAL);
    }
#endif
    
}

void panel_options::SetMenu ( int id )
{
    if ( id == ID_MENU_GAME ) {
        SetMenu(items_game,NUMELE(items_game));
        SetValues();
        return;
    }else if ( id == ID_MENU_DISPLAY ) {
        SetMenu(items_display,NUMELE(items_display));
        SetValues();
        return;
    } else if ( id == ID_MENU_CONTROL ) {

        // check if keyboard available
        //if ( !ui->IsKeyboardAvailable() )
        {
            disableOption(ID_OPTION_KEYBOARD_STYLE);
        }

        // check if compass feedback available
//        if ( !ui->IsVibrationAvailable()) {
//            menu2->DisableButton(ID_OPTION_COMPASS_FEEDBACK);
//            gl->settings.compass_feedback=CF_COMPASS_OFF;
//        }
//

        SetMenu(items_control,NUMELE(items_control));
        
#if defined(_OS_DESKTOP_)
        mr->settings->showmovementindicators=FALSE;
#endif
        
        SetValues();
        return;
    } else if ( id == ID_MENU_HELP ) {
        SetMenu(items_help,NUMELE(items_help));
        
#if !defined(_OS_IOS_) && !defined(_OS_OSX_)
        mr->settings->novella_pdf=TRUE;
#endif
        
        SetValues();
        return;
    }
}

void panel_options::SetValues()
{
    for (auto & option : options) {

        int item=0;
        if ( option.type == OPT_BOOL ) {
            bool* value = (bool*) option.var ;
            item = *value ? 1 : 0 ;
        }
        if ( option.type == OPT_NUMBER ) {
            int* value = (int*) option.var ;
            item = *value ;
        }
        
        if ( option.type != OPT_NONE ) {

            auto field = optionControls.at(option.id);
            if ( field != nullptr  )
                field->setValue(option.text[item]);
        }
        
    }
}

void panel_options::CreateMainMenu()
{
    f32 width = RES(512-32);
    
    f32 contentWidth = safeArea->getContentSize().width;
    f32 contentHeight = safeArea->getContentSize().height;
    
    mainMenu = uitextmenu::create(width, items_main, NUMELE(items_main) );
    mainMenu->setAnchorPoint(uihelper::AnchorCenter);
    mainMenu->setPosition(Vec2(contentWidth/4,contentHeight/2));
    mainMenu->setLocalZOrder(ZORDER_UI);
    safeArea->addChild(mainMenu);

    mainMenu->setNotificationCallback ( [&](const uinotificationinterface* s, uieventargs* e) {
        this->OnMenuNotification( s, (menueventargs*)e );
    });
    
}

void panel_options::SetMenu( uitextmenuitem items[], int elements )
{
    // map keyboard shortcut keys to layout children
    uishortcutkeys::registerCallback(safeArea, clickCallback);
 
    f32 width = RES(512-32);
    if(isPhoneScaleEnabled())
    {
        width+=RES(128+64);
    }
    
    f32 contentWidth = safeArea->getContentSize().width;
    
    if ( subMenu != nullptr )  {
        subMenu->removeAllChildren();
        subMenu->removeFromParent();
    }

    subMenu = Menu::create();
    subMenu->setLocalZOrder(ZORDER_UI);
    
    u32 gapY = PHONE_SCALE(RES(5));
    
    safeArea->addChild(subMenu);
    
    fields.clear();
    optionControls.clear();
    
    f32 height=0;
    for ( int ii=0; ii<elements; ii++ ) {
        
        uitextmenuitem* item = &items[ii];
        
        auto option = findOption(item->id);
        if(option->disabled)
        {
            continue;
        }
        
        auto button = uioptionitem::create( width, item );
        button->setTag(item->id);
        button->setUserData(item);
        
        addShortcutKey(button, item->id, item->keyboard_shortcut);

        // find the options
        optionControls.insert(item->id, button);
        
        auto menuItem = MenuItemNode::create(button);
        auto r = menuItem->getBoundingBox();
        height += r.size.height + gapY;

        menuItem->setCallback([&,item](cocos2d::Ref *sender) {
            menueventargs args;
            args.menuitem = item;
            OnMenuNotification( nullptr, &args );
        } );
        
        subMenu->addChild(menuItem);
        fields.pushBack(menuItem);
    }
    
    subMenu->setContentSize(Size(width,height) );
  
    uihelper::PositionParentCenterLeft(subMenu,contentWidth/2);
    
    // refresh positions
    auto offset = Vec2( width/2, height );
    for ( auto item : fields ) {
        f32 itemHeight = item->getBoundingBox().size.height;
        offset.y -= itemHeight / 2;
        item->setAnchorPoint( uihelper::AnchorCenter );
        item->setPosition(offset);
        offset.y -= (itemHeight / 2) + gapY;

    }

}

#if defined(_OS_DESKTOP_)
void panel_options::changeDisplayMode()
{
    AreYouSure(CHANGE_DISPLAY_MSG,
        [&] {
            // yes
            mr->changeDisplayMode();
        },
        [&] {
            // no
            Exit();
            mr->settings->Save();
        });
}
#endif

bool panel_options::OnKeyboardEvent( uikeyboardevent* event )
{
    if ( !event->isUp() ) {
        return false;
    }
    
    if ( mainMenu != nullptr ) {
        if( mainMenu->dispatchShortcutKey(event->getKey()) )
        return true;
    }

    if ( uipanel::OnKeyboardEvent(event) ) {
        return true;
    }

    if (mainMenu!=nullptr) {
        mainMenu->displayShortcuts();
    }
    displayShortcuts();
    return false;
}

void panel_options::OnNotification( Ref* sender )
{
    auto menuItem = dynamic_cast<uioptionitem*>(sender);
    if ( menuItem == nullptr )
        return;
    
    menueventargs args;
    args.menuitem = static_cast<uitextmenuitem*>(menuItem->getUserData());
    OnMenuNotification( nullptr, &args );
}

