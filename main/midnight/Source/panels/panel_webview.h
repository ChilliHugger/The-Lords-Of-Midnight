//
//  panel_webview.h
//  midnight
//
//  Displays a document (e.g. the novella or playing guide PDF) inside the
//  game, with an exit button to return. Uses a native PDFView on iOS/macOS
//  and the Axmol WebView elsewhere.
//

#ifndef __PANEL_WEBVIEW_H_
#define __PANEL_WEBVIEW_H_

#include "../axmol_sdk.h"

// iOS and macOS show PDFs with a native PDFKit view (platform/apple/PdfViewer-apple.mm);
// the other platforms use Axmol's WebView, which is built for the platforms below
// (ui/UIWebView/UIWebView.h)
#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC
#define _USE_NATIVE_PDFVIEW_
#define _USE_INTERNAL_WEBVIEW_
#elif (defined(_WIN32) && defined(AX_ENABLE_MSEDGE_WEBVIEW2)) ||                             \
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

#if defined(_USE_NATIVE_PDFVIEW_)
    virtual ~panel_webview();
    virtual void update( f32 delta );
#endif

protected:
    virtual void OnShown();
    void layoutViewer();
    virtual bool OnKeyboardEvent( uikeyboardevent* event );

    void OnExit();

#if defined(_USE_NATIVE_PDFVIEW_)
    ax::Node* viewArea = nullptr;   // reserves the space the native view is placed over
#else
    ax::ui::WebView* webView = nullptr;
#endif
    f32 barHeight = 0;
};

#endif

#endif /* __PANEL_WEBVIEW_H_ */
