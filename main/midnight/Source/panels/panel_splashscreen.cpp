#include "panel_splashscreen.h"
#include "panel_dedication.h"
#include "panel_mainmenu.h"
#include "../frontend/language.h"

#include "../system/resolutionmanager.h"
#include "../ui/uihelper.h"
#include "../system/moonring.h"
#include "../system/settingsmanager.h"
#include "../system/configmanager.h"
#include "../system/panelmanager.h"
#include "../system/progressmonitor.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>

USING_NS_AX;

constexpr f32 TRANSITION_TIME = 2.0f;

// ZX Spectrum screen layout: 256x192 pixels, 32x24 attribute cells
constexpr int SPECTRUM_LINES = 192;
constexpr int SPECTRUM_COLUMNS = 32;
constexpr int SPECTRUM_ROWS = 24;
constexpr int SPECTRUM_CELLS = SPECTRUM_COLUMNS*SPECTRUM_ROWS;
// Doomdark's Revenge used a fast loader, so its loading screen runs twice as fast
#if defined(_DDR_)
constexpr f32 SPECTRUM_SPEED = 2.0f;
#else
constexpr f32 SPECTRUM_SPEED = 1.0f;
#endif
constexpr f32 SPECTRUM_PIXELS_TIME = 6.0f / SPECTRUM_SPEED;      // seconds to 'load' the pixel data
constexpr f32 SPECTRUM_ATTRIBUTES_TIME = 3.0f / SPECTRUM_SPEED;  // seconds to 'load' the colour attributes
constexpr f32 SPECTRUM_PAUSE_TIME = 2.0f;                        // seconds to pause once loaded

// at least 3 seconds of splash screen, or when showing the loading screen long enough for it to finish and pause
constexpr s32 MIN_SPLASHSCREEN_TIME = 3000;
constexpr s32 MAX_SPLASHSCREEN_TIME = (s32)((SPECTRUM_PIXELS_TIME+SPECTRUM_ATTRIBUTES_TIME+SPECTRUM_PAUSE_TIME)*1000.0f);

#if defined(_LOM_)
constexpr f32 MAX_PROGRESS = 49.0f;
#endif
#if defined(_DDR_)
constexpr f32 MAX_PROGRESS = 49.0f;
#endif
#if defined(_CITADEL_)
constexpr f32 MAX_PROGRESS = 110.0f;
#endif


panel_splashscreen::panel_splashscreen() :
    progress(nullptr),
    loadingProgress(nullptr),
    loadingBars(nullptr),
    loadingComplete(false),
    loadingHeight(0),
    loadingWidth(0),
    startTime(0)
{
}

panel_splashscreen::~panel_splashscreen()
{
    SAFEDELETE(progress)
}

bool panel_splashscreen::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
    auto backgroundColor = setBackground(_clrDarkYellow);
    backgroundColor->setLocalZOrder(ZORDER_FAR-2);
     
    auto background = setBackgroundToHeight("screens/0/splash.png",false);
    if ( SETTINGS(loadingscreen) != CF_LOADING_SCREEN::OFF ) {
        initSpectrumLoad( background, "screens/0/splash.png" );
    }

#if defined(_LOM_) || defined(_CITADEL_)
    TTFConfig font_config_splash;

    font_config_splash.fontFilePath = FONT_FILENAME;
    font_config_splash.fontSize = RES(35);
    font_config_splash.glyphs = GlyphCollection::DYNAMIC;
    font_config_splash.outlineSize = 0;
    font_config_splash.distanceFieldEnabled = false;
    
    f32 lineHeight = RES(40);
    f32 x = RES(32);
    f32 y = RES(32);
    
    auto label1 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_EXPLORE );
    label1->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label1,x,y);
    bakeLabel(background,label1,Color4B(0,255,0,255));

    auto label2 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_TITLE );
    label2->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label2,x,y+(lineHeight*1));
    bakeLabel(background,label2,Color4B(253,253,0,255));

    auto label3 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_BY );
    label3->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label3,x,y+(lineHeight*2));
    bakeLabel(background,label3,Color4B(205,0,205,255));

    auto label4 = Label::createWithTTF( font_config_splash, SPLASH_SCREEN_MIKE );
    label4->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(background,label4,x,y+(lineHeight*3));
    bakeLabel(background,label4,Color4B(0,255,255,255));
