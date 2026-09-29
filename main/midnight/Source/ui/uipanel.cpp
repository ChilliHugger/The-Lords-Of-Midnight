//
//  uipanel.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../ui/uipopup.h"
#include "../ui/uihelpwindow.h"
#include "uipanel.h"
#include "../system/resolutionmanager.h"
#include "../system/moonring.h"
#include "../system/helpmanager.h"
#include "../system/settingsmanager.h"
#include "../system/panelmanager.h"
#include "../ui/uihelper.h"
#include "../panels/panel_webview.h"

USING_NS_AX;

#if defined(_MOUSE_ENABLED_)
Vec2 uipanel::cursorPosition;
bool uipanel::mouseButtonDown = false;
#endif

uipanel::uipanel() :
    popupWindow(nullptr),
    i_help(nullptr),
    help_window(nullptr),
    help_pending(false),
    help_visible(HELP_NONE),
    clickCallback(nullptr),
    eventCallback(nullptr),
    safeArea(nullptr),
#if defined(_MOUSE_ENABLED_)
    imgCursor(nullptr),
    currentCursor(MOUSE_NONE),
#endif
    currentmode(MODE_NONE)
{
}

uipanel::~uipanel()
{
#if defined(_MOUSE_ENABLED_)
    // registered with fixed priority (see addMouseListener), so it has no
    // associated node and won't be cleaned up automatically by ~Node()
    if(mouseEventListener!=nullptr) {
        _eventDispatcher->removeEventListener(mouseEventListener);
    }
#endif
}

bool uipanel::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    auto size = Director::getInstance()->getVisibleSize();
    setContentSize(size);
    
    
    mr = moonring::mikesingleton();
    auto padding = mr->resolution->getSafeArea();
    
    // create a safe layout panel for icons etc
    size.width -= padding.left+padding.right;
    size.height -= padding.top+padding.bottom;
    safeArea = ax::Layer::create();
    //safeArea = ax::LayerColor::create(Color4B(_clrRed), size.width, size.height);
    //safeArea->setOpacity(ALPHA(alpha_1qtr));
    safeArea->setContentSize(size);
    safeArea->setLocalZOrder(ZORDER_FAR+500);
    safeArea->setPosition(padding.left,padding.bottom);
    addChild(safeArea);
        
    //
    // Generic mapping from Axmol
    //
    clickCallback = [&] (Ref* ref ) {
        this->OnNotification(ref);
    };
    
    eventCallback = [&] (Ref* ref, int evt ) {
        this->OnNotification(ref);
    };
    
    addKeyboardListener();

#if defined(_MOUSE_ENABLED_)
    imgCursor = Sprite::create();
    imgCursor->setScale(1.0f);
    imgCursor->setVisible(false);
    addChild(imgCursor);

    addMouseListener();
    setCursor(MOUSE_NONE);
#endif

#if defined(_MOUSE_ENABLED_)
    setOnExitTransitionDidStartCallback([&]() {
        setCursor(MOUSE_NONE);
    });
#endif

    setOnEnterTransitionDidFinishCallback([&]() {
#if defined(_MOUSE_ENABLED_)
        setCursor(MOUSE_NORMAL);
#endif
        OnShown();
    });

    
    return true;
}

void uipanel::OnNotification( Ref* element )
{
}

f32 uipanel::phoneScale() const
{
    return mr->resolution->phoneScale();
}

padding uipanel::getSafeArea() const
{
    return mr->resolution->getSafeArea();
}

bool uipanel::isPhoneScaleEnabled() const
{
    return mr->resolution->IsPhoneScaleEnabled();
}

Node* uipanel::setBackground( Color3B color )
{
    auto background = LayerColor::create(Color4B(color));
    background->setLocalZOrder(ZORDER_FAR);
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);
    return background;
}

Node* uipanel::setBackgroundCentered( LPCSTR background )
{
    auto image = Sprite::create(background);
    uihelper::AddCenter(this, image);
    return image;
}

