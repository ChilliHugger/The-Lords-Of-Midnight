//
//  PdfViewer-apple.mm
//  midnight
//

#import <Foundation/Foundation.h>
#import <PDFKit/PDFKit.h>
#if TARGET_OS_OSX
#import <Cocoa/Cocoa.h>
#else
#import <UIKit/UIKit.h>
#endif

#include "PdfViewer-apple.h"
#include "../../axmol_sdk.h"

USING_NS_AX;

namespace
{
#if TARGET_OS_OSX
    typedef NSView PlatformView;
#else
    typedef UIView PlatformView;
#endif

    PDFView*                pdfView = nil;
    NSURLSessionDataTask*   pdfTask = nil;
    ax::Rect                pdfRect;

    PlatformView* hostView()
    {
        auto renderView = Director::getInstance()->getRenderView();
#if TARGET_OS_OSX
        return [(__bridge NSWindow*)renderView->getCocoaWindow() contentView];
#else
        return (__bridge UIView*)renderView->getEARenderView();
#endif
    }

    void applyFrame()
    {
        auto host = hostView();
        if (pdfView == nil || host == nil)
            return;

        // the design resolution is stretched over the whole host view, so work in fractions of it
        auto winSize = Director::getInstance()->getWinSize();
        CGRect bounds = host.bounds;
        CGFloat x = pdfRect.origin.x / winSize.width * bounds.size.width;
        CGFloat w = pdfRect.size.width / winSize.width * bounds.size.width;
        CGFloat h = pdfRect.size.height / winSize.height * bounds.size.height;
#if TARGET_OS_OSX
        CGFloat y = pdfRect.origin.y / winSize.height * bounds.size.height;
#else
        CGFloat y = bounds.size.height - (pdfRect.origin.y / winSize.height * bounds.size.height) - h;
#endif
        pdfView.frame = CGRectMake(x, y, w, h);
    }
}

namespace chilli
{
    namespace extensions
    {
        void showPdf(const std::string& url)
        {
            hidePdf();

            auto host = hostView();
            NSURL* nsUrl = [NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]];
            if (host == nil || nsUrl == nil)
                return;

            pdfView = [[PDFView alloc] initWithFrame:CGRectZero];
            pdfView.autoScales = YES;
            pdfView.displayMode = kPDFDisplaySinglePageContinuous;
            pdfView.hidden = YES;
            [host addSubview:pdfView];
            applyFrame();

            PDFView* view = pdfView;
            pdfTask = [[NSURLSession sharedSession] dataTaskWithURL:nsUrl
                completionHandler:^(NSData* data, NSURLResponse* response, NSError* error) {
                    if (data == nil || error != nil)
                        return;
                    dispatch_async(dispatch_get_main_queue(), ^{
                        // ignore the result if the view was closed while downloading
                        if (view != pdfView)
                            return;
                        PDFDocument* document = [[PDFDocument alloc] initWithData:data];
                        if (document == nil)
                            return;
                        view.document = document;
                        view.hidden = NO;
                    });
                }];
            [pdfTask resume];
        }

        void setPdfFrame(const ax::Rect& rect)
        {
            pdfRect = rect;
            applyFrame();
        }

        void hidePdf()
        {
            [pdfTask cancel];
            pdfTask = nil;
            [pdfView removeFromSuperview];
            pdfView = nil;
        }
    }
}
