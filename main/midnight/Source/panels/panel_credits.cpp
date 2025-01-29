//
//  CreditsScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../axmol_sdk.h"
#include "../extensions/CustomDirector.h"
#include "../platform/Extensions.h"

#include "panel_credits.h"
#include "panel_mainmenu.h"
#include "../frontend/language.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

USING_NS_AX;

   
LPCSTR text[] = {
    CREDITS_ORIGINAL_GAME,  CREDITS_MIKE,
    CREDITS_PROGRAMMING,    CREDITS_PROGRAMMERS,
    CREDITS_GRAPHICS,       CREDITS_ARTISTS,
    CREDITS_TESTING,        CREDITS_TESTERS,
    CREDITS_SPECIAL_THANKS, CREDITS_THANKS,
    CREDITS_WEBSITE,        CREDITS_WEBSITE_ADDRESS
};


constexpr f32 CREDIT_SCREEN_SCROLL_DELAY = 1.0f;
constexpr f32 CREDIT_SCREEN_SCROLL_DURATION = 5.0f;
constexpr f32 CREDIT_SCREEN_DELAY = CREDIT_SCREEN_SCROLL_DELAY + CREDIT_SCREEN_SCROLL_DURATION + 2.0f;

bool panel_credits::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
#if defined(_LOM_)
    auto backgroundColour = Color3B(152,162,209);
#endif

#if defined(_DDR_)
    auto backgroundColour = Color3B(194,182,213);
#endif

    setBackground(backgroundColour);

    std::string version = CREDITS_VERSION
        + chilli::extensions::getVersion()
        + " (" + chilli::extensions::getBuildNo() + ")";

    auto label = Label::createWithTTF(uihelper::font_config_debug, version);
    label->getFontAtlas()->setAntiAliasTexParameters();
    label->setTextColor(Color4B::BLACK);
    label->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(safeArea, label, RES(32), RES(32));

    auto image = Sprite::create("screens/misc/credits.png");
    uihelper::AddBottomCenter(this, image);


    auto scrollView = ScrollView::create();
    scrollView->setContentSize(safeArea->getContentSize());
    scrollView->setDirection(ScrollView::Direction::VERTICAL);
    uihelper::AddTopLeft(safeArea, scrollView);

    auto textNode = Node::create();
    auto size = safeArea->getContentSize();
    textNode->setContentSize(safeArea->getContentSize());
    
    TTFConfig font_config_splash;

    font_config_splash.fontFilePath = FONT_FILENAME;
    font_config_splash.fontSize = RES(35);
    font_config_splash.glyphs = GlyphCollection::DYNAMIC;
    font_config_splash.outlineSize = 0;
    font_config_splash.distanceFieldEnabled = false;
    
    f32 x = RES(0);
    f32 y = getContentSize().height/2;
  
    for(int ii=0; ii<NUMELE(text); ii+=2)
    {
        y+= RES(32);
        auto label1 = Label::createWithTTF( font_config_splash, text[ii] );
        label1->getFontAtlas()->setAntiAliasTexParameters();
        label1->setTextColor( Color4B(71,76,105,255) );
        label1->setLocalZOrder(ZORDER_DEFAULT);
        label1->setScale(scale_3qtr);
        uihelper::AddTopCenter(textNode,label1,0,y);

        y+= label1->getContentSize().height * label1->getScale() + RES(8);
     
        auto label2 = Label::createWithTTF( font_config_splash, text[ii+1] );
        label2->setWidth(size.width-RES(64));
        label2->getFontAtlas()->setAntiAliasTexParameters();
        label2->setTextColor( Color4B(_clrCyan) );
        label2->setLocalZOrder(ZORDER_DEFAULT);
        label2->setScale(scale_normal);
        label2->setAlignment(TextHAlignment::CENTER);
        label2->enableOutline(Color4B(backgroundColour),RES(2));
        uihelper::AddTopCenter(textNode,label2,x,y);
        
        y += label2->getContentSize().height * label2->getScale();
    }
    
    scrollView->setInnerContainerSize( Size(size.width,y+RES(64)) );
    scrollView->setEnabled(false);
    scrollView->setScrollBarEnabled(false);
    
    uihelper::AddTopLeft(scrollView,textNode,RES(0),RES(0));

    f32 layout_padding = RES(16);
    auto gradientB = uihelper::createVerticalGradient( backgroundColour, layout_padding*2, layout_padding, size.width, 1 );
    gradientB->setLocalZOrder(ZORDER_DEFAULT+1);
    uihelper::AddBottomLeft(this, gradientB);

    this->scheduleOnce( [&,scrollView](float) {
        scrollView->scrollToBottom(CREDIT_SCREEN_SCROLL_DURATION, false);
    }, CREDIT_SCREEN_SCROLL_DELAY, "delayed_scroll" );

    Exit(CREDIT_SCREEN_DELAY);
    
    return true;
}