Node* uipanel::setBackgroundToHeight( LPCSTR background, bool checkWidth )
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto p = Sprite::create((LPCSTR)background);
    f32 scale = visibleSize.height / p->getContentSize().height ;

    f32 width = p->getContentSize().width;
    f32 height = p->getContentSize().height;
    
    if(checkWidth){
        f32 w = p->getContentSize().width * scale;
        if ( w < visibleSize.width ) {
            scale = visibleSize.width / p->getContentSize().width ;
        }
    }
    
    p->setScale(scale, scale );
    p->setLocalZOrder(ZORDER_FAR);
    uihelper::AddCenter(this, p);
    return p;
}

Node* uipanel::setBackgroundToWidth( LPCSTR background )
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto p = Sprite::create((LPCSTR)background);
    f32 scale = visibleSize.width / p->getContentSize().width ;
    p->setScale(scale, scale );
    p->setLocalZOrder(ZORDER_FAR);
    uihelper::AddCenter(this, p);
    return p;
}

void uipanel::FadeExit(f32 delay )
{
    this->scheduleOnce( [&](float) {
        mr->panels->returnToPrevious(TRANSITION_FADEIN);
    }, delay, "delayed_fade_exit" );
}

void uipanel::Exit(f32 delay )
{
    this->scheduleOnce( [&](float) {
        mr->panels->returnToPrevious();
    }, delay, "delayed_exit" );
}


void uipanel::AreYouSure ( LPCSTR text, MXVoidCallback ok, MXVoidCallback notok )
{
    if ( popupWindow != nullptr ) {
        popupWindow->Close();
    }
    
    popupWindow = uipopup::create( this, point(0,0), RES(600), text );
    popupWindow->retain();
    
    popupWindow->onCancel = [&,notok] {
        RUN_ON_UI_THREAD([=, this]() {
            AX_SAFE_RELEASE_NULL(popupWindow);
            if (notok != nullptr) {
                RUN_EVENT(notok(););
            }
        });
    };
    popupWindow->onOk = [&,ok] {
        RUN_ON_UI_THREAD([=, this]() {
            AX_SAFE_RELEASE_NULL(popupWindow);
            if (ok != nullptr) {
                RUN_EVENT(ok(););
            }
        });
    };
    popupWindow->Show();
    
}

void uipanel::OpenPDF(LPCSTR pdf, bool allowInternal)
{
#if defined(_OS_ANDROID_GOOGLE_DOCS_)
    char buffer[MAX_PATH];
    snprintf(buffer, MAX_PATH, "%s%s", PDF_VIEWER, pdf);
    LPCSTR url = buffer;
#else
    LPCSTR url = pdf;
#endif

#if defined(_USE_INTERNAL_WEBVIEW_)
    if ( allowInternal && is(mr->settings->novella_internal) ) {
        panel_webview::setUrl(url);
        mr->panels->setPanelMode(MODE_WEBVIEW, TRANSITION_FADEIN, true);
        return;
    }
#endif

    Application::getInstance()->openURL(url);
}

void uipanel::FillBackground()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto p = Sprite::createWithSpriteFrameName("background_tile");
    f32 scaleY = visibleSize.height / p->getContentSize().height ;
    f32 scaleX = visibleSize.width / p->getContentSize().width ;
    p->setScale(scaleX, scaleY );
    uihelper::AddCenter(this, p);
    
}

bool uipanel::showHelpWindow ( helpid_t id, BOOL forceImportant, MXVoidCallback callback )
{
    helpmanager* help = mr->help;
    
    if ( !help->isAlways(id) ) {

        if ( !help->isNewFeature(id) ) {
            if ( isNot(mr->settings->tutorial) ) {
                if ( callback != nullptr )
                    callback();
                return true;
            }
        }
        
        if ( help->isShown(id) )
            return true;
        
        if ( help_pending && !help->isImportant(id) && !forceImportant )
            return false;
    }
    
    if ( help->isImportant(id) || forceImportant ) {
        popupHelpWindow(id,callback);
        return false;
    }
    
    // show the tutorial help
    if ( !help->isShown(HELP_TUTORIAL) ) {
        popupHelpWindow(HELP_TUTORIAL,callback);
    }
    
    // and set the required help to pending
    help_pending_id = id;
    help_pending=true;
    
    helpPending();

    return false;
}

