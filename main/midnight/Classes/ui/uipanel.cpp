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
#include "../system/configmanager.h"
#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

USING_NS_CC;


bool uipanel::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    mr = moonring::mikesingleton();
    
    return true;
}

void uipanel::OnNotification( Ref* element )
{
}

Node* uipanel::setBackground( Color3B color )
{
    auto background = LayerColor::create(Color4B(color));
    uihelper::AddBottomLeft(this, background);
    uihelper::FillParent(background);
    return background;
}

Node* uipanel::setBackground( LPCSTR background )
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto p = Sprite::create((LPCSTR)background);
    f32 scale = visibleSize.height / p->getContentSize().height ;
    p->setScale(scale, scale );
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



void uipanel::AreYouSure ( LPCSTR text, MXVoidCallback ok )
{
    if ( popupWindow != nullptr ) {
        popupWindow->Close();
    }
    
    popupWindow = new uipopup( this, point(0,0), RES(600), text );
    popupWindow->onCancel = [&] {
        popupWindow = nullptr;
    };
    popupWindow->onOk = [&,ok] {
        popupWindow = nullptr;
        if ( ok != nullptr )
            ok();
    };
    popupWindow->Show();
    
}

void uipanel::OpenPDF(LPCSTR pdf)
{
#if defined(_OS_ANDROID_GOOGLE_DOCS_)
    char buffer[MAX_PATH];
    snprintf(buffer, MAX_PATH, "%s%s", PDF_VIEWER, pdf);
    Application::getInstance()->openURL(buffer);
#else
    Application::getInstance()->openURL(pdf);
#endif
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

BOOL uipanel::ShowHelpWindow ( helpid_t id, BOOL forceImportant, MXVoidCallback callback )
{
    helpmanager* help = mr->help;
    
    if ( !help->isAlways(id) ) {
        
        if ( !mr->config->tutorial )
            return TRUE;
        
        if ( help->isShown(id) )
            return TRUE;
        
        if ( help_pending && !help->isImportant(id) && !forceImportant )
            return FALSE;
    }
    
    if ( help->isImportant(id) || forceImportant ) {
        PopupHelpWindow(id,callback);
        return FALSE;
    }
    
    // show the tutorial help
    if ( !help->isShown(HELP_TUTORIAL) ) {
        PopupHelpWindow(HELP_TUTORIAL,callback);
    }
    
    // and set the required help to pending
    help_pending_id = id;
    help_pending=TRUE;
    
    HelpPending();

    return FALSE;
}

void uipanel::HelpPending()
{
    // TODO: Help Pending
    //    i_help->ShowEnable();
    //    uiaction* flash = new uiaFlash(300,10*1000);
    //    flash->delegate=this;
    //    flash->tag=9999;
    //    i_help->AddAction ( flash) ;
    //    a_help_timer->start(30*1000);
}

void uipanel::PopupHelpWindow ( helpid_t id, MXVoidCallback callback )
{
    help_pending = FALSE;
    help_pending_id = HELP_NONE ;
    
    //if ( a_help_timer ) {
    //    a_help_timer->stop();
    //}
    help_visible=id;
    
    if ( help_window != nullptr ) {
        help_window->Close();
    }
    
    help_window = new uihelpwindow( this, id );
    help_window->Show( [&,callback] {
        help_window = nullptr;
        help_visible = HELP_NONE;
        if ( callback != nullptr )
            callback();
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

void uipanel::onEnterTransitionDidFinish()
{
    Scene::onEnterTransitionDidFinish();
    OnShown();
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
