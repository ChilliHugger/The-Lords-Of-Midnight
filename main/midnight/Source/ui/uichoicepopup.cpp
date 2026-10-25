//
//  uichoicepopup.cpp
//  midnight
//

#include "uihelper.h"
#include "uichoicepopup.h"
#include "uipanel.h"
#include "../frontend/language.h"
#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../system/keyboardmanager.h"

USING_NS_AX;
USING_NS_AX_UI;

uichoicepopup::~uichoicepopup()
{
    AX_SAFE_RELEASE_NULL(parent);
}

uichoicepopup* uichoicepopup::create( uipanel* parent, const std::string& title, const std::vector<uichoice>& choices )
{
    auto node = new (std::nothrow) uichoicepopup();
    if ( node && node->initWithParent(parent, title, choices) ) {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}

bool uichoicepopup::initWithParent( uipanel* parent, const std::string& title, const std::vector<uichoice>& choices )
{
    if ( !Element::init() )
        return false;

    this->parent = parent;
    parent->retain();
    this->choices = choices;

    setContentSize(parent->getBoundingBox().size);
    setPosition(Vec2::ZERO);

    auto background = LayerColor::create(Color4B(0,0,0,ALPHA(0.75f)));
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);

    f32 padding = RES(20);
    f32 width = RES(640);
    f32 buttonWidth = width - (2 * padding);
    f32 buttonHeight = RES(60);
    f32 spacing = RES(16);

    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    uihelper::AddCenter(this, layout);

    auto label = Label::createWithTTF(uihelper::font_config_big, title);
    label->setColor(_clrBlue);
    label->setAlignment(TextHAlignment::CENTER);
    label->setLineSpacing(CONTENT_SCALE(0));
    label->setMaxLineWidth(buttonWidth);
    label->enableWrap(true);
    label->setAnchorPoint(uihelper::AnchorTopCenter);
    layout->addChild(label);

    // the choices, then cancel
    size_t count = choices.size() + 1;
    f32 height = (3 * padding) + label->getContentSize().height + (count * buttonHeight) + ((count - 1) * spacing);
    layout->setContentSize(Size(width, height));
    uihelper::PositionParentTopCenter(label, 0, padding);

    f32 y = height - (2 * padding) - label->getContentSize().height;

    auto addButton = [&](const std::string& text, layoutid_t id, std::function<void()> action) {
        auto button = uihelper::CreateBoxButton(Size(buttonWidth, buttonHeight));
        button->setTitleText(text);
        button->setTag(id);
        button->setAnchorPoint(uihelper::AnchorTopCenter);
        button->setPosition(Vec2(width / 2, y));
        button->addClickEventListener([action](Ref*) { action(); });
        layout->addChild(button);
        y -= buttonHeight + spacing;
    };

    for ( size_t index = 0; index < choices.size(); index++ ) {
        addButton(choices[index].text, ID_NONE, [this, index] { choose(index); });
    }
    addButton(BUTTON_CANCEL, ID_CLOSE, [this] { Close(); });

    chilli::ui::WidgetClickCallback callback = [this](Ref* ref) {
        if ( static_cast<Widget*>(ref)->getTag() == ID_CLOSE )
            Close();
    };
    uishortcutkeys::registerCallback(layout, callback);
    addShortcutKey(ID_CLOSE, K_ESC);

    return true;
}

void uichoicepopup::choose( size_t index )
{
    // the action may replace the screen, so it runs once the window has gone
    auto action = choices[index].action;
    Close();
    if ( action )
        action();
}

void uichoicepopup::addTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        if ( layout->getBoundingBox().containsPoint(touch->getLocation()) )
            return true;
        Close();
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void uichoicepopup::Show()
{
    parent->pauseEvents();

    addTouchListener();
    uishortcutkeys::addKeyboardListener(this);

    setLocalZOrder(ZORDER_POPUP);
    parent->addChild(this);
    parent->getFocus().pushScope(this);
}

void uichoicepopup::Close()
{
    // a second close, e.g. a touch as well as a button, has nothing left to do
    if ( getParent() == nullptr )
        return;

    retain();
    parent->removeChild(this);
    parent->getFocus().popScope(this);
    parent->resumeEvents();
    release();
}
