//
//  panel_options
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../extensions/CustomDirector.h"
#include "../extensions/ScrollView.h" // scrolling menu
#include "../cocos.h"

#include "panel_options.h"
#include "panel_mainmenu.h"

#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/keyboardmanager.h"
#include "../system/panelmanager.h"

#include "../ui/helper.h"
#include "../ui/uitextmenuitem.h"
#include "../ui/uitextmenu.h"
#include "../ui/uioptionitem.h"

#include "../frontend/language.h"

USING_NS_CC;
USING_NS_CC_UI;



#if defined(_LOM_)
#define _USE_GAME_RULES_
static uitextmenuitem items_rules[] = {
    { ID_OPTION_DIFFICULTY,             {OPTIONS_SCREEN_DIFFICULTY},            KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE },
    { ID_OPTION_RULE_1,                 {OPTIONS_SCREEN_RULE_1},                KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
    { ID_OPTION_RULE_2,                 {OPTIONS_SCREEN_RULE_2},                KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
    { ID_OPTION_RULE_3,                 {OPTIONS_SCREEN_RULE_3},                KEYCODE(4), KEYBOARD_KEY_4, TB_DOUBLE },
    { ID_OPTION_RULE_4,                 {OPTIONS_SCREEN_RULE_4},                KEYCODE(5), KEYBOARD_KEY_5, TB_DOUBLE },
    { ID_OPTION_RULE_5,                 {OPTIONS_SCREEN_RULE_5},                KEYCODE(6), KEYBOARD_KEY_6, TB_DOUBLE }
};
static RULEFLAGS rule_mapping[] = {
    RF_IMPASSABLE_MOUNTAINS,
    RF_AI_IMPASSABLE_MOUNTAINS,
    RF_ADD_MOUNTAIN_PASSES,
    RF_SOLE_MOUNTAINEER,
    RF_LOM_UNRECRUITABLE_FEY,
    RF_NONE,                    // RF_FAST_TUNNELS
    RF_NONE,                    // RF_DDR_MOVEMENT_SPECTRUM,
    RF_NONE,                    // RF_DDR_MOVEMENT_C64,
    RF_NONE,                    // RF_DDR_BETTER_ARMIES
};

#elif defined(_DDR_)
#define _USE_GAME_RULES_
static uitextmenuitem items_rules[] = {
    { ID_OPTION_DIFFICULTY,             {OPTIONS_SCREEN_DIFFICULTY},            KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE },
    { ID_OPTION_RULE_1,                 {OPTIONS_SCREEN_RULE_1},                KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
    { ID_OPTION_RULE_2,                 {OPTIONS_SCREEN_RULE_2},                KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
    { ID_OPTION_RULE_4,                 {OPTIONS_SCREEN_RULE_4},                KEYCODE(4), KEYBOARD_KEY_4, TB_DOUBLE },
    { ID_OPTION_RULE_6,                 {OPTIONS_SCREEN_RULE_6},                KEYCODE(5), KEYBOARD_KEY_5, TB_DOUBLE },
    { ID_OPTION_RULE_7,                 {OPTIONS_SCREEN_RULE_7},                KEYCODE(6), KEYBOARD_KEY_6, TB_DOUBLE },
    { ID_OPTION_RULE_8,                 {OPTIONS_SCREEN_RULE_8},                KEYCODE(7), KEYBOARD_KEY_7, TB_DOUBLE }
};
static RULEFLAGS rule_mapping[] = {
    RF_IMPASSABLE_MOUNTAINS,
    RF_AI_IMPASSABLE_MOUNTAINS,
    RF_NONE,                    // RF_ADD_MOUNTAIN_PASSES
    RF_SOLE_MOUNTAINEER,
    RF_NONE,                    // RF_LOM_UNRECRUITABLE_FEY
    RF_FAST_TUNNELS,
    RF_NONE,                    // RF_DDR_MOVEMENT_SPECTRUM,
    RF_NONE,                    // RF_DDR_MOVEMENT_C64,
    RF_DDR_BETTER_ARMIES
};
#endif

static const char* values_movement_type[] = {
    OPTIONS_SCREEN_RULE_ORIGINAL,
    OPTIONS_SCREEN_RULE_INTENDED,
    OPTIONS_SCREEN_RULE_C64
};

static const char* values_onoff[] = {
    OPTIONS_ONOFF_OFF,
    OPTIONS_ONOFF_ON
};

static const char* values_yesno[] = {
    OPTIONS_YESNO_NO,
    OPTIONS_YESNO_YES
};

static const char* values_movement[] = {
    OPTIONS_MOVEMENT_SWIPE_PUSH,
    OPTIONS_MOVEMENT_PUSH,
    OPTIONS_MOVEMENT_SWIPE,
    OPTIONS_MOVEMENT_PRESS_SWIPE
};

//static const char* values_think[]               = {OPTIONS_THINK_SWIPE,OPTIONS_THINK_SWIPE,OPTIONS_THINK_ARROWS};

static const char* values_compass_delay[] = {
    OPTIONS_COMPASS_DELAY_OFF,
    OPTIONS_COMPASS_DELAY_NORMAL,
    OPTIONS_COMPASS_DELAY_SHORT,
    OPTIONS_COMPASS_DELAY_LONG
};

static const char* values_slowfast[] = {
    OPTIONS_SLOW,
    OPTIONS_FAST
};

static const char* values_fullbrief[] = {
    OPTIONS_BRIEF,
    OPTIONS_FULL
};

static const char* values_keyboard[] = {
    OPTIONS_KEYBOARD_CLASSIC,
    OPTIONS_KEYBOARD_NEW
};

static const char* values_novella[] = {
    OPTIONS_NOVELLA_EBOOK,
    OPTIONS_NOVELLA_PDF
};

static const char* values_screen[] = {
    OPTIONS_SCREEN_FULLSCREEN,
    OPTIONS_SCREEN_SMALL,
    OPTIONS_SCREEN_MEDIUM,
    OPTIONS_SCREEN_LARGE
};

static const char* values_screen2[] = {
    OPTIONS_SCREEN_NOTSUPPORTED,
    OPTIONS_SCREEN_SMALL,
    OPTIONS_SCREEN_MEDIUM,
    OPTIONS_SCREEN_LARGE
};

#if defined(_MOUSE_ENABLED_)
static const char* values_cursor[] = {
    OPTIONS_CURSOR_SMALL,
    OPTIONS_CURSOR_MEDIUM,
    OPTIONS_CURSOR_LARGE
};
#endif

static const char* values_difficulty[] = {
    OPTIONS_SCREEN_DF_NORMAL,
    OPTIONS_SCREEN_DF_EASY,
    OPTIONS_SCREEN_DF_MEDIUM,
    OPTIONS_SCREEN_DF_HARD
};

static const char* values_approach[] = {
    OPTIONS_SCREEN_APPROACH_SWAP,
    OPTIONS_SCREEN_APPROACH_STAY
};

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
    THINK PAGING
    NAVIGATION STYLE    SWIPE
 GAME RULES
    DIFFICULTY          NORMAL, EASY, MEDIUM, HARD
 
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
#if defined(_USE_GAME_RULES_)
    { ID_MENU_RULES,                    {OPTIONS_SCREEN_RULES},                 KEYCODE(R), KEYBOARD_KEY_R },
#endif
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
    { ID_OPTION_NIGHT_DISPLAY,          {OPTIONS_SCREEN_NIGHTDISPLAY},          KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE },
    { ID_OPTION_BATTLE_FULL,            {OPTIONS_SCREEN_BATTLEREPORT},          KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
    { ID_OPTION_NIGHT_CONFIRM,          {OPTIONS_SCREEN_NIGHTCONFIRM},          KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
    { ID_OPTION_APPROACH,               {OPTIONS_SCREEN_APPROACH},              KEYCODE(4), KEYBOARD_KEY_4, TB_DOUBLE },
#if defined(_LOM_)
    { ID_OPTION_AUTO_FIGHT,             {OPTIONS_SCREEN_AUTOFIGHT},             KEYCODE(5), KEYBOARD_KEY_5, TB_DOUBLE },
    { ID_OPTION_AUTO_UNHIDE,            {OPTIONS_SCREEN_AUTOUNHIDE},            KEYCODE(6), KEYBOARD_KEY_6, TB_DOUBLE },
    { ID_OPTION_AUTO_SEEK,              {OPTIONS_SCREEN_AUTOSEEK},              KEYCODE(7), KEYBOARD_KEY_7, TB_DOUBLE },
    { ID_OPTION_AUTO_APPROACH,          {OPTIONS_SCREEN_AUTOAPPROACH},          KEYCODE(8), KEYBOARD_KEY_8, TB_DOUBLE },
#endif
};

static uitextmenuitem items_control[] = {
    { ID_OPTION_COMPASS_DELAY,          {OPTIONS_SCREEN_COMPASSDELAY},          KEYCODE(1), KEYBOARD_KEY_1, TB_DOUBLE },
    { ID_OPTION_NAVIGATION,             {OPTIONS_SCREEN_NAVIGATIONSTYLE},       KEYCODE(2), KEYBOARD_KEY_2, TB_DOUBLE },
    { ID_OPTION_KEYBOARD_STYLE,         {OPTIONS_SCREEN_KEYBOARDSTYLE},         KEYCODE(3), KEYBOARD_KEY_3, TB_DOUBLE },
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
    {   ID_OPTION_AUTO_SEEK,        OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_AUTO_APPROACH,    OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_APPROACH,         OPT_NUMBER,  2, values_approach,            nullptr, false },

    {   ID_OPTION_MOVE_INDICATORS,  OPT_BOOL,    0, values_yesno,               nullptr, false },
    {   ID_OPTION_TUTORIAL,         OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_NAVIGATION,       OPT_NUMBER,  4, values_movement,            nullptr, false },
    {   ID_OPTION_TRANSITIONS,      OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_FLIPSCREEN,       OPT_BOOL,    0, values_onoff,               nullptr, false },
    
    {   ID_OPTION_NOVELLA,          OPT_BOOL,    0, values_novella,             nullptr, false },
    
    {   ID_OPTION_COMPASS_DELAY,    OPT_NUMBER,  4, values_compass_delay,       nullptr, false },
    {   ID_OPTION_NIGHT_DISPLAY,    OPT_BOOL,    0, values_slowfast,            nullptr, false },
    {   ID_OPTION_BATTLE_FULL,      OPT_BOOL,    0, values_fullbrief,           nullptr, false },
    {   ID_OPTION_NIGHT_CONFIRM,    OPT_BOOL,    0, values_onoff,               nullptr, false },

    {   ID_OPTION_SCREENMODE,       OPT_NUMBER,  4, values_screen,              nullptr, false },

    {   ID_OPTION_KEYBOARD_STYLE,   OPT_NUMBER,  2, values_keyboard,            nullptr, false },
#if defined(_MOUSE_ENABLED_)
    {   ID_OPTION_CURSOR_SIZE,      OPT_NUMBER,  3, values_cursor,              nullptr, false },
#endif

    {   ID_OPTION_RULE_1,           OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_RULE_2,           OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_RULE_3,           OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_RULE_4,           OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_RULE_5,           OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_RULE_6,           OPT_BOOL,    0, values_onoff,               nullptr, false },
    {   ID_OPTION_RULE_7,           OPT_NUMBER,  3, values_movement_type,       nullptr, false },
    {   ID_OPTION_RULE_8,           OPT_BOOL,    0, values_onoff,               nullptr, false },

    {   ID_OPTION_DIFFICULTY,       OPT_NUMBER,  4, values_difficulty,          nullptr, false },

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

bool enableDisableOption(int id, bool value)
{
    auto option = findOption(id);
    if(option!=nullptr)
    {
        option->disabled = value;
        return true;
    }
    return false;
}

void hideOption(int id)
{
    auto option = findOption(id);
    if(option!=nullptr)
    {
        option->hidden = true;
    }
}

#if defined(_USE_GAME_RULES_)
void setRulesFromMovementType(CONFIG_MOVEMENT_TYPE type, eflags<RULEFLAGS,u64>& rules) {
    switch(type) {
        case CF_MOVEMENT_ORIGINAL:
            rules.Reset(RF_DDR_MOVEMENT_MASK);
            break;
        case CF_MOVEMENT_INTENDED:
            rules.Set(RF_DDR_MOVEMENT_SPECTRUM);
            break;
        case CF_MOVEMENT_C64:
            rules.Set(RF_DDR_MOVEMENT_C64);
            break;
    }
}

CONFIG_MOVEMENT_TYPE getMovementTypeFromRules(eflags<RULEFLAGS,u64> rules) {
    if (rules.Is(RF_DDR_MOVEMENT_SPECTRUM)) {
        return CF_MOVEMENT_INTENDED;
    }
    else if (rules.Is(RF_DDR_MOVEMENT_C64)) {
        return CF_MOVEMENT_C64;
    }
    return CF_MOVEMENT_ORIGINAL;
}
#endif

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
    
    SET_OPTION(ID_OPTION_AUTO_FIGHT,autofight);
    SET_OPTION(ID_OPTION_AUTO_UNHIDE,autounhide);
    SET_OPTION(ID_OPTION_AUTO_SEEK,autoseek);
    SET_OPTION(ID_OPTION_AUTO_APPROACH,autoapproach);
    
    SET_OPTION(ID_OPTION_MOVE_INDICATORS,showmovementindicators);
    SET_OPTION(ID_OPTION_TUTORIAL,tutorial);
    SET_OPTION(ID_OPTION_NAVIGATION,nav_mode);
    SET_OPTION(ID_OPTION_TRANSITIONS,screentransitions);
    SET_OPTION(ID_OPTION_FLIPSCREEN,flipscreen);
    
    SET_OPTION(ID_OPTION_NOVELLA,novella_pdf);
    
    SET_OPTION(ID_OPTION_COMPASS_DELAY,compass_delay);
    SET_OPTION(ID_OPTION_NIGHT_DISPLAY,night_display_fast);
    SET_OPTION(ID_OPTION_BATTLE_FULL,night_battle_full);
    SET_OPTION(ID_OPTION_NIGHT_CONFIRM,night_confirm);
    SET_OPTION(ID_OPTION_APPROACH,approach_mode);
      
    SET_OPTION(ID_OPTION_SCREENMODE, screen_mode);
    SET_OPTION(ID_OPTION_KEYBOARD_STYLE, keyboard_mode);
    
#if defined(_MOUSE_ENABLED_)
    SET_OPTION(ID_OPTION_CURSOR_SIZE, cursor_size);
#endif
    
    if ( !mr->settings->fullscreensupported )
    {
        findOption(ID_OPTION_SCREENMODE)->text = values_screen2;
    }
        
#if defined(_USE_GAME_RULES_)
    // RULES
    for( int ii=0; ii<NUMELE(rule_mapping); ii++) {
        if(rule_mapping[ii]!=RF_NONE) {
            rules[ii] = mr->settings->game_rules.Is(rule_mapping[ii]);
            findOption(ID_OPTION_RULES+ii)->var = &rules[ii];
        }else{
            rules[ii] = false;
        }
    }
    
    mr->settings->movement_type = getMovementTypeFromRules(mr->settings->game_rules);
    SET_OPTION(ID_OPTION_RULE_7, movement_type);
    
    // END RULES
#endif
        
    SET_OPTION(ID_OPTION_DIFFICULTY, game_difficulty);
    
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
        case ID_MENU_RULES:
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

    if(option->id>=ID_OPTION_RULES) {
        checkDisabledRules();
    }

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
        SetSubMenu(items_game,NUMELE(items_game));
        SetValues();
        return;
    }else if ( id == ID_MENU_DISPLAY ) {
        SetSubMenu(items_display,NUMELE(items_display));
        SetValues();
        return;
#if defined(_USE_GAME_RULES_)
    }else if ( id == ID_MENU_RULES ) {
        SetSubMenu(items_rules,NUMELE(items_rules));
        SetValues();
        checkDisabledRules();
        return;
#endif
    } else if ( id == ID_MENU_CONTROL ) {

        // check if keyboard available
#if !defined(_OS_DESKTOP_)
        hideOption(ID_OPTION_KEYBOARD_STYLE);
#endif

        SetSubMenu(items_control,NUMELE(items_control));
        
#if defined(_OS_DESKTOP_)
        mr->settings->showmovementindicators=FALSE;
#endif
        
        SetValues();
        return;
    } else if ( id == ID_MENU_HELP ) {
        SetSubMenu(items_help,NUMELE(items_help));
        
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

        CONTINUE_IF_NULL(option.var);

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
    
#if defined(_USE_GAME_RULES_)
    // RULES
    mr->settings->game_rules.Clear();
    for( int ii=0; ii<NUMELE(rule_mapping); ii++) {
        if(rule_mapping[ii]!=RF_NONE) {
            mr->settings->game_rules.Set(rule_mapping[ii], rules[ii]);
        }
    }
    
    setRulesFromMovementType(mr->settings->movement_type,mr->settings->game_rules);
    
    // END RULES
#endif

}


void panel_options::checkDisabledRules()
{
#if defined(_USE_GAME_RULES_)
    #if defined(_LOM_)
    clearRule(ID_OPTION_RULE_3, mr->settings->game_rules.Is(RF_IMPASSABLE_MOUNTAINS));
    clearRule(ID_OPTION_RULE_4, mr->settings->game_rules.Is(RF_IMPASSABLE_MOUNTAINS));
    #endif
    #if defined(_DDR_)
    clearRule(ID_OPTION_RULE_4, mr->settings->game_rules.Is(RF_IMPASSABLE_MOUNTAINS));
    #endif
#endif
}

void panel_options::clearRule(int id, bool condition)
{
    if(!enableDisableOption(id, !condition)) {
        return;
    }
    
    // we need to clear the selected value and update the text to 'off'
    if(!condition) {
        int index = id-ID_OPTION_RULES;
        mr->settings->game_rules.Reset(rule_mapping[index]);
        rules[index] = false;
        auto option = findOption(id);
        auto field = optionControls.at(id);
        field->setValue(option->text[rules[index]]);
    }
    
    // change the appearance of the disabled items
    for ( auto item : fields ) {
        if(item->getTag()==id) {
            item->setEnabled(condition);
            item->setOpacity( condition ? ALPHA(1.0) : ALPHA(0.25) );
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

void panel_options::SetSubMenu( uitextmenuitem items[], int elements )
{
    // map keyboard shortcut keys to layout children
    uishortcutkeys::registerCallback(safeArea, clickCallback);
 
    f32 width = RES(600-32);
    if(isPhoneScaleEnabled())
    {
        width+=RES(128+64);
    }
    
    f32 contentWidth = safeArea->getContentSize().width;
    
    if ( subMenuScrollView != nullptr )  {
        subMenuScrollView->removeAllChildren();
        subMenuScrollView->removeFromParent();
    }

    subMenuScrollView = ScrollView::create();
    subMenuScrollView->setLocalZOrder(ZORDER_UI);
    subMenuScrollView->setDirection(ScrollView::Direction::VERTICAL);
    subMenuScrollView->setScrollBarAutoHideEnabled(true);
    subMenuScrollView->setScrollBarAutoHideTime(0);

    auto subMenu = extensions::ScrollingMenu::create();
    subMenu->setLocalZOrder(ZORDER_UI);
    
    u32 gapY = PHONE_SCALE(RES(5));
    
    subMenuScrollView->addChild(subMenu);
    safeArea->addChild(subMenuScrollView);
    
    fields.clear();
    optionControls.clear();
    
    f32 height=0;
    for ( int ii=0; ii<elements; ii++ ) {
        
        uitextmenuitem* item = &items[ii];
        
        auto option = findOption(item->id);
        if(option->hidden)
        {
            continue;
        }
        
        auto button = uioptionitem::create( width, item );
        button->setTag(item->id);
        button->setUserData(item);
        button->setCascadeOpacityEnabled(true);
        
        addShortcutKey(button, item->id, item->keyboard_shortcut);

        // find the options
        optionControls.insert(item->id, button);
        
        auto menuItem = MenuItemNode::create(button);
        menuItem->setTag(item->id);
        menuItem->setEnabled(!option->disabled);
  
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
    
    auto maxHeight = getContentSize().height - RES(32);
    
    subMenu->setContentSize(Size(width,height) );
    subMenuScrollView->setContentSize(Size(width, std::min(height,maxHeight)));
    subMenuScrollView->setInnerContainerSize(subMenu->getContentSize());

    int adjustX = contentWidth/2 ;
    if ( adjustX + width > contentWidth ) {
        adjustX = contentWidth - width - RES(16);
    }
    
    uihelper::PositionParentTopLeft( subMenu, 0, 0 ) ;
    uihelper::PositionParentCenterLeft(subMenuScrollView,adjustX);
    
    bool scrollingEnabled = height > maxHeight ;
    subMenuScrollView->setBounceEnabled( scrollingEnabled );
    subMenuScrollView->setScrollBarEnabled( scrollingEnabled );
    
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

