//
//  uihelpwindow.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#include "uihelper.h"
#include "uicommandwindow.h"
#include "uipanel.h"
#include "../frontend/language.h"
#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/keyboardmanager.h"

#include "../frontend/layout_id.h"
#include "../frontend/choose_id.h"

#include "../tme/tme_interface.h"
#include "../system/tmemanager.h"

#include "characters/uisinglelord.h"

USING_NS_CC;
USING_NS_CC_UI;

uicommandwindow::uicommandwindow() :
      parent(nullptr)
    , mr(nullptr)
{
}

uicommandwindow::~uicommandwindow()
{
}

uicommandwindow* uicommandwindow::create( uipanel* parent )
{
    uicommandwindow* node = new (std::nothrow) uicommandwindow();
    if (node && node->initWithParent(parent))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool uicommandwindow::initWithParent( uipanel* parent )
{
    RETURN_IF_NULL(parent) false;
    
    if ( !Element::init() )
        return false;
    
    mr = parent->GetMoonring();
    
    auto rect = parent->getBoundingBox();
    
    f32 scale = mr->resolution->phoneScale() ;
    
    grid=size(RES(128)*scale,RES(80)*scale);
    padding = size( RES(32)*scale, RES(24)*scale );
    
    this->parent = parent;
    
    setContentSize( rect.size );
    setPosition( Vec2::ZERO );
    
    auto background = LayerColor::create(Color4B(0,0,0,ALPHA(alpha_3qtr)));
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);
    
    //
    //f32 layout_padding = RES(32);
    f32 width = (5*grid.cx) + (2*padding.cx);
    f32 maxHeight = (4*grid.cy) + (2*padding.cy);
   
    // layout
    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter( this,layout ) ;

    layout->setContentSize(Size(width,maxHeight));
    
    this->setLocalZOrder(ZORDER_POPUP);
    
    initialiseCommands();
    
    addShortcutKey(ID_THINK,        K_THINK);
    addShortcutKey(ID_NIGHT,        K_NIGHT);
    addShortcutKey(ID_HOME,         K_ESC);
    addShortcutKey(ID_MAP,          K_MAP);
#if defined(_LOM_)
    addShortcutKey(ID_SEEK,         K_SEEK);
    addShortcutKey(ID_HIDE,         K_HIDE);
    addShortcutKey(ID_UNHIDE,       K_UNHIDE);
    addShortcutKey(ID_FIGHT,        K_FIGHT);
#endif
#if defined(_DDR_)
    addShortcutKey(ID_GIVE,         K_GIVE);
    addShortcutKey(ID_TAKE,         K_TAKE);
    addShortcutKey(ID_USE,          K_USE);
    addShortcutKey(ID_ENTER_TUNNEL, K_TUNNEL);
    addShortcutKey(ID_REST,         K_REST);
#endif
    addShortcutKey(ID_APPROACH,   K_APPROACH);
    addShortcutKey(ID_RECRUITMEN, K_RECRUIT);
    addShortcutKey(ID_POSTMEN,    K_POST);
    addShortcutKey(ID_ATTACK,     K_ATTACK);
    
    addShortcutKey(ID_UNDO_DAWN,  KEYCODE(F9));
    addShortcutKey(ID_UNDO,       KEYCODE(F10));
    
    return true;
}

