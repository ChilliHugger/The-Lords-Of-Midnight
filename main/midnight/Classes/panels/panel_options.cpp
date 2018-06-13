//
//  panel_options
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../Extensions/CustomDirector.h"
#include "ui/CocosGUI.h"

#include "panel_options.h"
#include "panel_mainmenu.h"


#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../system/configmanager.h"
#include "../system/keyboardmanager.h"
#include "../system/panelmanager.h"

#include "../ui/uihelper.h"
#include "../ui/uitextmenuitem.h"
#include "../ui/uitextmenu.h"
#include "../ui/uioptionitem.h"

USING_NS_CC;
using namespace cocos2d::ui;

static const char* values_onoff[]               = {"OFF","ON"};
static const char* values_yesno[]               = {"NO","YES"};
static const char* values_movement[]            = {"SWIPE & PUSH","PUSH","SWIPE","PRESS LOOK/SWIPE MOVE"};
static const char* values_think[]               = {"SWIPE LEFT/RIGHT","PUSH","ARROWS ONLY"};
static const char* values_compass_delay[]       = {"OFF","NORMAL PRESS","SHORT PRESS","LONG PRESS"};
static const char* values_compass_feedback[]    = {"OFF","LOW","MEDIUM","HIGH"};
static const char* values_slowfast[]            = {"SLOW","FAST"};
static const char* values_fullbrief[]           = {"BRIEF","FULL"};
static const char* values_keyboard[]            = {"CLASSIC","NEW"};
static const char* values_novella[]             = {"EBOOK","PDF"};
static const char* values_screen[]              = {"FULLSCREEN","SMALL","MEDIUM","LARGE"};
static const char* values_screen2[]             = {"FULLSCREEN - NOT SUPPORTED","SMALL","MEDIUM","LARGE"};



