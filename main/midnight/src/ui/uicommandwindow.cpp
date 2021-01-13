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
    //CC_SAFE_RELEASE_NULL(parent);
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
    if ( parent == nullptr )
        return false;
    
    if ( !Element::init() )
        return false;
    
    mr = parent->GetMoonring();
    
    auto rect = parent->getBoundingBox();
    
    f32 scale = mr->resolution->phoneScale() ;
    
    grid=size(RES(128)*scale,RES(80)*scale);
    padding = size( RES(32)*scale, RES(24)*scale );
    
    this->parent = parent;
    // this->id = id;
    //parent->retain();
    
    this->setContentSize( rect.size );
    this->setPosition( Vec2::ZERO );
    
    auto background = LayerColor::create(Color4B(0,0,0,ALPHA(alpha_3qtr)));
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);
    
    //
    //f32 layout_padding = RES(32);
    f32 width = (5*grid.cx) + (2*padding.cx);
    f32 maxHeight = (4*grid.cy) + (2*padding.cy);
    //f32 innerWidth = width-(2*layout_padding);
    
    // layout
    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter( this,layout ) ;

    layout->setContentSize(Size(width,maxHeight));
    
    this->setLocalZOrder(ZORDER_POPUP);
    
    initialiseCommands();
    
    addShortcutKey(ID_THINK,     K_THINK);
    addShortcutKey(ID_NIGHT,     K_NIGHT);
    addShortcutKey(ID_HOME,      K_ESC);
    addShortcutKey(ID_MAP,       K_MAP);
#if defined(_LOM_)
    addShortcutKey(ID_SEEK,      K_SEEK);
    addShortcutKey(ID_HIDE,      K_HIDE);
    addShortcutKey(ID_UNHIDE,    K_UNHIDE);
    addShortcutKey(ID_FIGHT,     K_FIGHT);
#endif
#if defined(_DDR_)
    addShortcutKey(ID_GIVE,      K_GIVE);
    addShortcutKey(ID_TAKE,      K_TAKE);
    addShortcutKey(ID_USE,       K_USE);
    addShortcutKey(ID_ENTER_TUNNEL,    K_TUNNEL);
    addShortcutKey(ID_REST,      K_REST);
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
        this->parent->OnNotification(ref);
    };
    
    // map keyboard shortcut keys to layout children
    uishortcutkeys::init(layout, callback);
    
    // THINK
    auto think = uihelper::CreateImageButton("i_think", ID_THINK, callback);
    addItem(think, CHOOSE_THINK);
    //think->setTitleText("He thinks again...");
    //think->getTitleLabel()->setPosition(think->get().x, think->getPosition().y);
    //think->getTitleLabel()->setAnchorPoint(uihelper::AnchorCenter);
    
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
    
    auto givetext = Label::createWithTTF( uihelper::font_config_small, "luxor" );
    givetext->setName("givetext");
    givetext->getFontAtlas()->setAntiAliasTexParameters();
    givetext->setTextColor(Color4B(_clrWhite));
    givetext->enableOutline(Color4B(_clrBlack),RES(1));
    givetext->setAnchorPoint(uihelper::AnchorCenter);
    givetext->setWidth(give->getContentSize().width-RES(16));
    givetext->setHorizontalAlignment(TextHAlignment::CENTER);
    givetext->setVerticalAlignment(TextVAlignment::BOTTOM);
    givetext->setVisible(false);
    uihelper::AddBottomCenter(give, givetext, RES(0), RES(-16));
    
    auto take = uihelper::CreateImageButton("i_take", ID_TAKE, callback);
    addItem(take,CHOOSE_TAKE);
    
    auto use = uihelper::CreateImageButton("i_use", ID_USE, callback);
    addItem(use,CHOOSE_USE);
    
    auto rest = uihelper::CreateImageButton("i_rest", ID_REST, callback);
    addItem(rest,CHOOSE_REST);
    
    auto tunnel = uihelper::CreateImageButton("i_entertunnel", ID_ENTER_TUNNEL, callback);
    addItem(tunnel, CHOOSE_TUNNEL);
    
