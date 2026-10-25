//
//  uifocus.cpp
//  midnight
//

#include "uifocus.h"
#include "uipanel.h"
#include "uihelper.h"
#include "../system/moonring.h"
#include "../system/settingsmanager.h"

USING_NS_AX;
USING_NS_AX_UI;

using namespace chilli::ui;

namespace {

    // Widget keeps its click dispatch protected. Naming it through a derived class is the
    // only way to run a click without faking a touch, which needs a rendering camera.
    struct WidgetPress : public ax::ui::Widget
    {
        static void click( ax::ui::Widget* widget )
        {
            (widget->*(&WidgetPress::releaseUpEvent))();
        }
    };

    constexpr f32 HIGHLIGHT_PADDING     = 6.0f;
    constexpr f32 HIGHLIGHT_RADIUS      = 10.0f;
    constexpr f32 HIGHLIGHT_THICKNESS   = 5.0f;
    constexpr s32 HIGHLIGHT_ZORDER      = 10000;
    constexpr f32 PRESS_DURATION        = 0.1f;
    constexpr f32 REFRESH_INTERVAL      = 0.1f;
    constexpr int MAX_LINK_HOPS         = 16;
    constexpr const char* REFRESH_KEY   = "focus_refresh";
}

//
// FocusItem
//

FocusItem* FocusItem::of( Node* node )
{
    if ( node == nullptr )
        return nullptr;
    return static_cast<FocusItem*>(node->getComponent(NAME));
}

FocusItem* FocusItem::mark( Node* node, FocusStyle style )
{
    if ( node == nullptr )
        return nullptr;

    auto item = of(node);
    if ( item == nullptr ) {
        item = new (std::nothrow) FocusItem();
        if ( item == nullptr || !item->init() ) {
            AX_SAFE_DELETE(item);
            return nullptr;
        }
        item->autorelease();
        node->addComponent(item);
    }

    item->style = style;
    return item;
}

FocusItem* FocusItem::setLink( FocusDirection direction, layoutid_t id )
{
    links[(int)direction] = id;
    return this;
}

//
// uifocuscontroller
//

uifocuscontroller::~uifocuscontroller()
{
    detach();
    for ( auto root : scopes )
        root->release();
    scopes.clear();
}

bool uifocuscontroller::isEnabled() const
{
    return moonring::mikesingleton()->settings->isDpadNavigation();
}

void uifocuscontroller::attach()
{
    detach();

    auto dispatcher = panel->getEventDispatcher();

    // fixed priority listeners have no node to pause them: a modal window pauses its
    // parent panel, but the focus has to carry on working for the window's controls
    keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode key, Event* event) {
        if ( onKeyDown(key) )
            event->stopPropagation();
    };
    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode key, Event* event) {
        if ( onKeyUp(key) )
            event->stopPropagation();
    };
    dispatcher->addEventListenerWithFixedPriority(keyboardListener, -2);

    controllerListener = EventListenerController::create();
    controllerListener->onKeyDown = [this](Controller*, int key, Event* event) {
        if ( onControllerDown(key) )
            event->stopPropagation();
    };
    controllerListener->onKeyUp = [this](Controller*, int key, Event* event) {
        if ( onControllerUp(key) )
            event->stopPropagation();
    };
    dispatcher->addEventListenerWithFixedPriority(controllerListener, -2);

    if ( isEnabled() ) {
        moonring::mikesingleton()->startControllers();
    }

    panel->schedule([this](float) { refresh(); }, REFRESH_INTERVAL, REFRESH_KEY);
}

void uifocuscontroller::detach()
{
    // a panel that is only covered by another comes back to where it was
    if ( focused != nullptr && focused->getTag() != Node::INVALID_TAG )
        lastId = (layoutid_t)focused->getTag();

    setFocusNode(nullptr);
    AX_SAFE_RELEASE_NULL(highlight);

    if ( panel != nullptr ) {
        panel->unschedule(REFRESH_KEY);

        auto dispatcher = panel->getEventDispatcher();
        if ( keyboardListener != nullptr )
            dispatcher->removeEventListener(keyboardListener);
        if ( controllerListener != nullptr )
            dispatcher->removeEventListener(controllerListener);
    }

    keyboardListener = nullptr;
    controllerListener = nullptr;
}

