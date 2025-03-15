//
//  shieldbuilder.cpp
//  midnight
//
//  Created by Chris Wild on 09/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#include "../system/moonring.h"

#include "shieldbuilder.h"
#include "../tme/tme_interface.h"
#include "../system/tmemanager.h"
#include "../ui/uihelper.h"
#include "../system/resolutionmanager.h"
#include <ranges>

USING_NS_AX;
USING_NS_AX_UI;

/*
static ShieldPart_t Parts[] =
{
    {"shield_bird", 514, 340 }, // BirdTop
    {"shield_bird", 514, 604 }, // BirdBottom
    {"shield_cat",  504, 300 }, // CatTop
    {"shield_cat",  504, 380 }, // CatMiddle
    {"shield_eye",  430, 290 }, // EyeTop Left
    {"shield_eye",  520, 621 }, // EyeBottom
    {"shield_moon", 521, 400 }, // MoonCenter
    {"shield_moon", 635, 400 }, // MoonRight
    {"shield_moon", 392, 400 }, // MoonLeft
    {"shield_star", 514, 316 }, // StarTop Middle
    {"shield_star", 400, 322 }, // StarTop left
    {"shield_star", 400, 430 }, // StarMiddle Left
    {"shield_star", 514, 602 }, // StarBottom
    {"shield_tear", 315, 310 }, // Tear1
    {"shield_tear", 717, 358 }, // Tear2
    {"shield_tear", 480, 605 }, // Tear3
    {"shield_tear", 615, 550 }, // Tear4
    {"shield_tear", 590, 620 }  // Tear5
};*/

static ShieldPart_t Parts[] =
{
    {"shield_bird", 514, 312 }, // BirdTop
    {"shield_bird", 514, 554 }, // BirdMiddle
    {"shield_bird", 514, 688 }, // BirdBottom
    {"shield_cat",  504, 316 }, // CatTop
    {"shield_cat",  504, 450 }, // CatMiddle
    {"shield_eye",  450, 270 }, // EyeTopLeft
    {"shield_eye",  580, 630 }, // EyeBottomRight
    {"shield_moon", 500, 380 }, // MoonCenter
    {"shield_moon", 620, 380 }, // MoonRight
    {"shield_moon", 370, 380 }, // MoonLeft
    {"shield_star", 514, 306 }, // StarTopMiddle
    {"shield_star", 380, 306 }, // StarTopleft
    {"shield_star", 380, 430 }, // StarMiddleLeft
    {"shield_star", 514, 670 }, // StarBottom
    {"shield_tear", 324, 294 }, // Tear1
    {"shield_tear", 710, 408 }, // Tear2
    {"shield_tear", 456, 660 }, // Tear3
    {"shield_tear", 590, 550 }, // Tear4
    {"shield_tear", 584, 670 }  // Tear5
};


static Color3B Colours[] = {
    _clrBlack,
    _clrDarkBlue,
    _clrDarkRed,
    _clrDarkMagenta,
    _clrDarkGreen,
    _clrDarkCyan,
    _clrDarkYellow,
    _clrDarkWhite
};