/*
 DISPLAY
    MODE                FULL, Window1, Window2, Window3
    TRANSITIONS         OFF, ON
 GAME
    AUTO FIGHT          ON, OFF
    AUTO HIDE           ON, OFF
    BATTLE REPORT       SLOW, FAST
    NIGHT DISPLAY       BRIEF, FULL
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
    { ID_MENU_DISPLAY,                  {"DISPLAY"},            KEYCODE(D), "D" },
    { ID_MENU_GAME,                     {"GAME"},               KEYCODE(G), "G" },
    { ID_MENU_CONTROL,                  {"CONTROL"},            KEYCODE(C), "C" },
    { ID_MENU_HELP,                     {"HELP"},               KEYCODE(H), "H" },
    { ID_HOME,                          {"MAIN MENU"},          KEYCODE(ESCAPE), "ESC" }
};

static uitextmenuitem items_display[] = {
#if defined(_OS_DESKTOP_)
    { ID_OPTION_SCREENMODE,             {"SCREEN MODE"},        KEYCODE(1), "1", TB_DOUBLE, },
#endif
    { ID_OPTION_TRANSITIONS,            {"SCREEN TRANSITIONS"}, KEYCODE(2), "2", TB_DOUBLE },
};

static uitextmenuitem items_game[] = {
    { ID_OPTION_AUTO_FIGHT,             {"AUTO FIGHT"},         KEYCODE(1), "1", TB_DOUBLE },
    { ID_OPTION_AUTO_UNHIDE,            {"AUTO UNHIDE"},        KEYCODE(2), "2", TB_DOUBLE },
    { ID_OPTION_NIGHT_DISPLAY,          {"NIGHT DISPLAY"},      KEYCODE(3), "3", TB_DOUBLE },
    { ID_OPTION_BATTLE_FULL,            {"BATTLE REPORT "},     KEYCODE(4), "4", TB_DOUBLE },
};

static uitextmenuitem items_control[] = {
    { ID_OPTION_COMPASS_DELAY,          {"COMPASS DELAY"},      KEYCODE(1), "1", TB_DOUBLE },
    { ID_OPTION_COMPASS_FEEDBACK,       {"COMPASS VIBRATE"},    KEYCODE(2), "2", TB_DOUBLE },
    { ID_OPTION_THINK_PAGING,           {"THINK PAGING"},       KEYCODE(3), "3", TB_DOUBLE },
    { ID_OPTION_NAVIGATION,             {"NAVIGATION STYLE"},   KEYCODE(4), "4", TB_DOUBLE },
    { ID_OPTION_KEYBOARD_STYLE,         {"KEYBOARD STYLE"},     KEYCODE(5), "5", TB_DOUBLE },
};

static uitextmenuitem items_help[] = {
    { ID_OPTION_TUTORIAL,               {"TUTORIAL"},           KEYCODE(1), "1", TB_DOUBLE },
#if !defined(_OS_DESKTOP_)
    { ID_OPTION_MOVE_INDICATORS,        {"MOVEMENT INDICATORS"},KEYCODE(2), "2", TB_DOUBLE },
#endif
#if defined(_OS_IOS_) || defined(_OS_OSX_)
    { ID_OPTION_NOVELLA,                {"NOVELLA"},            KEYCODE(3), "3", TB_DOUBLE },
#endif
};

static option_t options[] = {
    
    {   ID_OPTION_AUTO_FIGHT,       OPT_BOOL,    0, values_onoff, NULL },
    {   ID_OPTION_AUTO_UNHIDE,      OPT_BOOL,    0, values_onoff, NULL },
    {   ID_OPTION_MOVE_INDICATORS,  OPT_BOOL,    0, values_yesno, NULL },
    {   ID_OPTION_TUTORIAL,         OPT_BOOL,    0, values_onoff, NULL },
    {   ID_OPTION_NAVIGATION,       OPT_NUMBER,  4, values_movement, NULL },
    {   ID_OPTION_TRANSITIONS,      OPT_BOOL,    0, values_onoff, NULL },
    {   ID_OPTION_NOVELLA,          OPT_BOOL,    0, values_novella, NULL },
    
    {   ID_OPTION_COMPASS_DELAY,    OPT_NUMBER,  4, values_compass_delay, NULL },
    {   ID_OPTION_COMPASS_FEEDBACK, OPT_NUMBER,  4, values_compass_feedback, NULL },
    {   ID_OPTION_THINK_PAGING,     OPT_NUMBER,  3, values_think, NULL },
    {   ID_OPTION_NIGHT_DISPLAY,    OPT_BOOL,    0, values_slowfast, NULL },
    {   ID_OPTION_BATTLE_FULL,      OPT_BOOL,    0, values_fullbrief, NULL },
    
    {   ID_OPTION_SCREENMODE,       OPT_NUMBER,  4, values_screen, NULL }, // 11
    {   ID_OPTION_KEYBOARD_STYLE,   OPT_NUMBER,  2, values_keyboard, NULL },
    
    {   ID_HOME,                    OPT_NONE,    0, NULL, NULL },
};

bool panel_options::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }

    menu2_background = NULL;
    fields.clear();
    
    FillBackground();

    auto logo = Sprite::createWithSpriteFrameName("lom_logo");
    uihelper::AddTopLeft(this,logo);
    
    
    auto p2 = Sprite::create("screens/misc/corleth.png");
    uihelper::AddBottomLeft(this,p2);
    
    CreateMenu1();
    
    SET_OPTION(0,autofight);
    SET_OPTION(1,autounhide);
    SET_OPTION(2,showmovementindicators);
    SET_OPTION(3,tutorial);
    SET_OPTION(4,nav_mode);
    SET_OPTION(5,screentransitions);
    SET_OPTION(6,novella_pdf);
    
    SET_OPTION(7,compass_delay);
    SET_OPTION(8,compass_feedback);
    SET_OPTION(9,think_paging_mode);
    SET_OPTION(10,night_display_fast);
    SET_OPTION(11,night_battle_full);
    
    SET_OPTION(12, screen_mode);
    SET_OPTION(13, keyboard_mode);
    
    if ( !mr->config->fullscreensupported )
        options[12].text = values_screen2 ;
    
    
    SetMenu(ID_MENU_DISPLAY);
    
    return true;
}

void panel_options::OnMenuNotification( uinotificationinterface* sender, menueventargs* args )
{
    int tag = args->menuitem->id;
    
    if ( tag == ID_HOME ) {
        Exit();
        mr->config->Save();
        //gl->SetPanelMode(MODE_MAINMENU,TRANSITION_FADEIN);
        return;
    } else if ( tag == ID_MENU_GAME ) {
        SetMenu(tag);
        return;
    }else if ( tag == ID_MENU_DISPLAY ) {
        SetMenu(tag);
        return;
    } else if ( tag == ID_MENU_CONTROL ) {
        SetMenu(tag);
        return;
    } else if ( tag == ID_MENU_HELP ) {
        SetMenu(tag);
        return;
    } else if ( tag == ID_CLOSE || tag == ID_HELP_CLOSE ) {
        return;
    }
    
    int index=-1;
    for ( int ii=0; ii<NUMELE(options); ii++ ) {
        if ( options[ii].id == tag ) {
            index = ii;
            break;
        }
    }

    if ( index == -1 )
        return;

    
    if ( options[index].type == OPT_BOOL ) {
        BOOL* value = (BOOL*) options[index].var;
        *value = ! *value;
    }
    
    if ( options[index].type == OPT_NUMBER ) {
        int* value = (int*) options[index].var;
        *value = *value +1;
        if ( *value >= options[index].max )
            *value=0;
    }
    
    SetValues();
    
    if ( options[index].id == ID_OPTION_TUTORIAL ) {
        if ( mr->config->tutorial )
            ShowHelpWindow(HELP_TUTORIAL_ON);
        else
            ShowHelpWindow(HELP_TUTORIAL_OFF);
    }
    
    
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
        SetMenu(items_control,NUMELE(items_control));
        
        // check if compass feedback available
//        if ( !ui->IsVibrationAvailable()) {
//            menu2->DisableButton(ID_OPTION_COMPASS_FEEDBACK);
//            gl->settings.compass_feedback=CF_COMPASS_OFF;
//        }
//
//        // check if keyboard available
//        if ( !ui->IsKeyboardAvailable() ) {
//            menu2->DisableButton(ID_OPTION_KEYBOARD_STYLE);
//        }
        
        
#if defined(_OS_DESKTOP_)
        mr->config->showmovementindicators=FALSE;
#endif
        
        SetValues();
        return;
    } else if ( id == ID_MENU_HELP ) {
        SetMenu(items_help,NUMELE(items_help));
        
#if !defined(_OS_IOS_) && !defined(_OS_OSX_)
        mr->config->novella_pdf=TRUE;
#endif
        
        SetValues();
        return;
    }
}



void panel_options::SetValues ( void )
{
    for ( u32 ii=0; ii<NUMELE(options); ii++ ) {

        int item=0;
        if ( options[ii].type == OPT_BOOL ) {
            BOOL* value = (BOOL*) options[ii].var ;
            item = *value ? 1 : 0 ;
        }
        if ( options[ii].type == OPT_NUMBER ) {
            int* value = (int*) options[ii].var ;
            item = *value ;
        }
        
        if ( options[ii].type != OPT_NONE ) {

            auto field = optionControls.at(options[ii].id);
            if ( field != nullptr  )
                field->setValue(options[ii].text[item]);
        }
        
    }
}





void panel_options::CreateMenu1()
{
    
    f32 width = RES(512-40);

    auto menu = new uitextmenu(width, items_main, NUMELE(items_main) );
    uihelper::AddCenterLeft(this,menu,RES(20), 0);

    menu->setNotificationCallback ( [&](uinotificationinterface* s, uieventargs* e) {
        this->OnMenuNotification( s, (menueventargs*)e );
    });
    
}



void panel_options::SetMenu( uitextmenuitem items[], int elements )
{
    f32 width = RES(512-40);
    
    if ( menu2 != nullptr )  {
        menu2->removeAllChildren();
        menu2->removeFromParent();
    }

    menu2 = Menu::create();
    
    if ( menu2_background == nullptr ) {
        menu2_background = DrawNode::create();
        addChild(menu2_background);
    }else{
        menu2_background->removeAllChildren();
        menu2_background->clear();
    }
    
    u32 gapY = RES(5);
    
    this->addChild(menu2);
    
    fields.clear();
    optionControls.clear();
    
    f32 height=0;
    for ( int ii=0; ii<elements; ii++ ) {
        
        uitextmenuitem* item = &items[ii];
        
        auto button = new uioptionitem( width, item );
        button->setTag(item->id);
        
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
        
        menu2->addChild(menuItem);
        fields.pushBack(menuItem);
    }
    
    menu2_background->setContentSize(Size(width,height) );
    menu2->setContentSize(Size(width,height) );

    //menu2_background->drawSolidRect(Point::ZERO, Point(RES(512),height), Color4F::BLACK);
    uihelper::PositionParentCenterRight(menu2_background,RES(20));
    uihelper::PositionParentCenterRight(menu2,RES(20));
    
    // refresh positions
    auto offset = Vec2( width/2, height );
    for ( auto item : fields ) {
        f32 height = item->getBoundingBox().size.height;
        offset.y -= height/2;
        item->setAnchorPoint( uihelper::AnchorCenter );
        item->setPosition(offset);
        offset.y -=  (height/2) + gapY;
    }
    
}