//
// Candidates
//

Node* uifocuscontroller::scopeRoot() const
{
    while ( !scopes.empty() ) {
        auto root = scopes.back();
        if ( root->isRunning() )
            return root;
        // closed without being popped
        auto closed = scopes.back();
        const_cast<uifocuscontroller*>(this)->scopes.pop_back();
        closed->release();
    }
    return panel;
}

// The focused node is retained: it can be removed from the screen while it has focus, and
// has to stay valid until refresh() notices.
void uifocuscontroller::setFocusNode( Node* node )
{
    AX_SAFE_RETAIN(node);

    if ( focused != nullptr ) {
        if ( auto item = FocusItem::of(focused) ) {
            if ( item->onFocus )
                item->onFocus(false);
        }
    }

    hideFocus();
    AX_SAFE_RELEASE(focused);
    focused = node;
}

// A window takes over, so the focus beneath it is let go. Focus is not given back by
// remembering the node: the panel beneath may well have changed meanwhile.
void uifocuscontroller::pushScope( Node* root )
{
    if ( root == nullptr )
        return;

    setFocusNode(nullptr);
    root->retain();
    scopes.push_back(root);
    activateArmed = false;
    focusDefault();
}

void uifocuscontroller::popScope( Node* root )
{
    auto found = std::find(scopes.begin(), scopes.end(), root);
    if ( found == scopes.end() )
        return;

    scopes.erase(found);
    root->release();
    setFocusNode(nullptr);
    activateArmed = false;
    focusDefault();
}

ax::Rect uifocuscontroller::worldRect( Node* node ) const
{
    auto size = node->getContentSize();
    Vec2 corners[4] = {
        node->convertToWorldSpace(Vec2(0, 0)),
        node->convertToWorldSpace(Vec2(size.width, 0)),
        node->convertToWorldSpace(Vec2(size.width, size.height)),
        node->convertToWorldSpace(Vec2(0, size.height)),
    };

    float minX = corners[0].x, maxX = corners[0].x;
    float minY = corners[0].y, maxY = corners[0].y;
    for ( auto& corner : corners ) {
        minX = std::min(minX, corner.x);
        maxX = std::max(maxX, corner.x);
        minY = std::min(minY, corner.y);
        maxY = std::max(maxY, corner.y);
    }
    return ax::Rect(minX, minY, maxX - minX, maxY - minY);
}

bool uifocuscontroller::isWithin( Node* node, Node* ancestor ) const
{
    for ( auto n = node; n != nullptr; n = n->getParent() ) {
        if ( n == ancestor )
            return true;
    }
    return false;
}

bool uifocuscontroller::isCandidate( Node* node ) const
{
    if ( node == nullptr || !node->isRunning() )
        return false;

    auto item = FocusItem::of(node);
    if ( item == nullptr || !item->isEnabled() )
        return false;

    for ( auto n = node; n != nullptr; n = n->getParent() ) {
        if ( !n->isVisible() )
            return false;
    }

    if ( node->getDisplayedOpacity() == 0 )
        return false;

    if ( auto widget = dynamic_cast<Widget*>(node) ) {
        if ( !widget->isEnabled() || !widget->isTouchEnabled() )
            return false;
    }
    if ( auto menuItem = dynamic_cast<MenuItem*>(node) ) {
        if ( !menuItem->isEnabled() )
            return false;
    }

    // must be within the current window
    if ( !isWithin(node, scopeRoot()) )
        return false;

    auto size = node->getContentSize();
    if ( size.width <= 0 || size.height <= 0 )
        return false;

    // on screen, or inside a scroll view that is, whose content is scrolled into view
    // when it is reached
    auto director = Director::getInstance();
    ax::Rect screen(director->getVisibleOrigin().x, director->getVisibleOrigin().y,
                    director->getVisibleSize().width, director->getVisibleSize().height);

    if ( screen.intersectsRect(worldRect(node)) )
        return true;

    for ( auto n = node->getParent(); n != nullptr; n = n->getParent() ) {
        if ( dynamic_cast<ScrollView*>(n) ) {
            return screen.intersectsRect(worldRect(n));
        }
    }
    return false;
}

