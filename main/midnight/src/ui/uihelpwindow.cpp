//
//  uihelpwindow.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#include "uihelper.h"
#include "uihelpwindow.h"
#include "uipanel.h"
#include "../frontend/language.h"
#include "../system/resolutionmanager.h"
#include "../system/moonring.h"


USING_NS_CC;
USING_NS_CC_UI;

uihelpwindow::uihelpwindow() :
      parent(nullptr)
    , layout(nullptr)
    , id(HELP_NONE)
    , closeCallback(nullptr)
    , mr(nullptr)
{
}

uihelpwindow::~uihelpwindow()
{
    CC_SAFE_RELEASE_NULL(parent);
}

uihelpwindow* uihelpwindow::create(uipanel* parent, helpid_t id)
{
    uihelpwindow* node = new (std::nothrow) uihelpwindow();
    if (node && node->initWithParent(parent,id))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) ;

bool uihelpwindow::initWithParent( uipanel* parent, helpid_t id)
{
    if ( parent == nullptr )
        return false;
    
    if ( !Element::init() )
        return false;
    
    chilli::ui::WidgetClickCallback callback = [&] (Ref* ref ) {
        auto button = static_cast<Widget*>(ref);
        if ( button == nullptr )
            return;
        
        layoutid_t id = static_cast<layoutid_t>(button->getTag());
        
        switch ( id ) {
            case ID_CLOSE: OnClose(); break;
            default: break;
        }
        
    };
    
    
    auto rect = parent->getBoundingBox();
    
    this->mr = parent->GetMoonring();
    this->parent = parent;
    this->id = id;
    this->parent->retain();
    
    std::string text = parent->GetMoonring()->help->Get(id);
    
    text = ReplaceAll( text, "\t", "    ");
    
    
    this->setContentSize( rect.size );
    this->setPosition( Vec2::ZERO );

    auto background = LayerColor::create(Color4B(0,0,0,ALPHA(0.75f)));
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);
    
    //
    f32 layout_padding = PHONE_SCALE(RES(32));
    f32 border = PHONE_SCALE(RES(8));
    f32 width = std::min<f32>( this->getContentSize().width-RES(128), PHONE_SCALE(RES(800)));
    f32 maxHeight = std::min<f32>( this->getContentSize().height-RES(128), PHONE_SCALE(RES(500)));
    f32 innerWidth = width-(2*layout_padding);
    
    // layout
    //auto
    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter( this,layout ) ;

    auto scrollView = ScrollView::create();
    scrollView->setDirection(ScrollView::Direction::VERTICAL);
    layout->addChild(scrollView);

    // add text
    auto label = Label::createWithTTF(uihelper::font_config_big, text);
    label->setColor(_clrDarkRed);
    label->setAlignment(TextHAlignment::LEFT);
    label->setLineSpacing(0);
    label->setMaxLineWidth(innerWidth);
    label->enableWrap(true);
    scrollView->addChild(label);

    
    f32 textHeight = label->getContentSize().height+(2*layout_padding);
    
    // calc label height
    auto innerHeight = MIN(textHeight,maxHeight) ;
    auto boxheight = innerHeight + (2*border) ;
    layout->setContentSize(Size(width,boxheight));
    
    scrollView->setInnerContainerSize(Size(innerWidth,textHeight));
    
    // allow some extra padding for the scrollview
    // so that the scrollbar is in the right padding area
    scrollView->setContentSize(Size(innerWidth+layout_padding,innerHeight));
  
    bool scrollingEnabled = textHeight > innerHeight ;
    
    scrollView->setBounceEnabled( scrollingEnabled );
    scrollView->setScrollBarEnabled( scrollingEnabled );
    
    uihelper::PositionParentTopLeft(scrollView,layout_padding,border);
    uihelper::PositionParentTopLeft( label, 0, layout_padding ) ;
    
    // bottom gradient
    auto gradientB = LayerGradient::create( Color4B(_clrWhite,ALPHA(0.0f)), Color4B(_clrWhite,ALPHA(1.0f)) );
    gradientB->setContentSize(Size(width-(2*border), layout_padding));
    uihelper::AddBottomLeft(layout, gradientB,border,border);
    
    // top gradient
    auto gradientT = LayerGradient::create( Color4B(_clrWhite,ALPHA(1.0f)), Color4B(_clrWhite,ALPHA(0.0f)) );
    gradientT->setContentSize(Size(width-(2*border), layout_padding));
    uihelper::AddTopLeft(layout, gradientT,border,border);
    
    // Close Button in top left corner
    auto close = uihelper::CreateImageButton("close", ID_CLOSE, callback ) ;
    uihelper::AddTopLeft(layout,close, RES(8), RES(8) );
    close->setAnchorPoint(uihelper::AnchorCenter);
    
    this->setLocalZOrder(ZORDER_POPUP);
    
    // map keyboard shortcut keys to layout children
    uishortcutkeys::init(layout,callback);
    addShortcutKey(ID_CLOSE, K_ESC);
    
    return true;
}

void uihelpwindow::addTouchListener()
{
    // now add an event listener
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

void uihelpwindow::Show( MXVoidCallback callback )
{
    closeCallback = callback ;
    
    // stop the underlying parent
    // from getting any of the events
    parent->getEventDispatcher()->pauseEventListenersForTarget(parent,true);
    
    addTouchListener();
    
    uishortcutkeys::addKeyboardListener(this);

    // and show
    parent->addChild(this);
}

void uihelpwindow::OnClose()
{
    // Flsg this particular help event as having been shown
    mr->help->Shown( id );
    mr->help->Save( mr->stories->current_story() );
    
    Close();
    if ( closeCallback!=nullptr )
        closeCallback();
    
}

void uihelpwindow::Close()
{
    //
    parent->removeChild(this);
    
    // give the parent events back
    parent->getEventDispatcher()->resumeEventListenersForTarget(parent,true);
    
}