#endif
    
    // MAP
    auto map = uihelper::CreateImageButton("i_map", ID_MAP, callback);
    addItem(map,CHOOSE_MAP);
    
    // NIGHT 9
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, callback);
    addItem(night,CHOOSE_NIGHT);
    
    
    // APPROACH
    auto approach = uihelper::CreateImageButton("i_approach", ID_APPROACH, callback);
    addItem(approach,CHOOSE_APPROACH);
    
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
        TME_GetCharacter(c, default_characters[ii] );
        
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
}

void uicommandwindow::updateElements()
{
    character& c = TME_CurrentCharacter();
    TME_GetCharacterLocationInfo(c);
    
    
#if defined(_LOM_)
    // SEEK
    enableItem(ID_SEEK, location_flags.Is(lif_seek) );
    
    // HIDE
    if ( Character_IsHidden(c) ) {
        showItem(ID_UNHIDE, true);
        showItem(ID_HIDE, false);
    } else {
        showItem(ID_UNHIDE, false);
        showItem(ID_HIDE, true);
        enableItem(ID_HIDE, location_flags.Is(lif_hide));
    }
#endif
    
    // FIGHT
    enableItem(ID_FIGHT, location_flags.Is(lif_fight) );
    
    
#if defined(_DDR_)
    
    enableItem(ID_GIVE, location_flags.Is(lif_give) );
    setupGiveText();
    
    enableItem(ID_TAKE, location_flags.Is(lif_take) );
    enableItem(ID_USE, location_flags.Is(lif_use) );
    enableItem(ID_REST, location_flags.Is(lif_rest) );
    enableItem(ID_ENTER_TUNNEL, location_flags.Is(lif_enter_tunnel) );
    
    
#endif
    
    // APPROACH
    enableItem(ID_APPROACH, location_flags.Is(lif_recruitchar) );
    
    // RECRUIT SOLDIERS
    enableItem(ID_RECRUITMEN, location_flags.Is(lif_recruitmen) );
    
    // POST SOLDIERS
    enableItem(ID_POSTMEN, location_flags.Is(lif_guardmen) );
    
    // BATTLE
    enableItem(ID_ATTACK, location_flags.Is(lif_enterbattle) );
    
    // UNDO
    enableItem(ID_UNDO_DAWN, mr->stories->canUndo(savemode_dawn) );
    enableItem(ID_UNDO, mr->stories->canUndo(savemode_last) );
    
    // CHARACTERS
    for ( u32 ii=0; ii<default_characters.Count(); ii++ ) {
        character c;
        TME_GetCharacter(c, default_characters[ii] );

        layoutid_t id = (layoutid_t)((ID_CHAR1+ii));
        
        enableItem( id, Character_IsAlive(c)
                   && Character_IsControllable(c.id)
                   && c.id != TME_CurrentCharacter().id );
        //showItem( id, c.id != TME_CurrentCharacter().id );
        
    }

}

#if defined(_DDR_)
void uicommandwindow::setupGiveText()
{
    auto button = findItemById(ID_GIVE);
    auto text = button->getChildByName<Label*>(std::string("givetext"));

    if ( location_flags.Is(lif_give) ) {
        character c;
        TME_GetCharacter(c, location_someone_to_give_to );
        text->setString(c.shortname);
        text->setVisible(true);
    }else{
        text->setVisible(false);
    }
}
#endif

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
    parent->getEventDispatcher()->pauseEventListenersForTarget(parent,true);// now add an event listener
    
    uishortcutkeys::addKeyboardListener(this);
    
    addTouchListener();
    
    // and show
    parent->addChild(this);
    
    if ( mr->settings->screentransitions ) {
    
        this->setCascadeOpacityEnabled(true);
        this->setOpacity(ALPHA(alpha_zero));
        this->runAction(FadeIn::create(0.25f));
        
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
    //
    parent->removeChild(this);
    
    // give the parent events back
    parent->getEventDispatcher()->resumeEventListenersForTarget(parent,true);
    
}

void uicommandwindow::clearItems()
{
}

Widget* uicommandwindow::findItemById( layoutid_t id )
{
    for(auto const& button: elements)
     {
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
    
    
    Vec2 pos(padding.cx+(ix*grid.cx)+(grid.cx/2),padding.cy+(iy*grid.cy)+(grid.cy/2) );
    
    // invert the y
    pos.y = layout->getContentSize().height - pos.y;
    
    element->setPosition(pos);
    element->setAnchorPoint(uihelper::AnchorCenter);
    layout->addChild(element);
    elements.pushBack(element);

    
}

