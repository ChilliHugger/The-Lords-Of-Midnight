//
//  panel_webview.cpp
//  midnight
//

#include "panel_webview.h"

#if defined(_USE_INTERNAL_WEBVIEW_)

#include "ui/UIWebView/UIWebView.h"
#include "../frontend/language.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

USING_NS_AX;

static std::string pendingUrl;

void panel_webview::setUrl( const std::string& url )
{
    pendingUrl = url;
}

bool panel_webview::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }

    setBackground(Color3B::BLACK);

    auto exit = uihelper::CreateBoxButton(Size(RES(160), RES(64)));
    exit->setTitleText(OPTIONS_WEBVIEW_EXIT);
    exit->setTag(ID_CLOSE);
    exit->addClickEventListener(clickCallback);
    auto buttonSize = exit->getContentSize();
    uihelper::AddTopRight(safeArea, exit, RES(16), RES(16));

    // the web view is a native widget that draws above everything, so it can't
    // sit under the exit button: fill the whole screen except a strip along the
    // top (below the safe area inset) that holds the button
    barHeight = mr->resolution->getSafeArea().top + buttonSize.height + RES(32);

    webView = ax::ui::WebView::create();
    webView->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    layoutWebView();
    webView->setScalesPageToFit(true);
    webView->loadURL(pendingUrl);
    addChild(webView);

    return true;
}

void panel_webview::layoutWebView()
{
    // the native frame is derived from the web view's world transform, so use the
    // full window size and set it again once the panel has settled in the scene
    auto size = Director::getInstance()->getWinSize();
    webView->setContentSize(Size(size.width, size.height - barHeight));
    webView->setPosition(Vec2::ZERO);
}

void panel_webview::OnShown()
{
    uipanel::OnShown();
    if ( webView != nullptr ) {
        layoutWebView();
    }
}

void panel_webview::OnExit()
{
    // native views aren't part of the transition, so remove it immediately
    if ( webView != nullptr ) {
        webView->stopLoading();
        webView->setVisible(false);
    }
    mr->panels->returnToPrevious();
}

bool panel_webview::OnKeyboardEvent( uikeyboardevent* event )
{
    if ( event->isUp() && event->getKey() == KEYCODE(ESCAPE) ) {
        OnExit();
        return true;
    }
    return uipanel::OnKeyboardEvent(event);
}

void panel_webview::OnNotification( Ref* element )
{
    auto node = static_cast<Node*>(element);
    if ( node->getTag() == ID_CLOSE ) {
        OnExit();
    }
}

#endif
