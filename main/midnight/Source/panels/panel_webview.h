//
//  panel_webview.h
//  midnight
//
//  Displays a document (e.g. the novella or playing guide PDF) inside the
//  game using the Axmol WebView, with an exit button to return.
//

#ifndef __PANEL_WEBVIEW_H_
#define __PANEL_WEBVIEW_H_

#include "../axmol_sdk.h"

// mirrors the platforms Axmol's WebView is built for (ui/UIWebView/UIWebView.h)
#if (defined(_WIN32) && defined(AX_ENABLE_MSEDGE_WEBVIEW2)) ||                             \
    (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_IOS || \
     AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#define _USE_INTERNAL_WEBVIEW_
#endif

#if defined(_USE_INTERNAL_WEBVIEW_)

#include "../ui/uipanel.h"

class panel_webview : public uipanel
{
public:
    virtual bool init();

    // the url to load the next time the panel is created
    static void setUrl( const std::string& url );

    CREATE_FUNC(panel_webview);

    virtual void OnNotification( Ref* element );

protected:
    virtual void OnShown();
    void layoutWebView();
    virtual bool OnKeyboardEvent( uikeyboardevent* event );

    void OnExit();

    ax::ui::WebView* webView = nullptr;
    f32 barHeight = 0;
};

#endif

#endif /* __PANEL_WEBVIEW_H_ */
