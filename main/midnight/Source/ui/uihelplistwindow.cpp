//
//  uihelplistwindow.cpp
//  midnight
//
//  Created by Chris Wild.
//

#include "uihelper.h"
#include "uihelplistwindow.h"
#include "uipanel.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include <string>
#include <algorithm>

USING_NS_AX;
USING_NS_AX_UI;

uihelplistwindow::uihelplistwindow() :
      parent(nullptr)
    , layout(nullptr)
    , scrollView(nullptr)
    , closeCallback(nullptr)
    , mr(nullptr)
{
}

uihelplistwindow::~uihelplistwindow()
{
}

uihelplistwindow* uihelplistwindow::create( uipanel* parent )
{
    uihelplistwindow* node = new (std::nothrow) uihelplistwindow();
    if (node && node->initWithParent(parent))
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}

std::string uihelplistwindow::titleFor( helpid_t id )
{
    std::string text = mr->help->Get(id);
    auto pos = text.find('\n');
    return (pos == std::string::npos) ? text : text.substr(0, pos);
}

ax::ui::Button* uihelplistwindow::createRow( helpid_t id, f32 width, f32 height )
{
    auto button = uihelper::CreateBoxButton(Size(width, height));
    button->setTag(ID_HELP_ITEM + (int)id);
    button->setTitleText(titleFor(id));
    button->setTitleColor( mr->help->isShown(id) ? _clrDarkRed : _clrGrey );

    chilli::ui::WidgetClickCallback callback = [&] (Ref* ref ) {
        auto sender = static_cast<Widget*>(ref);
        if ( sender == nullptr )
            return;
        auto selected = (helpid_t)(sender->getTag() - ID_HELP_ITEM);

        // hide (rather than close) while the item's popup is shown, so that
        // closing the popup (including via ESC) returns to this list
        setVisible(false);
        _eventDispatcher->pauseEventListenersForTarget(this,true);

        parent->popupHelpWindow(selected, [this]{
            // uihelpwindow::Close() already resumed the whole panel recursively;
            // re-pause it for the game, then resume just this list on top of
            // that, so our resume is the one that sticks for this node
            parent->pauseEvents();
            setVisible(true);
            _eventDispatcher->resumeEventListenersForTarget(this,true);
        });
    };
    button->addClickEventListener(callback);

    return button;
}

void uihelplistwindow::createList()
{
    f32 layout_padding = PHONE_SCALE(RES(32));
    f32 border = PHONE_SCALE(RES(8));
    f32 rowHeight = PHONE_SCALE(RES(64));
    f32 rowSpacing = PHONE_SCALE(RES(12));

    f32 width = std::min<f32>( this->getContentSize().width-RES(128), PHONE_SCALE(RES(800)) );
    f32 maxHeight = std::min<f32>( this->getContentSize().height-RES(128), PHONE_SCALE(RES(600)) );
    f32 rowWidth = width-(2*layout_padding);

    scrollView = ScrollView::create();
    scrollView->setDirection(ScrollView::Direction::VERTICAL);
    scrollView->setScrollBarAutoHideEnabled(true);
    scrollView->setScrollBarAutoHideTime(0);
    layout->addChild(scrollView);

    auto items = mr->help->AllItems();
    std::sort(items.begin(), items.end(), [&](helpid_t a, helpid_t b) {
        return titleFor(a) < titleFor(b);
    });

    f32 contentHeight = items.empty() ? 0 :
        (items.size()*rowHeight) + ((items.size()-1)*rowSpacing);

    f32 y = contentHeight;
    for ( auto id : items ) {
        auto row = createRow(id, rowWidth, rowHeight);
        row->setAnchorPoint(uihelper::AnchorTopCenter);
        row->setPosition( Vec2(rowWidth/2, y) );
        scrollView->addChild(row);
        y -= (rowHeight+rowSpacing);
    }

    auto innerHeight = std::min<f32>(contentHeight, maxHeight);
    auto boxHeight = innerHeight + (2*border);
    layout->setContentSize(Size(width,boxHeight));

    scrollView->setInnerContainerSize(Size(rowWidth,contentHeight));
    scrollView->setContentSize(Size(rowWidth+layout_padding,innerHeight));

    uihelper::PositionParentTopLeft(scrollView, layout_padding, border);

    bool scrollingEnabled = contentHeight > innerHeight;
    scrollView->setBounceEnabled( scrollingEnabled );
    scrollView->setScrollBarEnabled( scrollingEnabled );

    // bottom gradient
    auto gradientB = LayerGradient::create( Color4B(_clrWhite,ALPHA(0.0f)), Color4B(_clrWhite,ALPHA(1.0f)) );
    gradientB->setContentSize(Size(width-(2*border), layout_padding));
    uihelper::AddBottomLeft(layout, gradientB,border,border);

    // top gradient
    auto gradientT = LayerGradient::create( Color4B(_clrWhite,ALPHA(1.0f)), Color4B(_clrWhite,ALPHA(0.0f)) );
    gradientT->setContentSize(Size(width-(2*border), layout_padding));
    uihelper::AddTopLeft(layout, gradientT,border,border);
}

bool uihelplistwindow::initWithParent( uipanel* parent )
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

    this->setContentSize( rect.size );
    this->setPosition( Vec2::ZERO );

    auto background = LayerColor::create(Color4B(0,0,0,ALPHA(0.75f)));
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);

    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter( this,layout );

    createList();

    // Close Button in top left corner
    auto close = uihelper::CreateImageButton("close", ID_CLOSE, callback ) ;
    uihelper::AddTopLeft(layout,close, RES(8), RES(8) );
    close->setAnchorPoint(uihelper::AnchorCenter);

    this->setLocalZOrder(ZORDER_POPUP);

    // map keyboard shortcut keys to layout children
    uishortcutkeys::registerCallback(layout,callback);
    addShortcutKey(ID_CLOSE, K_ESC);

    return true;
}

void uihelplistwindow::addTouchListener()
{
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

void uihelplistwindow::show( MXVoidCallback callback )
{
    closeCallback = callback;

    parent->pauseEvents();

    addTouchListener();

    uishortcutkeys::addKeyboardListener(this);

    parent->addChild(this);
    setVisible(true);
}

void uihelplistwindow::OnClose()
{
    close();
    if ( closeCallback!=nullptr )
        closeCallback();
}

void uihelplistwindow::close()
{
    setVisible(false);

    parent->resumeEvents();
    parent->removeChild(this);
}
