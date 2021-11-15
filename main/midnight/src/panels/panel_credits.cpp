//
//  CreditsScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../cocos.h"
#include "../extensions/CustomDirector.h"
#include "../platform/Extensions.h"

#include "panel_credits.h"
#include "panel_mainmenu.h"
#include "../frontend/language.h"
#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../system/panelmanager.h"
#include "../ui/uihelper.h"

USING_NS_CC;

   
LPCSTR text[] = {
    CREDITS_ORIGINAL_GAME,  CREDITS_MIKE,
    CREDITS_PROGRAMMING,    CREDITS_PROGRAMMERS,
    CREDITS_GRAPHICS,       CREDITS_ARTISTS,
    CREDITS_TESTING,        CREDITS_TESTERS
};

/*
Alasdair Allan, Chris Booker, Timothy Bromige, Iain Brown, Basem Chbaklo,
Richard Clements, Menno Everaarts, Scott Gibbens, Martin Graaskov,
Ross Harris, Bill Hoggett, Thanos Koukoulis, Harvey Lodder, Gary Mcdonald,
Fergus Mcneill, Jordan Myers, Kai Panholzer, Guy Preston, Gemma Rigby,
Julian Rigby, Tam Rigby, Jean-Yves Rouffiac, David Ivan Perez Salazar,
Andrew Smart, Stuart Voice, Seamus Waldron, Chris Webster, Jason Webster,
David West, and all the members of midnight/mu
*/

constexpr f32 CREDIT_SCREEN_DELAY = 5.0f;

bool panel_credits::init()
{
    if ( !uipanel::init() )
    {
        return false;
    }
    
#if defined(_LOM_)
    setBackground(Color3B(152,162,209));
#endif

#if defined(_DDR_)
    setBackground(Color3B(194,182,213));
#endif

    std::string version = CREDITS_VERSION
        + Application::getInstance()->getVersion()
        + " (" + chilli::extensions::getBuildNo() + ")";

    auto label = Label::createWithTTF(uihelper::font_config_debug, version);
    label->getFontAtlas()->setAntiAliasTexParameters();
    label->setTextColor(Color4B::BLACK);
    label->setLocalZOrder(ZORDER_DEFAULT);
    uihelper::AddTopLeft(safeArea, label, RES(32), RES(32));

    auto image = Sprite::create("screens/misc/credits.png");
    uihelper::AddBottomCenter(safeArea, image);

    TTFConfig font_config_splash;

    font_config_splash.fontFilePath = FONT_FILENAME;
    font_config_splash.fontSize = RES(35);
    font_config_splash.glyphs = GlyphCollection::DYNAMIC;
    font_config_splash.outlineSize = 0;
    font_config_splash.customGlyphs = nullptr;
    font_config_splash.distanceFieldEnabled = false;
    
    f32 lineHeight = RES(40);
    f32 x = 0;
    f32 y = RES(64);
   
    for(int ii=0; ii<NUMELE(text); ii+=2)
    {
        auto label1 = Label::createWithTTF( font_config_splash, text[ii] );
        label1->getFontAtlas()->setAntiAliasTexParameters();
        label1->setTextColor( Color4B(71,76,105,255) );
        label1->setLocalZOrder(ZORDER_DEFAULT);
        label1->setScale(scale_3qtr);
        uihelper::AddTopCenter(safeArea,label1,x,y);
        y+=lineHeight*label1->getScale();

        auto label2 = Label::createWithTTF( font_config_splash, text[ii+1] );
        label2->getFontAtlas()->setAntiAliasTexParameters();
        label2->setTextColor( Color4B(_clrWhite) );
        label2->setLocalZOrder(ZORDER_DEFAULT);
        label2->setScale(scale_normal+scale_1qtr);
        uihelper::AddTopCenter(safeArea,label2,x,y);
        y+=(lineHeight*2);
    }

    Exit(CREDIT_SCREEN_DELAY);
    
    return true;
}