void uipanel::helpPending()
{
    if ( i_help == nullptr )
        return;
    
    auto fadeInOut = Sequence::createWithTwoActions( FadeOut::create(0.5f),
                                                    FadeIn::create(0.5f));

    i_help->stopAllActions();
    i_help->setVisible(true);
    i_help->setOpacity(ALPHA(1.0f));
    i_help->runAction(Sequence::create(
                                Repeat::create( fadeInOut, 10),
                                DelayTime::create(15.0f),
                                FadeOut::create(0.5f),
                                ToggleVisibility::create(),
                                nullptr)
                      );
    
}

void uipanel::showHelpPending()
{
    if ( help_pending ) {
        popupHelpWindow(help_pending_id, nullptr);
    }
}

void uipanel::popupHelpWindow ( helpid_t id, MXVoidCallback callback )
{
    help_pending = false;
    help_pending_id = HELP_NONE ;
    help_visible=id;
    
    if ( i_help ) {
        i_help->stopAllActions();
        i_help->setVisible(false);
    }
    
    if ( help_window != nullptr ) {
        help_window->Close();
    }
    
    help_window = uihelpwindow::create( this, id );
    help_window->retain();
    
    help_window->Show( [&,callback] {
        RUN_ON_UI_THREAD([=, this]() {
            AX_SAFE_RELEASE_NULL(help_window);
            help_visible = HELP_NONE;
            if (callback != nullptr)
                callback();
        });
    });
    
}

void uipanel::DebugNodes()
{
    auto layer = DrawNode::create();
    layer->setContentSize( this->getContentSize() );
    
    layer->drawSolidRect(Point::ZERO, Point(this->getContentSize().width,this->getContentSize().width), Color4F(0, 0, 0, 0.25));

    
    DebugNodesChildren(layer,this, Vec2::ZERO );

    addChild(layer);
}

void uipanel::DebugNodesChildren(DrawNode* layer, Node* parent, Vec2 origin)
{
    for( const auto &node : parent->getChildren() ) {
        auto r = node->getBoundingBox();
        
        auto p = r.origin + origin ;
        
        layer->drawRect( p, Point(p.x + r.size.width, p.y + r.size.height), Color4F::WHITE);
        
        DebugNodesChildren(layer,node, p );
        
    }
    
}

void uipanel::onExit()
{
    Scene::onExit();
    OnDeActivate();
}

void uipanel::onEnter()
{
    Scene::onEnter();
    OnActivate();
}

void uipanel::OnShown( void )
{
}

void uipanel::OnDeActivate()
{
}

void uipanel::OnActivate()
{
}

void uipanel::setEnabled(bool enabled)
{
    this->enabled = enabled;
    Director::getInstance()->getEventDispatcher()->setEnabled( enabled );
}

void uipanel::pauseEvents()
{
    getEventDispatcher()->pauseEventListenersForTarget(this,true);
#if defined(_MOUSE_ENABLED_)
    ResumeMouseListener();
#endif
}

void uipanel::resumeEvents()
{
    getEventDispatcher()->resumeEventListenersForTarget(this,true);
}


bool uipanel::OnKeyboardEvent( uikeyboardevent* event )
{
    if ( event->isUp() ) {
        if ( dispatchShortcutKey(event->getKey()) )
            return true;
        //
        displayShortcuts();
    }
    
    return false;
}

void uipanel::addKeyboardListener()
{
    auto eventListener = EventListenerKeyboard::create();
    
    eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event)
    {
        auto keyEvent = mr->keyboard->createEvent(keyCode, true) ;
        if ( OnKeyboardEvent( &keyEvent ) ) {
            event->stopPropagation();
        }
    };
    
    eventListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event)
    {
        auto keyEvent = mr->keyboard->createEvent(keyCode, false) ;
        if ( OnKeyboardEvent( &keyEvent ) ) {
            event->stopPropagation();
        }
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener,this);
}