void uifocuscontroller::collect( Node* node, std::vector<Node*>& result ) const
{
    for ( auto child : node->getChildren() ) {
        if ( !child->isVisible() )
            continue;

        if ( FocusItem::of(child) != nullptr && isCandidate(child) )
            result.push_back(child);

        collect(child, result);
    }
}

std::vector<Node*> uifocuscontroller::candidates() const
{
    std::vector<Node*> result;
    if ( panel == nullptr )
        return result;

    auto root = scopeRoot();
    collect(root, result);
    return result;
}

chilli::ui::focus::rect uifocuscontroller::focusRect( Node* node ) const
{
    auto r = worldRect(node);
    return { r.origin.x, r.origin.y, r.size.width, r.size.height };
}

Node* uifocuscontroller::findNearest( Node* from, Direction direction ) const
{
    std::vector<Node*> nodes;
    std::vector<chilli::ui::focus::rect> rects;

    for ( auto node : candidates() ) {
        if ( node == from )
            continue;
        nodes.push_back(node);
        rects.push_back(focusRect(node));
    }

    int index = chilli::ui::focus::findNext(focusRect(from), rects, direction);
    return index == -1 ? nullptr : nodes[index];
}

layoutid_t uifocuscontroller::linkFor( Node* node, Direction direction ) const
{
    if ( node == nullptr )
        return ID_NONE;

    if ( auto item = FocusItem::of(node) ) {
        auto id = item->link(direction);
        if ( id != ID_NONE )
            return id;
    }

    for ( size_t ii = 0; ii < linkCount; ii++ ) {
        auto& link = links[ii];
        if ( link.id != node->getTag() )
            continue;

        switch ( direction ) {
            case Direction::left:   return link.left;
            case Direction::right:  return link.right;
            case Direction::up:     return link.up;
            case Direction::down:   return link.down;
        }
    }
    return ID_NONE;
}

Node* uifocuscontroller::findLinked( Node* from, Direction direction ) const
{
    auto id = linkFor(from, direction);

    // a link to something hidden or disabled carries on to wherever that item links
    for ( int hops = 0; id != ID_NONE && hops < MAX_LINK_HOPS; hops++ ) {
        auto node = uihelper::getChildByTagRecursively<Node*>(id, scopeRoot());
        if ( isCandidate(node) )
            return node;
        id = linkFor(node, direction);
    }
    return nullptr;
}

void uifocuscontroller::setLinks( const FocusLink* links, size_t count )
{
    this->links = links;
    this->linkCount = count;
}

//
// Focus
//

bool uifocuscontroller::focus( layoutid_t id )
{
    if ( panel == nullptr )
        return false;

    auto node = uihelper::getChildByTagRecursively<Node*>(id, scopeRoot());
    if ( !isCandidate(node) )
        return false;

    return focus(node);
}

bool uifocuscontroller::focus( Node* node )
{
    if ( !isCandidate(node) )
        return false;

    if ( node == focused ) {
        showFocus();
        return true;
    }

    auto lost = focused;
    AX_SAFE_RETAIN(lost);
    setFocusNode(node);

    if ( auto item = FocusItem::of(node) ) {
        if ( item->onFocus )
            item->onFocus(true);
    }

    ensureVisible(node);
    showFocus();

    if ( onChanged )
        onChanged(lost, node);
    AX_SAFE_RELEASE(lost);
    return true;
}

void uifocuscontroller::focusDefault()
{
    if ( panel == nullptr || !isEnabled() )
        return;

    if ( isCandidate(focused) ) {
        showFocus();
        return;
    }

    auto returnTo = lastId;
    lastId = ID_NONE;
    if ( returnTo != ID_NONE && focus(returnTo) )
        return;

    if ( defaultId != ID_NONE && focus(defaultId) )
        return;

    std::vector<Node*> nodes = candidates();
    std::vector<chilli::ui::focus::rect> rects;
    for ( auto node : nodes ) {
        rects.push_back(focusRect(node));
    }

    int index = chilli::ui::focus::findFirst(rects);
    if ( index != -1 )
        focus(nodes[index]);
}

