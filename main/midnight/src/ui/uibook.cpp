//
//  uibook.cpp
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//
#include "uihelper.h"
#include "uibook.h"
#include "../system/resolutionmanager.h"

#define BOOK_IMAGE              "book"
#define BOOKMARK_IMAGE          "bookmark"
#define BOOKMARK_OVERLAY_IMAGE  "bookmark_overlay"

USING_NS_CC;

// TODO: These should should be read from the config file
// like they used to be tag panel_menu::stories
//
static rgb_t bookmark_colours[] = {
    rgb_t(0xde, 0x00, 0x00),
    rgb_t(0xde, 0x7e, 0x00),
    rgb_t(0xcc, 0xcc, 0x00),
    rgb_t(0x69, 0xa9, 0x00),
    rgb_t(0x00, 0xb2, 0xa4),
    rgb_t(0x00, 0x7e, 0xcc),
    rgb_t(0x84, 0x00, 0xdb),
    rgb_t(0x96, 0x96, 0x96)
};

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

uibook* uibook::createWithChapter( storyheader_t* story )
{
    uibook* node = new (std::nothrow) uibook();
    if (node && node->initWithChapter(story))
    {
        node->autorelease();
        return node;
    }
    CC_SAFE_DELETE(node);
    return nullptr;
}

bool uibook::initWithChapter( storyheader_t* story )
{
    if ( !Node::init() )
        return false;
    
    this->setContentSize(Size(RES(512), RES(400)) );
    
    f32 itemHeight = RES(28);
    
    auto bookImage = Sprite::createWithSpriteFrameName(BOOK_IMAGE);
    uihelper::AddCenter(this, bookImage);
    
    auto bookmarkImage = Sprite::createWithSpriteFrameName(BOOKMARK_IMAGE);
    bookmarkImage->setColor(bookmark_colours[story->slot]);
    uihelper::AddTopLeft(bookImage, bookmarkImage, RES(22), RES(22));
    
    auto bookmarkOverlayImage = Sprite::createWithSpriteFrameName(BOOKMARK_OVERLAY_IMAGE);
    bookmarkOverlayImage->setColor(_clrBlack);
    uihelper::AddCenter(bookmarkImage, bookmarkOverlayImage);
    
    auto gradient = LayerGradient::create( Color4B(_clrBlack,0), Color4B(_clrBlack,100) );
    gradient->setContentSize(Size(RES(504), RES(198)));
    uihelper::AddBottomLeft(this, gradient, RES(4), RES(4));
    
    //auto outlineColor = Color4B(_clrBlack); //Color4B(0x3b,0x3d,0xa8,0xff); // 3b3da8
    auto textColor = Color4B(_clrWhite);
    
    auto line = ReplaceAll( (LPCSTR)story->description, "\n", "\n\r");
    
    auto title1 = Label::createWithTTF( uihelper::font_config_medium, line );
    //title1->enableOutline(outlineColor,RES(1));
    title1->getFontAtlas()->setAntiAliasTexParameters();
    title1->setTextColor(textColor);
    //title1->setLineHeight(itemHeight);
    title1->setHorizontalAlignment(TextHAlignment::CENTER);
    
    uihelper::AddBottomCenter(this, title1, RES(0), RES(16));
    
    return true;
}

