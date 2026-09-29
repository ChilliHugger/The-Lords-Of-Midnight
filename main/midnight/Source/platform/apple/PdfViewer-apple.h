//
//  PdfViewer-apple.h
//  midnight
//
//  A native PDFKit PDFView overlaid on the game view (iOS and macOS).
//

#pragma once

#include "axmol.h"

namespace chilli
{
    namespace extensions
    {
        // downloads and displays the PDF at url; the view is hidden until it has loaded
        void showPdf(const std::string& url);

        // places the view; rect is in design (world) coordinates, origin bottom left
        void setPdfFrame(const ax::Rect& rect);

        void hidePdf();
    }
}