void uifocuscontroller::refresh()
{
    if ( panel == nullptr )
        return;

    if ( !isEnabled() ) {
        setFocusNode(nullptr);
        return;
    }

    if ( focused != nullptr && !isCandidate(focused) )
        setFocusNode(nullptr);

    if ( focused == nullptr ) {
        focusDefault();
        return;
    }

    // an item can change size, e.g. when its text changes
    if ( highlight == nullptr || highlight->getParent() != focused
        || highlightSize != focused->getContentSize() ) {
        showFocus();
    }
}

bool uifocuscontroller::navigate( Direction direction )
{
    if ( panel == nullptr || !isEnabled() )
        return false;

    // the first press only shows where the focus is
    if ( !isCandidate(focused) ) {
        setFocusNode(nullptr);
        focusDefault();
        return true;
    }

    Node* next = nullptr;
    if ( onMove )
        next = onMove(focused, direction);
    if ( !isCandidate(next) )
        next = findLinked(focused, direction);
    if ( !isCandidate(next) )
        next = findNearest(focused, direction);

    if ( next != nullptr ) {
        focus(next);
        return true;
    }

    if ( onEdge && onEdge(direction) )
        return true;

    scroll(direction);
    return true;
}

bool uifocuscontroller::activate()
{
    if ( panel == nullptr || !isEnabled() || !isCandidate(focused) )
        return false;

    press(focused);
    return true;
}

void uifocuscontroller::press( Node* node )
{
    if ( auto item = FocusItem::of(node) ) {
        if ( item->onActivate ) {
            item->onActivate();
            return;
        }
    }

    // the click is run after a moment so the press can be seen
    node->retain();

    if ( auto menuItem = dynamic_cast<MenuItem*>(node) ) {
        menuItem->selected();
        node->runAction(Sequence::createWithTwoActions(
            DelayTime::create(PRESS_DURATION),
            CallFunc::create([menuItem] {
                menuItem->unselected();
                menuItem->activate();
                menuItem->release();
            })));
        return;
    }

    if ( auto widget = dynamic_cast<Widget*>(node) ) {
        widget->setHighlighted(true);
        node->runAction(Sequence::createWithTwoActions(
            DelayTime::create(PRESS_DURATION),
            CallFunc::create([widget] {
                widget->setHighlighted(false);
                WidgetPress::click(widget);
                widget->release();
            })));
        return;
    }

    node->release();
}

//
// Scrolling
//

void uifocuscontroller::ensureVisible( Node* node )
{
    ScrollView* scrollView = nullptr;
    for ( auto n = node->getParent(); n != nullptr; n = n->getParent() ) {
        scrollView = dynamic_cast<ScrollView*>(n);
        if ( scrollView != nullptr )
            break;
    }
    if ( scrollView == nullptr )
        return;

    if ( auto pageView = dynamic_cast<PageView*>(scrollView) ) {
        auto& pages = pageView->getItems();
        for ( ssize_t index = 0; index < pages.size(); index++ ) {
            for ( auto n = node; n != nullptr; n = n->getParent() ) {
                if ( n == pages.at(index) ) {
                    if ( pageView->getCurrentPageIndex() != index )
                        pageView->scrollToItem(index);
                    return;
                }
            }
        }
        return;
    }

    // the node's rectangle in the scroll view's own space
    auto target = worldRect(node);
    auto bottomLeft = scrollView->convertToNodeSpace(target.origin);
    auto topRight = scrollView->convertToNodeSpace(Vec2(target.getMaxX(), target.getMaxY()));
    auto view = scrollView->getContentSize();

    Vec2 delta;
    if ( topRight.y > view.height )
        delta.y = view.height - topRight.y;
    else if ( bottomLeft.y < 0 )
        delta.y = -bottomLeft.y;

    if ( topRight.x > view.width )
        delta.x = view.width - topRight.x;
    else if ( bottomLeft.x < 0 )
        delta.x = -bottomLeft.x;

    if ( delta.isZero() )
        return;

    auto inner = scrollView->getInnerContainerSize();
    auto position = scrollView->getInnerContainerPosition() + delta;
    position.x = std::max(std::min(position.x, 0.0f), std::min(view.width - inner.width, 0.0f));
    position.y = std::max(std::min(position.y, 0.0f), std::min(view.height - inner.height, 0.0f));
    scrollView->setInnerContainerPosition(position);
}