#endif
    

    loadingWidth = RES(512);
    loadingHeight = RES(16);

    loadingProgress = DrawNode::create();
    uihelper::AddBottomLeft(background, loadingProgress, RES(8), RES(8));

    loadingBars = DrawNode::create();
    loadingBars->setContentSize(getContentSize());
    loadingBars->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    loadingBars->setLocalZOrder(ZORDER_FAR-1);
    uihelper::AddTopLeft(this,loadingBars);
    
    loadingComplete=false;
    scheduleUpdate();
    
    
    progress = new progressmonitor([&](int value) {
        updateProgress((f32)value / MAX_PROGRESS );
    });
    progress->Start();
    
    startTime = utils::getTimeInMilliseconds();

    // Initialise in a thread
    auto atp = AsyncTaskPool::getInstance();
    atp->enqueue(AsyncTaskPool::TaskType::TASK_IO, [&]()
    {
        mr->initialise( progress );
        
        this->scheduleOnce( [&](float) {
            complete();
        }, 0.1, "delayed_complete" );
        
    });
    
    return true;
}

void panel_splashscreen::update(float delta)
{
    f32 y=0;
    f32 width = getContentSize().width;
    int c=chilli::randomno::instance->get()&1;

    uipanel::update(delta);

    updateSpectrumLoad(delta);

    loadingProgress->clear();

    if(loadingProgress->isVisible()) {
        loadingProgress->drawSolidRect(Vec2::ZERO, Vec2(loadingWidth,loadingHeight), Color4F(_clrDarkYellow) );
        loadingProgress->drawSolidRect(Vec2::ZERO, Vec2(loadingWidth*MIN(1.0f,currentProgress),loadingHeight), Color4F(_clrDarkBlue) );
    }

    if(chilli::randomno::instance->chance(0.5)) {
        return;
    }
    
    loadingBars->clear();
    
    // keep the bars going until the image has finished loading too
    bool imageLoaded = (pixelCover==nullptr) || (spectrumTime >= SPECTRUM_PIXELS_TIME+SPECTRUM_ATTRIBUTES_TIME);
    if(loadingComplete && imageLoaded) {
        return;
    }

    while(y<getContentSize().height)
    {
        f32 height = chilli::randomno::instance->get(RES(2), RES(20));

        if(c==0) {
            loadingBars->drawSolidRect(Vec2(0,y), Vec2(0+width, y+height), Color4F(_clrDarkBlue) );
        }
        c = (c+1)&1;

        y+=height;
    }
}

void panel_splashscreen::complete()
{
    progress->Stop();
    loadingComplete=true;
    UIDEBUG("MAX_PROGRESS=%d",progress->current);
    SAFEDELETE(progress)

    // NONE means this is the first run, so the loading screen is shown and then switched off.
    // The player can switch it on to see it every time.
    if ( SETTINGS(loadingscreen) == CF_LOADING_SCREEN::NONE ) {
        SETTINGS(loadingscreen) = CF_LOADING_SCREEN::OFF;
        mr->settings->Save();
    }

    auto Duration = utils::getTimeInMilliseconds() - startTime;
    
    // we want the splash screen shown for the minimum time
    s32 minTime = (pixelCover != nullptr) ? MAX_SPLASHSCREEN_TIME : MIN_SPLASHSCREEN_TIME;
    f32 delay = MAX(0,(f32)(minTime-Duration) / 1000.0f);
    
    if ( is(SETTINGS(screentransitions)) ) {
        loadingProgress->runAction( FadeOut::create(1.0) );
    }else{
        loadingProgress->setVisible(false);
        if ( pixelCover == nullptr ) {
            delay = 1;
        }
    }
    
    this->scheduleOnce([&](float)
    {
        bool shouldShowDedication = SETTINGS(bumpDedication());
        if (shouldShowDedication) {
            mr->panels->setPanelMode(MODE_DEDICATION, TRANSITION_FADEIN );
        } else {
            mr->panels->showMainMenu();
        }
    }, delay, "show_mainmenu" );
}

