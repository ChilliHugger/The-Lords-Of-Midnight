//
//  uipopup.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "uihelper.h"
#include "uipopup.h"
#include "uipanel.h"
#include "../frontend/language.h"
#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../system/keyboardmanager.h"

USING_NS_AX;
USING_NS_AX_UI;

uipopup::uipopup() :
    parent(nullptr),
    layout(nullptr)
{
}

uipopup::~uipopup()
{
    AX_SAFE_RELEASE_NULL(parent);
}

uipopup* uipopup::create( uipanel* parent, point pos, f32 width, LPCSTR text )
{
    uipopup* node = new (std::nothrow) uipopup();
    if (node && node->initWithParent(parent,pos,width,text) )
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}


bool uipopup::initWithParent( uipanel* parent, point pos, f32 width, LPCSTR text )
{
    if ( !Element::init() )
        return false;
    
    chilli::ui::WidgetClickCallback callback = [&] (Ref* ref ) {
        auto button = static_cast<Widget*>(ref);
        if ( button == nullptr )
            return;
        
        layoutid_t id = static_cast<layoutid_t>(button->getTag());
        
        switch ( id ) {
            case ID_YES: OnYes(); break;
            case ID_NO: OnNo(); break;
            default: break;
        }
        
    };
    
    auto rect = parent->getBoundingBox();
    
    this->parent = parent;
    parent->retain();
    
    this->setContentSize( rect.size );
    this->setPosition( Vec2::ZERO );

    auto background = LayerColor::create(Color4B(0,0,0,ALPHA(0.75f)));
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);
    
    //
    f32 layout_padding = RES(20);
    f32 button_height = RES(60);
    f32 button_width = RES(128);
    
    // layout
    //auto
    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter( this, layout ) ;
    
    // add text
    auto label = Label::createWithTTF(uihelper::font_config_big, text);
    label->setColor(_clrBlue);
    label->setAlignment(TextHAlignment::CENTER);
    //label->setLineHeight(RES(FONT_SIZE_BIG));
    label->setLineSpacing(CONTENT_SCALE(0));
    label->setMaxLineWidth(width-(2*layout_padding));
    label->enableWrap(true);
    layout->addChild(label);
    label->setAnchorPoint( uihelper::AnchorTopCenter );
    
    // calc label height
    auto h = label->getContentSize().height + (3*layout_padding) + PHONE_SCALE(button_height);
    layout->setContentSize(Size(width,h));
    uihelper::PositionParentTopCenter( label, 0, layout_padding ) ;
    
    
    
    // TODO: Make these buttons part of a menu
    
    auto yes = uihelper::CreateBoxButton(Size(button_width,button_height));
    yes->setTitleText(BUTTON_YES);
    yes->setTag(ID_YES);
    yes->setFocused(true);
    layout->addChild(yes);

    uihelper::PositionParentBottomCenter(yes, -layout_padding/2, layout_padding);
    yes->setAnchorPoint( uihelper::AnchorBottomRight );
    yes->addClickEventListener(callback);

    // add cancel
    auto no = uihelper::CreateBoxButton(Size(button_width,button_height));
    no->setTitleText(BUTTON_NO);
    no->setTag(ID_NO);
    layout->addChild(no);
    uihelper::PositionParentBottomCenter(no, layout_padding/2, layout_padding);
    no->setAnchorPoint( uihelper::AnchorBottomLeft );
    no->addClickEventListener(callback);
    
    // map keyboard shortcut keys to layout children
    uishortcutkeys::registerCallback(layout,callback);
    addShortcutKey(ID_YES, K_YES);
    addShortcutKey(ID_NO, K_NO);
    addShortcutKey(ID_NO, K_ESC);
    
    return true;
}


void uipopup::addTouchListener()
{
    // now add an event listener
    // for the using cancelling
    // by touching off the main window
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event* event){
        // eat the touch in the message area
        if ( layout->getBoundingBox().containsPoint(touch->getLocation()) )
            return true;
        OnNo();
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void uipopup::Show()
{
    // stop the underlying parent
    // from getting any of the events
    parent->pauseEvents();

    addTouchListener();
    
    uishortcutkeys::addKeyboardListener(this);
    
    // and show
    this->setLocalZOrder(ZORDER_POPUP);
    parent->addChild(this);
}

void uipopup::OnYes()
{
    Close();
    if ( onOk!=nullptr )
        onOk();
}

void uipopup::OnNo()
{
    Close();
    if ( onCancel!=nullptr )
        onCancel();
}


void uipopup::Close()
{
    //
    parent->removeChild(this);
    
    // give the parent events back
    parent->resumeEvents();
    
}