bool uifocuscontroller::scroll( Direction direction )
{
    if ( panel == nullptr )
        return false;

    ScrollView* found = nullptr;
    std::function<void(Node*)> search = [&](Node* node) {
        for ( auto child : node->getChildren() ) {
            if ( found != nullptr || !child->isVisible() )
                continue;
            auto scrollView = dynamic_cast<ScrollView*>(child);
            if ( scrollView != nullptr && dynamic_cast<PageView*>(child) == nullptr
                && scrollView->isEnabled() && scrollView->isTouchEnabled() ) {
                found = scrollView;
                return;
            }
            search(child);
        }
    };
    search(scopeRoot());

    if ( found == nullptr )
        return false;

    bool vertical = (direction == Direction::up || direction == Direction::down);
    auto view = found->getContentSize();
    auto inner = found->getInnerContainerSize();
    auto position = found->getInnerContainerPosition();

    // moving the contents down shows what is above
    f32 step = (vertical ? view.height : view.width) / 3;
    switch ( direction ) {
        case Direction::up:     position.y -= step; break;
        case Direction::down:   position.y += step; break;
        case Direction::left:   position.x += step; break;
        case Direction::right:  position.x -= step; break;
    }

    // the inner container's position is 0 with the end of the contents showing, and negative
    // by as much as the contents are larger than the view with the start showing
    position.x = std::max(std::min(position.x, 0.0f), std::min(view.width - inner.width, 0.0f));
    position.y = std::max(std::min(position.y, 0.0f), std::min(view.height - inner.height, 0.0f));

    if ( position == found->getInnerContainerPosition() )
        return false;

    found->setInnerContainerPosition(position);
    return true;
}

//
// Drawing
//

void uifocuscontroller::hideFocus()
{
    if ( highlight != nullptr && highlight->getParent() != nullptr )
        highlight->removeFromParent();
}

void uifocuscontroller::showFocus()
{
    if ( focused == nullptr || !isEnabled() ) {
        hideFocus();
        return;
    }

    if ( highlight == nullptr ) {
        highlight = DrawNode::create();
        highlight->retain();
    }

    auto item = FocusItem::of(focused);
    highlightStyle = item != nullptr ? item->style : FocusStyle::outline;
    highlightSize = focused->getContentSize();

    highlight->clear();
    highlight->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    highlight->setIgnoreAnchorPointForPosition(true);
    highlight->setPosition(Vec2::ZERO);

    f32 padding = RES(HIGHLIGHT_PADDING);
    f32 radius = RES(HIGHLIGHT_RADIUS);
    f32 thickness = RES(HIGHLIGHT_THICKNESS);

    // rounded rectangle, built as a polygon
    std::vector<Vec2> outline;
    auto size = highlightSize;
    Vec2 low(-padding, -padding);
    Vec2 high(size.width + padding, size.height + padding);
    radius = std::min(radius, std::min((high.x - low.x), (high.y - low.y)) / 2);

    const int steps = 6;
    const Vec2 centres[4] = {
        Vec2(high.x - radius, high.y - radius),
        Vec2(low.x + radius, high.y - radius),
        Vec2(low.x + radius, low.y + radius),
        Vec2(high.x - radius, low.y + radius),
    };
    for ( int corner = 0; corner < 4; corner++ ) {
        for ( int step = 0; step <= steps; step++ ) {
            float angle = (M_PI / 2) * ((float)corner + ((float)step / steps));
            outline.push_back(centres[corner] + Vec2(cosf(angle), sinf(angle)) * radius);
        }
    }

    auto colour = Color4F(FOCUS_COLOUR);

    if ( highlightStyle == FocusStyle::background ) {
        highlight->drawPolygon(outline.data(), (int)outline.size(), colour, 0, colour, true);
        highlight->setLocalZOrder(-HIGHLIGHT_ZORDER);
    } else {
        // a dark edge keeps the outline visible against light and coloured backgrounds
        highlight->drawPolygon(outline.data(), (int)outline.size(), Color4F(0, 0, 0, 0), thickness + RES(3),
                               Color4F(0, 0, 0, 0.6f), true);
        highlight->drawPolygon(outline.data(), (int)outline.size(), Color4F(0, 0, 0, 0), thickness, colour, true);
        highlight->setLocalZOrder(HIGHLIGHT_ZORDER);
    }

    if ( highlight->getParent() != focused ) {
        hideFocus();
        focused->addChild(highlight);
    }
}