#if defined(_MOUSE_ENABLED_)
void uipanel::addMouseListener()
{
    mouseEventListener = ResumeEventListenerMouse::create();

    mouseEventListener->onMouseDown = [this](Event* event)
    {
        auto mouseEvent = static_cast<EventMouse*>(event);
        if(mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
            mouseButtonDown = true;
        }
        return false;
    };

    mouseEventListener->onMouseUp = [this](Event* event)
    {
        auto mouseEvent = static_cast<EventMouse*>(event);
        if(mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
            mouseButtonDown = false;
        }
        return false;
    };

    mouseEventListener->onMouseMove = [this](Event* event)
    {
        auto mouseEvent = static_cast<EventMouse*>(event);
        cursorPosition = Vec2(mouseEvent->getLocation().x, mouseEvent->getLocation().y);
        
        if(imgCursor!=nullptr) {
            imgCursor->setPosition(cursorPosition+cursorAnchor);
        }
        
        OnMouseMove(cursorPosition);
        
        return false;
    };
    
    // Fixed priority (rather than scene graph priority) so this always sees mouse-move
    // events first. Otherwise, axmol gadgets that enable their own mouse listener with
    // swallowing (e.g. ui::ScrollView, which turns this on for wheel scrolling) are
    // dispatched to first as they're deeper in the scene graph, and swallow the move
    // event before the cursor position below ever gets updated.
    _eventDispatcher->addEventListenerWithFixedPriority(mouseEventListener, -1);
}

void uipanel::ResumeMouseListener()
{
    mouseEventListener->Resume();
}

f32 cursor_scales[] = { 0.5f, 0.75f, 1.0f };

void uipanel::setCursor(MOUSE_CURSOR cursor)
{
    currentCursor = cursor;
    
    if(cursor == MOUSE_NONE) {
        imgCursor->setVisible(false);
        return;
    }
    
    if(mr->mouseData.empty())
        return;

    // the game's own cursor art is only available once mr->mouseData has loaded
    // (not until partway through the splash screen), so don't hide the OS cursor
    // until we actually have something to replace it with
    static bool osCursorHidden = false;
    if(!osCursorHidden) {
        osCursorHidden = true;
        Director::getInstance()->getGLView()->setCursorVisible(false);
    }

    /// small 0.5
    /// medium 0.75
    /// large 1.0
    
    f32 scale = cursor_scales[(int)mr->settings->cursor_size];
    
    cursorAnchor.x = -RES(mr->mouseData[cursor-1]->anchor.x) * scale;
    cursorAnchor.y = RES(mr->mouseData[cursor-1]->anchor.y) * scale;
    
    //cursorAnchor.x = -cursorAnchor.x;
    
    imgCursor->initWithSpriteFrameName(mr->mouseData[cursor-1]->file);
    imgCursor->setVisible(true);
    imgCursor->setAnchorPoint(uihelper::AnchorTopLeft);
    imgCursor->setPosition(cursorPosition+cursorAnchor);
    imgCursor->setLocalZOrder(ZORDER_DRAG+1);
    imgCursor->setScale(scale);
}

bool uipanel::OnMouseMove( Vec2 pos )
{
    return true;
}

Node* uipanel::childFromPoint( Node* node, Vec2 pos )
{
    for( auto n : node->getChildren()) {
        
        auto widget = dynamic_cast<ax::ui::Widget*>(n);
        
        if( widget != nullptr && widget->isEnabled() ) {
            auto b = n->getBoundingBox();
            if(b.containsPoint(pos)) {
                return widget;
            }
        }

        auto c = childFromPoint(n, pos);
        if(c!=nullptr) {
            return c;
        }
    }
    return nullptr;
}

Node* uipanel::childFromPoint( Vec2 pos )
{
    return childFromPoint(this,pos);
}

#endif