void uicommandwindow::initialiseCommands()
{
    chilli::ui::WidgetClickCallback callback = [&] (Ref* ref ) {
        this->OnClose();
        // check for escape being pressed
        // and don't pass it on so that we can close 
        //if(uihelper::getIdFromSender(ref)!=ID_HOME)
        {
            this->parent->OnNotification(ref);
        }
    };
    
    // map keyboard shortcut keys to layout children
    uishortcutkeys::registerCallback(layout, callback);
        
    // THINK
    auto think = uihelper::CreateImageButton("i_think", ID_THINK, callback);
    addItem(think, CHOOSE_THINK);
  
#if defined(_LOM_)
    // SEEK
    auto seek = uihelper::CreateImageButton("i_seek", ID_SEEK, callback);
    addItem(seek,CHOOSE_SEEK);

    auto hide = uihelper::CreateImageButton("i_hide", ID_HIDE, callback);
    addItem(hide,CHOOSE_HIDE);

    auto unhide = uihelper::CreateImageButton("i_unhide", ID_UNHIDE, callback);
    addItem(unhide,CHOOSE_HIDE);

    // FIGHT
    auto fight = uihelper::CreateImageButton("i_fight", ID_FIGHT, callback);
    addItem(fight,CHOOSE_FIGHT);
#endif
    
#if defined(_DDR_)
    if ( tme::variables::sv_cheat_nasties_noblock ) {
        // FIGHT
        auto fight = uihelper::CreateImageButton("i_fight", ID_FIGHT, callback);
        addItem(fight,CHOOSE_FIGHT);
    }

    auto give = uihelper::CreateImageButton("i_give", ID_GIVE, callback);
    addItem(give,CHOOSE_GIVE);
    addCommandText(give);
    
    auto take = uihelper::CreateImageButton("i_take", ID_TAKE, callback);
    addItem(take,CHOOSE_TAKE);
    addCommandText(take);
    
    auto use = uihelper::CreateImageButton("i_use", ID_USE, callback);
    addItem(use,CHOOSE_USE);
    addCommandText(use);
        
    auto rest = uihelper::CreateImageButton("i_rest", ID_REST, callback);
    addItem(rest,CHOOSE_REST);
    
    auto tunnel = uihelper::CreateImageButton("i_entertunnel", ID_ENTER_TUNNEL, callback);
    addItem(tunnel, CHOOSE_TUNNEL);
#endif
    
    // MAP
    auto map = uihelper::CreateImageButton("i_map", ID_MAP, callback);
    addItem(map,CHOOSE_MAP);
    
    // NIGHT
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, callback);
    addItem(night,CHOOSE_NIGHT);
    
    // APPROACH
    auto approach = uihelper::CreateImageButton("i_approach", ID_APPROACH, callback);
    addItem(approach,CHOOSE_APPROACH);
    addCommandText(approach);
        
    // RECRUIT SOLDIERS
    auto recruitMen = uihelper::CreateImageButton("i_recruit", ID_RECRUITMEN, callback);
    addItem(recruitMen,CHOOSE_RECRUIT);
    
    // POST SOLDIERS
    auto postMen = uihelper::CreateImageButton("i_post", ID_POSTMEN, callback);
    addItem(postMen,CHOOSE_GUARD);
    
    // ATTACK
    auto attack = uihelper::CreateImageButton("i_battle", ID_ATTACK, callback);
    addItem(attack,CHOOSE_BATTLE);
    
    auto undoDawn = uihelper::CreateImageButton("undo_dawn", ID_UNDO_DAWN, callback);
    addItem(undoDawn,CHOOSE_UNDO_DAWN);
    
    auto undoLast = uihelper::CreateImageButton("undo", ID_UNDO, callback);
    addItem(undoLast,CHOOSE_UNDO_LAST);
    
    auto home = uihelper::CreateImageButton("i_home", ID_HOME, callback);
    addItem(home,CHOOSE_EXIT);
    
    // CHARACTERS
    
    // get the default faces
    for ( u32 ii=0; ii<default_characters.Count(); ii++ ) {
        char_data_t* data = (char_data_t*)TME_GetEntityUserData( default_characters[ii] );
        
        character c;
        TME_GetCharacter(c, default_characters[ii]);
        
        layoutid_t tag = (layoutid_t) (ID_SELECT_CHAR+c.id);
        
        auto lord = uisinglelord::createWithLord(c.id);
        lord->setTag(tag);
        lord->setUserData(data);
        lord->addClickEventListener(callback);
        lord->status.Reset(LORD_STATUS::status_location|LORD_STATUS::status_selected);
        lord->refreshStatus();
        addItem(lord, CHOOSE_CHAR+ii);
        
        if ( mr->settings->keyboard_mode == CF_KEYBOARD_CLASSIC )
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(data->shortcut_old));
        else
            addShortcutKey(tag, mr->keyboard->getKeyboardValue(data->shortcut_new));
        
    }
    
    setVisible(false);
}