void panel_splashscreen::updateProgress(f32 percent)
{
    currentProgress = percent;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// Order in which a pixel line is loaded (spectrum screen memory order). The height is split
// into 3 sections, each split into 8 parts of 8 lines. Line 1 of every part is drawn, then
// line 2 of every part and so on, before moving to the next section.
static int spectrumLineOrder( int y )
{
    int section = y>>6;
    int part = (y>>3)&7;
    int line = y&7;
    return (section*64) + (line*8) + part;
}

// Builds a black & white copy of the splash, split into attribute cells, with a white
// cover over it. Loading reveals the pixel lines in spectrum memory order, then the
// cells are removed in reading order to reveal the colour image underneath.
void panel_splashscreen::initSpectrumLoad( Node* background, const char* filename )
{
    auto image = new Image();
    image->autorelease();

    if ( !image->initWithImageFile(filename) ) {
        return;
    }

    int bpp = 0;
    if ( image->getPixelFormat() == backend::PixelFormat::RGBA8 ) bpp = 4;
    if ( image->getPixelFormat() == backend::PixelFormat::RGB8 ) bpp = 3;
    if ( bpp == 0 ) {
        return;
    }

    int w = image->getWidth();
    int h = image->getHeight();
    auto src = image->getData();

    auto luma = [&](int x, int y) -> f32 {
        auto p = src + ((y*w)+x)*bpp;
        return (0.299f*p[0] + 0.587f*p[1] + 0.114f*p[2]) / 255.0f;
    };

    // threshold each attribute cell between its darkest and lightest pixel
    std::vector<uint8_t> mono( w*h*4 );
    for ( int cy=0; cy<SPECTRUM_ROWS; cy++ ) {
        for ( int cx=0; cx<SPECTRUM_COLUMNS; cx++ ) {
            int x0 = cx*w/SPECTRUM_COLUMNS;
            int x1 = (cx+1)*w/SPECTRUM_COLUMNS;
            int y0 = cy*h/SPECTRUM_ROWS;
            int y1 = (cy+1)*h/SPECTRUM_ROWS;

            f32 lo=1.0f, hi=0.0f;
            for ( int y=y0; y<y1; y++ ) {
                for ( int x=x0; x<x1; x++ ) {
                    f32 l = luma(x,y);
                    lo = MIN(lo,l);
                    hi = MAX(hi,l);
                }
            }
            f32 threshold = (hi-lo) > 0.15f ? (lo+hi)*0.5f : 0.5f;

            for ( int y=y0; y<y1; y++ ) {
                for ( int x=x0; x<x1; x++ ) {
                    uint8_t v = luma(x,y) > threshold ? 255 : 0;
                    auto d = &mono[((y*w)+x)*4];
                    d[0]=d[1]=d[2]=v;
                    d[3]=255;
                }
            }
        }
    }

    auto texture = new Texture2D();
    texture->autorelease();
    if ( !texture->initWithData( mono.data(), mono.size(), backend::PixelFormat::RGBA8, w, h ) ) {
        return;
    }

    // child nodes are positioned in the sprite's own units, which differ from texture pixels
    spectrumWidth = background->getContentSize().width;
    spectrumHeight = background->getContentSize().height;

    // the image can be taller than the screen, so only the visible part is split into lines
    auto toImage = [&](const Vec2& p) { return background->convertToNodeSpace( convertToWorldSpace(p) ); };
    auto corner1 = toImage( Vec2::ZERO );
    auto corner2 = toImage( Vec2(getContentSize()) );
    spectrumBottom = MAX( 0.0f, MIN(corner1.y,corner2.y) );
    spectrumTop = MIN( spectrumHeight, MAX(corner1.y,corner2.y) );
    if ( spectrumTop-spectrumBottom < 1.0f ) {
        spectrumBottom = 0;
        spectrumTop = spectrumHeight;
    }

    // child nodes of the background use the image's own pixel coordinates
    auto layer = Node::create();
    layer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    layer->setPosition(Vec2::ZERO);
    layer->setContentSize(background->getContentSize());
    layer->setLocalZOrder(1);
    background->addChild(layer);

    f32 cw = spectrumWidth / SPECTRUM_COLUMNS;
    f32 ch = spectrumHeight / SPECTRUM_ROWS;
    f32 texCw = (f32)w / SPECTRUM_COLUMNS;
    f32 texCh = (f32)h / SPECTRUM_ROWS;
    f32 scaleFactor = Director::getInstance()->getContentScaleFactor();
    attributeCells.reserve(SPECTRUM_CELLS);
    for ( int cy=0; cy<SPECTRUM_ROWS; cy++ ) {
        f32 cellTop = spectrumHeight-(cy*ch);
        if ( cellTop-ch >= spectrumTop || cellTop <= spectrumBottom ) {
            continue; // off screen
        }
        for ( int cx=0; cx<SPECTRUM_COLUMNS; cx++ ) {
            // texture rects are in points, not pixels
            auto cell = Sprite::createWithTexture( texture, Rect(cx*texCw/scaleFactor, cy*texCh/scaleFactor, texCw/scaleFactor, texCh/scaleFactor) );
            cell->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
            cell->setScale( cw/(texCw/scaleFactor), ch/(texCh/scaleFactor) );
            cell->setPosition( Vec2(cx*cw, cellTop-ch) );
            layer->addChild(cell);
            attributeCells.push_back(cell);
            attributeCellRows.push_back(cy);
        }
    }

    pixelCover = DrawNode::create();
    pixelCover->setLocalZOrder(3);
    background->addChild(pixelCover);

    spectrumTime = 0;
    pixelLinesLoaded = -1;
    attributeCellsLoaded = 0;
    updateSpectrumLoad(0);
}

// Text is part of the picture: it loads in white (as ink on the black sky) and takes its
// colour when the attribute sweep reaches the row it sits on.
void panel_splashscreen::bakeLabel( Node* background, Label* label, Color4B colour )
{
    if ( pixelCover == nullptr ) {
        label->setTextColor(colour);
        return;
    }

    label->setTextColor( Color4B::WHITE );
    label->setLocalZOrder(2);   // above the black & white cells, below the pixel cover

    f32 ch = spectrumHeight / SPECTRUM_ROWS;
    int row = (int)((spectrumHeight - label->getBoundingBox().getMinY()) / ch);
    row = MAX(0,MIN(SPECTRUM_ROWS-1,row));

    bakedLabels.push_back( { label, colour, row, false } );
}

void panel_splashscreen::updateSpectrumLoad( f32 delta )
{
    if ( pixelCover == nullptr ) {
        return;
    }

    spectrumTime += delta;

    // phase 1 : pixel lines, in spectrum screen memory order
    int lines = (int)(MIN(1.0f, spectrumTime / SPECTRUM_PIXELS_TIME) * SPECTRUM_LINES);
    if ( lines != pixelLinesLoaded ) {
        pixelLinesLoaded = lines;
        pixelCover->clear();

        f32 lh = (spectrumTop-spectrumBottom) / SPECTRUM_LINES;
        for ( int y=0; y<SPECTRUM_LINES; y++ ) {
            int order = spectrumLineOrder(y);
            if ( order >= lines ) {
                f32 top = spectrumTop - (y*lh);
                pixelCover->drawSolidRect( Vec2(0,top-lh), Vec2(spectrumWidth,top), Color4F::WHITE );
            }
        }
    }

    // phase 2 : attribute sweep, removing the black & white cells to reveal the colour
    if ( spectrumTime > SPECTRUM_PIXELS_TIME ) {
        int cells = (int)(MIN(1.0f, (spectrumTime-SPECTRUM_PIXELS_TIME) / SPECTRUM_ATTRIBUTES_TIME) * attributeCells.size());
        for ( ; attributeCellsLoaded<cells; attributeCellsLoaded++ ) {
            attributeCells[attributeCellsLoaded]->setVisible(false);
        }

        if ( attributeCellsLoaded > 0 ) {
            int sweepRow = attributeCellRows[attributeCellsLoaded-1];
            for ( auto& baked : bakedLabels ) {
                if ( !baked.coloured && sweepRow >= baked.row ) {
                    baked.label->setTextColor(baked.colour);
                    baked.coloured = true;
                }
            }
        }
    }
}
