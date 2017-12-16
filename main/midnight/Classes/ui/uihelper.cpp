//
//  uihelper.c
//  midnight
//
//  Created by Chris Wild on 05/12/2017.
//
#include "uihelper.h"
#include "uipanel.h"
#include "../frontend/resolutionmanager.h"
#include "../ui/uielement.h"
#include "../ui/uieventargs.h"

Vec2 uihelper::AnchorTopLeft = Vec2(0,1);
Vec2 uihelper::AnchorTopRight = Vec2(1,1);
Vec2 uihelper::AnchorTopCenter = Vec2(0.5,1);

Vec2 uihelper::AnchorCenter = Vec2(0.5,0.5);
Vec2 uihelper::AnchorLeftCenter = Vec2(0,0.5);
Vec2 uihelper::AnchorRightCenter = Vec2(1,0.5);

Vec2 uihelper::AnchorBottomLeft = Vec2(0,0);
Vec2 uihelper::AnchorBottomRight = Vec2(1,0);
Vec2 uihelper::AnchorBottomCenter = Vec2(0.5,0);

TTFConfig uihelper::font_config_big;


void uihelper::Init()
{
    font_config_big.fontFilePath = FONT_FILENAME;
    font_config_big.fontSize = RES(FONT_SIZE_BIG);
    font_config_big.glyphs = GlyphCollection::DYNAMIC;
    font_config_big.outlineSize = 0;
    font_config_big.customGlyphs = nullptr;
    font_config_big.distanceFieldEnabled = false;

}

//
// Parent Positioning
//

void uihelper::PositionParentTopCenter( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition((r.size.width/2)+paddingX, r.size.height-paddingY );
    node->setAnchorPoint(uihelper::AnchorTopCenter);
    
}

void uihelper::PositionParentTopLeft( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(paddingX, r.size.height-paddingY );
    node->setAnchorPoint(uihelper::AnchorTopLeft);
    
}

void uihelper::PositionParentCenterLeft( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(paddingX, (r.size.height/2)-paddingY );
    node->setAnchorPoint(uihelper::AnchorLeftCenter);
}

void uihelper::PositionParentCenterRight( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(r.size.width - paddingX, (r.size.height/2)-paddingY );
    node->setAnchorPoint(uihelper::AnchorRightCenter);
}

void uihelper::PositionParentBottomCenter( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition((r.size.width/2)+paddingX, paddingY );
    
}

void uihelper::PositionParentBottomLeft( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    node->setPosition(paddingX, paddingY );
    node->setAnchorPoint( uihelper::AnchorBottomLeft );
    
}

void uihelper::PositionParentBottomRight( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(r.size.width - paddingX, paddingY );
    node->setAnchorPoint( uihelper::AnchorBottomRight );
}

void uihelper::PositionParentCenter( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition((r.size.width/2)+paddingX, (r.size.height/2)+paddingY );
    node->setAnchorPoint( uihelper::AnchorCenter );
}

Node* uihelper::AddTopCenter( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentTopCenter(node,paddingX,paddingY);
    }
    return node;
}

Node* uihelper::AddTopLeft( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentTopLeft(node,paddingX,paddingY);
    }
    return node;
}

Node* uihelper::AddCenterLeft( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentCenterLeft(node,paddingX,paddingY);
    }
    return node;
}

Node* uihelper::AddCenterRight( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentCenterRight(node,paddingX,paddingY);
    }
    return node;
}

Node* uihelper::AddBottomCenter( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentBottomCenter(node,paddingX,paddingY);
    }
    return node;
}

Node* uihelper::AddBottomLeft( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentBottomLeft(node,paddingX,paddingY);
    }
    return node;
}

Node* uihelper::AddBottomRight( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentBottomRight(node,paddingX,paddingY);
    }
    return node;
}

Node* uihelper::AddCenter( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentCenter(node,paddingX,paddingY);
    }
    return node;
}



//
// Positioning
//

void uihelper::PositionBelow( Node* node, Node* ref, f32 paddingY)
{
    if ( ref == nullptr )
        return;
    
    auto r = ref->getBoundingBox();
    auto p = ref->getPosition();
    
    node->setPosition( p.x, p.y - r.size.height - paddingY );
    node->setAnchorPoint( uihelper::AnchorTopCenter );
}

//
// Creating
//

ui::Button* uihelper::CreateImageButton( const std::string& name )
{
    return ui::Button::create(name,"","", cocos2d::ui::TextureResType::PLIST);
    
}

ui::Button* uihelper::CreateImageButton( const std::string& name, u32 id, const cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback& callback )
{
    auto button = uihelper::CreateImageButton(name);
    button->setTag(id);
    //parent->addChild(button);
    button->addClickEventListener(callback);
    return button;
    
}

void uihelper::FillParent( Node* node )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setContentSize(r.size);
}

ui::Button* uihelper::CreateBoxButton( Size size )
{
    auto button = ui::Button::create(BOX_BACKGROUND_FILENAME);
    button->setTitleFontName(FONT_FILENAME);
    button->setTitleFontSize(RES(FONT_SIZE_BIG));
    button->setTitleColor(Color3B::BLUE);
    button->setTouchEnabled(true);
    button->setScale9Enabled(true);
    button->setContentSize(size);
    // Adjust for centreY without trailing character
    button->getTitleRenderer()->setLineHeight(RES(25));
    return button;
}