void uicommandwindow::updateElements()
{
    character& c = TME_CurrentCharacter();
    TME_GetCharacterLocationInfo(c);
    
    #define ENABLE_IF_LOC_FLAG(x,y) \
        enableItem(x,location_flags.Is(y))
    
#if defined(_LOM_)
    // SEEK
    ENABLE_IF_LOC_FLAG(ID_SEEK, lif_seek);
    
    // HIDE
    if ( Character_IsHidden(c) ) {
        showItem(ID_UNHIDE, true);
        showItem(ID_HIDE, false);
        ENABLE_IF_LOC_FLAG(ID_UNHIDE, lif_unhide);
    } else {
        showItem(ID_UNHIDE, false);
        showItem(ID_HIDE, true);
        ENABLE_IF_LOC_FLAG(ID_HIDE, lif_hide);
    }
#endif
    
    // FIGHT
    ENABLE_IF_LOC_FLAG(ID_FIGHT, lif_fight);
    
#if defined(_DDR_)
    ENABLE_IF_LOC_FLAG(ID_GIVE, lif_give);
    setupGiveText();
    
    ENABLE_IF_LOC_FLAG(ID_TAKE, lif_take);
    setupTakeText();
    
    ENABLE_IF_LOC_FLAG(ID_USE, lif_use);
    setupUseText();
    
    ENABLE_IF_LOC_FLAG(ID_REST, lif_rest);
    ENABLE_IF_LOC_FLAG(ID_ENTER_TUNNEL, lif_enter_tunnel);
#endif
    
    // APPROACH
    ENABLE_IF_LOC_FLAG(ID_APPROACH, lif_recruitchar);
    setupApproachText();
    
    // RECRUIT SOLDIERS
    ENABLE_IF_LOC_FLAG(ID_RECRUITMEN, lif_recruitmen);
    
    // POST SOLDIERS
    ENABLE_IF_LOC_FLAG(ID_POSTMEN, lif_guardmen);
    
    // BATTLE
    ENABLE_IF_LOC_FLAG(ID_ATTACK, lif_enterbattle);
    
    // UNDO
    enableItem(ID_UNDO_DAWN, mr->stories->canUndo(savemode_dawn));
    enableItem(ID_UNDO, mr->stories->canUndo(savemode_last));

    // CHARACTERS
    for ( u32 ii=0; ii<default_characters.Count(); ii++ ) {
        character c;
        TME_GetCharacter(c, default_characters[ii] );

        layoutid_t id = (layoutid_t)((ID_CHAR1+ii));
        
        enableItem( id, Character_IsAlive(c)
                   && Character_IsControllable(c.id)
                   && c.id != TME_CurrentCharacter().id );
    }
}

#if defined(_DDR_)
void uicommandwindow::setupGiveText()
{
    setCommandBottomText(
        ID_GIVE,
        getCharacterName(
            location_flags.Is(lif_give),
            location_someone_to_give_to)
    );
        
    setCommandTopText(
        ID_GIVE,
        getObjectName(
            location_flags.Is(lif_give),
            TME_CurrentCharacter().object)
    );
}

void uicommandwindow::setupTakeText()
{
    setCommandBottomText(
        ID_TAKE,
        getObjectName(
            location_flags.Is(lif_take),
            location_object_to_take)
    );
}

void uicommandwindow::setupUseText()
{
    setCommandBottomText(
        ID_USE,
        getObjectName(
            location_flags.Is(lif_use),
            TME_CurrentCharacter().object)
    );
}
#endif

void uicommandwindow::setupApproachText()
{
    setCommandBottomText(
        ID_APPROACH,
        getCharacterName(
            location_flags.Is(lif_recruitchar),
            recruitable_characters.size() > 0 ?  recruitable_characters[0] : 0)
    );
}

std::string  uicommandwindow::getCharacterName(bool enabled, mxid characterId)
{
    if ( enabled ) {
        character c;
        TME_GetCharacter(c, characterId);
        return c.longname;
    }
    return "";
}

std::string  uicommandwindow::getObjectName(bool enabled, mxid objectId)
{
    if ( enabled ) {
        object o;
        TME_GetObject(o, objectId);
        return o.description;
    }
    return "";
}

void uicommandwindow::setCommandText(layoutid_t id, int tag,  const std::string& text)
{
    auto button = findItemById(id);
    auto label = button->getChildByTag<Label*>(tag);

    if(text.empty()) {
        label->setVisible(false);
    }else{
        auto value = StringExtensions::toUpper(text);
        label->setString(value);
        label->setVisible(true);
    }
}

void uicommandwindow::setCommandTopText(layoutid_t id, const std::string& text)
{
    setCommandText(id, TAG_COMMAND_TEXT_TOP, text);
}

void uicommandwindow::setCommandBottomText(layoutid_t id, const std::string& text)
{
    setCommandText(id, TAG_COMMAND_TEXT_BOTTOM, text);
}

