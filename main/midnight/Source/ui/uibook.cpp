//
//  uibook.cpp
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//
#include "uihelper.h"
#include "uibook.h"

#include "../system/moonring.h"
#include "../system/resolutionmanager.h"
#include "../utils/imagebuilder.h"

USING_NS_AX;

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

uibook* uibook::createWithChapter( storyheader_t* story )
{
    uibook* node = new (std::nothrow) uibook();
    if (node && node->initWithChapter(story))
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}

bool uibook::initWithChapter( storyheader_t* story )
{
    if ( !Node::init() )
        return false;
    
    this->setContentSize(Size(RES(512), RES(400)) );
    
    // the book, its bookmark and the fade for the title are one picture (book.xml); the bookmark is tinted for the slot
    auto builder = moonring::mikesingleton()->imagebuilder;
    if ( auto def = builder->find("story_book") ) {
        auto book = *def;
        if ( auto e = book.find("bookmark") ) {
            auto colour = bookmark_colours[story->slot % NUMELE(bookmark_colours)];
            e->colour = { colour.r, colour.g, colour.b };
        }
        builder->setupPicture(this, book);
    }
    
    auto textColor = Color4B(_clrWhite);
    auto line = chilli::lib::StringExtensions::replaceAll( story->description, "\n", "\n\r");
    auto title1 = Label::createWithTTF( uihelper::font_config_medium, line );
    title1->getFontAtlas()->setAntiAliasTexParameters();
    title1->setTextColor(textColor);
    title1->setHorizontalAlignment(TextHAlignment::CENTER);
    
    uihelper::AddBottomCenter(this, title1, RES(0), RES(16));
    
    return true;
}