//
// Input
//

bool uifocuscontroller::getDirection( EventKeyboard::KeyCode key, Direction& direction ) const
{
    switch ( key ) {
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:    direction = Direction::left; return true;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:   direction = Direction::right; return true;
        case EventKeyboard::KeyCode::KEY_UP_ARROW:      direction = Direction::up; return true;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:    direction = Direction::down; return true;
        default:                                        return false;
    }
}

bool uifocuscontroller::getDirection( int key, Direction& direction ) const
{
    switch ( key ) {
        case Controller::Key::BUTTON_DPAD_LEFT:     direction = Direction::left; return true;
        case Controller::Key::BUTTON_DPAD_RIGHT:    direction = Direction::right; return true;
        case Controller::Key::BUTTON_DPAD_UP:       direction = Direction::up; return true;
        case Controller::Key::BUTTON_DPAD_DOWN:     direction = Direction::down; return true;
        default:                                    return false;
    }
}

bool uifocuscontroller::isActivateKey( EventKeyboard::KeyCode key ) const
{
    return key == EventKeyboard::KeyCode::KEY_ENTER
        || key == EventKeyboard::KeyCode::KEY_KP_ENTER
        || key == EventKeyboard::KeyCode::KEY_SPACE;
}

bool uifocuscontroller::onKeyDown( EventKeyboard::KeyCode key )
{
    if ( !isEnabled() )
        return false;

    Direction direction;
    if ( getDirection(key, direction) )
        return navigate(direction);

    if ( isActivateKey(key) && isCandidate(focused) ) {
        activateArmed = true;
        return true;
    }
    return false;
}

bool uifocuscontroller::onKeyUp( EventKeyboard::KeyCode key )
{
    if ( !isEnabled() )
        return false;

    Direction direction;
    if ( getDirection(key, direction) )
        return true;

    if ( isActivateKey(key) && activateArmed ) {
        activateArmed = false;
        return activate();
    }
    return false;
}

bool uifocuscontroller::onControllerDown( int key )
{
    if ( !isEnabled() )
        return false;

    Direction direction;
    if ( getDirection(key, direction) )
        return navigate(direction);

    if ( key == Controller::Key::BUTTON_A && isCandidate(focused) ) {
        activateArmed = true;
        return true;
    }
    return false;
}

bool uifocuscontroller::onControllerUp( int key )
{
    if ( !isEnabled() )
        return false;

    Direction direction;
    if ( getDirection(key, direction) )
        return true;

    if ( key == Controller::Key::BUTTON_A && activateArmed ) {
        activateArmed = false;
        return activate();
    }

    // B on a game controller and Play/Pause on the Siri Remote go back, as Escape does
    if ( key == Controller::Key::BUTTON_B || key == Controller::Key::BUTTON_X ) {
        goBack();
        return true;
    }
    return false;
}

void uifocuscontroller::goBack()
{
    if ( panel == nullptr )
        return;

    EventKeyboard released(EventKeyboard::KeyCode::KEY_ESCAPE, false);
    panel->getEventDispatcher()->dispatchEvent(&released);
}