void uicommandwindow::addCommandText(Node* node)
{
    // position of text messes up because of phone scale
    // so remove it, then add it back
    auto scale = node->getScale();
    node->setScale(scale_normal);
        
    auto textBottom = Label::createWithTTF( uihelper::font_config_small, "" );
    textBottom->setTag(TAG_COMMAND_TEXT_BOTTOM);
    textBottom->getFontAtlas()->setAntiAliasTexParameters();
    textBottom->setTextColor(Color4B(_clrWhite));
    textBottom->enableOutline(Color4B(_clrBlack),RES(1));
    textBottom->setAnchorPoint(uihelper::AnchorCenter);
    textBottom->setLineSpacing(CONTENT_SCALE(0));
    textBottom->setHeight(RES(FONT_SIZE_SMALL)*3); // two lines
    textBottom->setWidth(node->getContentSize().width);
    textBottom->enableWrap(true);
    textBottom->setHorizontalAlignment(TextHAlignment::CENTER);
    textBottom->setVerticalAlignment(TextVAlignment::BOTTOM);
    textBottom->setVisible(false);
    uihelper::AddBottomCenter(node, textBottom, RES(0), RES(-20));

    auto textTop = Label::createWithTTF( uihelper::font_config_small, "" );
    textTop->setTag(TAG_COMMAND_TEXT_TOP);
    textTop->getFontAtlas()->setAntiAliasTexParameters();
    textTop->setTextColor(Color4B(_clrWhite));
    textTop->enableOutline(Color4B(_clrBlack),RES(1));
    textTop->setAnchorPoint(uihelper::AnchorCenter);
    textTop->setLineSpacing(CONTENT_SCALE(0));
    textTop->setHeight(RES(FONT_SIZE_SMALL)*3); // two lines
    textTop->setWidth(node->getContentSize().width);
    textTop->enableWrap(true);
    textTop->setHorizontalAlignment(TextHAlignment::CENTER);
    textTop->setVerticalAlignment(TextVAlignment::BOTTOM);
    textTop->setVisible(false);
    uihelper::AddTopCenter(node, textTop, RES(0), RES(-24));
    
    node->setScale(scale);
}

void uicommandwindow::addTouchListener()
{
    // for the using cancelling
    // by touching off the main window
    auto listener = EventListenerTouchOneByOne::create();
    
    listener->onTouchBegan = [&](Touch* touch, Event* event){
        // eat the touch in the message area
        if ( layout->getBoundingBox().containsPoint(touch->getLocation()) )
            return true;
        OnClose();
        return true;
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void uicommandwindow::show( MXVoidCallback callback )
{
    closeCallback = callback ;
    
    // stop the underlying parent
    // from getting any of the events

    parent->pauseEvents();

    uishortcutkeys::addKeyboardListener(this);
    
    addTouchListener();
    
    // and show
    parent->addChild(this);
    setVisible(true);
    
    if ( mr->settings->screentransitions ) {
        setCascadeOpacityEnabled(true);
        setOpacity(ALPHA(alpha_zero));
        runAction(FadeIn::create(0.25f));
        
        layout->setOpacity(ALPHA(alpha_zero));
        layout->setCascadeOpacityEnabled(true);
        layout->runAction(ActionFloat::create(0.25f,alpha_zero,alpha_3qtr, [&](f32 value){
            layout->setOpacity(ALPHA(value));
        }));
    }else{
        layout->setOpacity(ALPHA(alpha_3qtr));
    }
}

void uicommandwindow::OnClose()
{    
    close();
    if ( closeCallback!=nullptr )
        closeCallback();
}

void uicommandwindow::close()
{
    setVisible(false);

    // give the parent events back
    parent->resumeEvents();
    
    //
    parent->removeChild(this);
}

void uicommandwindow::clearItems()
{
}

Widget* uicommandwindow::findItemById( layoutid_t id )
{
    for(auto const& button: elements) {
        if ( button->getTag() == id ) {
            return button;
        }
    }
    return nullptr;
}

void uicommandwindow::enableItem( layoutid_t id, bool enable )
{
    auto button = findItemById(id);
    if ( button != nullptr ) {
        button->setEnabled(enable);
        button->setOpacity( enable ? ALPHA(alpha_normal) : ALPHA(alpha_1qtr) );
    }
}

void uicommandwindow::showItem( layoutid_t id, bool show )
{
    auto button = findItemById(id);
    if ( button != nullptr ) {
        button->setVisible(show);
    }
}

void uicommandwindow::addItem( Widget* element, u32 index )
{
    auto width = (int)layout->getContentSize().width;
    
    int iy = index/(width/grid.cx);
    int ix = index%(width/grid.cx);
    
    Vec2 pos(padding.cx+(ix*grid.cx)+(grid.cx/2),padding.cy+(iy*grid.cy)+(grid.cy/2));
    
    // invert the y
    pos.y = layout->getContentSize().height - pos.y;
    
    element->setPosition(pos);
    element->setAnchorPoint(uihelper::AnchorCenter);
    layout->addChild(element);
    elements.pushBack(element);
}