std::vector<Shield_t> lordshields = {
    { "ch_athoril",   PartColour::Black,  { { ShieldPart::StarTopMiddle,    PartColour::Green } } },
    { "ch_blood",     PartColour::Black,  { { ShieldPart::MoonCenter,       PartColour::Red } } },
    { "ch_brith",     PartColour::Red,    { { ShieldPart::MoonCenter,       PartColour::Yellow } } },
    { "ch_corleth",   PartColour::Red,    { { ShieldPart::BirdTop,          PartColour::Yellow } } },
    { "ch_dawn",      PartColour::Yellow, { { ShieldPart::CatMiddle ,       PartColour::Red } } },
    { "ch_dreams",    PartColour::Red,    { { ShieldPart::CatTop,           PartColour::Yellow },   {ShieldPart::BirdBottom,    PartColour::Cyan } } },
    { "ch_dregrim",   PartColour::Magenta,{ { ShieldPart::CatTop,           PartColour::Yellow },   {ShieldPart::BirdBottom,    PartColour::Cyan } } },
    { "ch_farflame",  PartColour::Red,    { { ShieldPart::Tear1,            PartColour::Yellow },   {ShieldPart::Tear2,         PartColour::Yellow}, {ShieldPart::Tear3, PartColour::Yellow } } },
    { "ch_fawkrin",   PartColour::Black,  { { ShieldPart::StarTopLeft,      PartColour::Yellow },   {ShieldPart::Tear4,         PartColour::Magenta } } },
    { "ch_gard",      PartColour::Magenta,{ { ShieldPart::CatTop,           PartColour::White },    {ShieldPart::StarBottom,    PartColour::Yellow } } },
    { "ch_gloom",     PartColour::Black,  { { ShieldPart::CatMiddle ,       PartColour::Red } } },
    { "ch_herath",    PartColour::Green,  { { ShieldPart::MoonCenter,       PartColour::White } } },
    { "ch_ithrorn",   PartColour::Red,    { { ShieldPart::CatMiddle ,       PartColour::White } } },
    { "ch_korinel",   PartColour::White,  { { ShieldPart::BirdTop,          PartColour::Red } } },
    { "ch_kumar",     PartColour::Green,  { { ShieldPart::CatMiddle ,       PartColour::White } } },
    { "ch_lorgrim",   PartColour::Yellow, { { ShieldPart::EyeTopLeft,       PartColour::Red },      {ShieldPart::Tear4,         PartColour::Blue } } },
    { "ch_lothoril",  PartColour::Magenta,{ { ShieldPart::BirdTop,          PartColour::Yellow } } },
    { "ch_luxor",     PartColour::Magenta,{ { ShieldPart::StarMiddleLeft,   PartColour::Yellow },   {ShieldPart::MoonRight,     PartColour::White } } },
    { "ch_marakith",  PartColour::Red,    { { ShieldPart::CatTop,           PartColour::Yellow },   {ShieldPart::StarBottom,    PartColour::White } } },
    { "ch_mitharg",   PartColour::Magenta,{ { ShieldPart::StarTopMiddle ,   PartColour::White } } },
    { "ch_morkin",    PartColour::Red,    { { ShieldPart::MoonLeft,         PartColour::Yellow },   {ShieldPart::Tear2,         PartColour::Yellow}, {ShieldPart::Tear5, PartColour::Yellow } } },
    { "ch_morning",   PartColour::Red,    { { ShieldPart::StarTopMiddle,    PartColour::Yellow } } },
    { "ch_rorath",    PartColour::Cyan,   { { ShieldPart::MoonCenter,       PartColour::White } } },
    { "ch_rorthron",  PartColour::Cyan,   { { ShieldPart::EyeTopLeft,       PartColour::Blue },     {ShieldPart::BirdMiddle,    PartColour::White } } },
    { "ch_shadows",   PartColour::Black,  { { ShieldPart::BirdTop,          PartColour::Magenta } } },
    { "ch_shimeril",  PartColour::Cyan,   { { ShieldPart::CatMiddle ,       PartColour::White } } },
    { "ch_thimrath",  PartColour::Green,  { { ShieldPart::BirdTop,          PartColour::White } } },
    { "ch_thrall",    PartColour::White,  { { ShieldPart::BirdTop,          PartColour::Green } } },
    { "ch_trorn",     PartColour::White,  { { ShieldPart::MoonCenter,       PartColour::Magenta } } },
    { "ch_utarg",     PartColour::Black,  { { ShieldPart::CatTop,           PartColour::Magenta },  {ShieldPart::EyeBottomRight, PartColour::Green } } },
    { "ch_whispers",  PartColour::Cyan,   { { ShieldPart::BirdTop,          PartColour::Red } } },
    { "ch_xajorkith", PartColour::Magenta,{ { ShieldPart::CatTop,           PartColour::Yellow },   {ShieldPart::StarBottom, PartColour::Cyan } } }
};


shieldbuilder::shieldbuilder() 
{
}

shieldbuilder::~shieldbuilder()
{
}

ImageView* shieldbuilder::AddPart(ImageView* node, ShieldPart part, PartColour colour) {
    auto item = Parts[(int)part];
    auto p1 = Sprite::createWithSpriteFrameName( item.name );
    p1->setColor(Colours[(int)colour]);
    uihelper::AddTopLeft(node, p1, RES(item.x * 0.25), RES(item.y * 0.25));
    p1->setAnchorPoint(uihelper::AnchorCenter);
    return node;
}

Shield_t* shieldbuilder::findShield(std::string& name)
{
    auto key = StringExtensions::toLower(name) ;
    for(std::vector<Shield_t>::iterator it = lordshields.begin(); it != lordshields.end(); ++it) {
        if (it->name == key) {
            return it.base();
        }
    }
    return nullptr;
}

bool shieldbuilder::build(mxid id, ImageView* parent)
{
    auto data = static_cast<char_data_t*>(TME_GetEntityUserData(id));
    if (data == nullptr) {
        return false;
    }

    auto shield = findShield(data->symbol);
    if (shield == nullptr) {
        return false;
    }
    parent->removeAllChildren();
    parent->loadTexture("shield", Widget::TextureResType::PLIST);
    parent->setColor(Colours[(int)shield->colour]);

    auto bevel = Sprite::createWithSpriteFrameName( "shield_bevel" );
    bevel->setOpacity(ALPHA(0.30f));
    uihelper::AddTopLeft(parent, bevel, RES(0), RES(0));

    for( auto item : shield->parts ) {
        AddPart(parent, item.part, item.colour);
    }
    
    return true;
}


