//
//  uihelper.c
//  midnight
//
//  Created by Chris Wild on 05/12/2017.
//
#include "uihelper.h"
#include "uipanel.h"
#include "../system/resolutionmanager.h"
#include "../ui/uielement.h"
#include "../ui/uieventargs.h"
#include "../system/moonring.h"

USING_NS_AX;
USING_NS_AX_UI;

using chilli::ui::WidgetClickCallback;

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
TTFConfig uihelper::font_config_medium;
TTFConfig uihelper::font_config_small;
TTFConfig uihelper::font_config_shortcut;
TTFConfig uihelper::font_config_debug;



void uihelper::initialiseFonts()
{
    f32 scale = scale_normal;
    if(PHONE_SCALE(scale_normal)!=scale_normal) {
        scale = scale_normal+scale_half;
    }

    font_config_big.fontFilePath = FONT_FILENAME;
    font_config_big.fontSize = RES(FONT_SIZE_BIG)*scale;
    font_config_big.glyphs = GlyphCollection::DYNAMIC;
    font_config_big.outlineSize = 0;
    font_config_big.distanceFieldEnabled = false;

    font_config_medium.fontFilePath = FONT_FILENAME;
    font_config_medium.fontSize = RES(FONT_SIZE_MEDIUM)*scale;
    font_config_medium.glyphs = GlyphCollection::DYNAMIC;
    font_config_medium.outlineSize = 0;
    font_config_medium.distanceFieldEnabled = false;

    font_config_small.fontFilePath = FONT_FILENAME;
    font_config_small.fontSize = RES(FONT_SIZE_SMALL);
    font_config_small.glyphs = GlyphCollection::DYNAMIC;
    font_config_small.outlineSize = 0;
    font_config_small.distanceFieldEnabled = false;

    font_config_shortcut.fontFilePath = FONT_FILENAME;
    font_config_shortcut.fontSize = RES(FONT_SIZE_SHORTCUT);
    font_config_shortcut.glyphs = GlyphCollection::DYNAMIC;
    font_config_shortcut.outlineSize = 0;
    font_config_shortcut.distanceFieldEnabled = false;
    
    font_config_debug.fontFilePath = "fonts/arial.ttf";
    font_config_debug.fontSize = RES(16)*scale;
    font_config_debug.glyphs = GlyphCollection::DYNAMIC;
    font_config_debug.outlineSize = 0;
    font_config_debug.distanceFieldEnabled = false;
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
    node->setIgnoreAnchorPointForPosition(false);
}

void uihelper::PositionParentTopLeft( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(paddingX, r.size.height-paddingY );
    node->setAnchorPoint(uihelper::AnchorTopLeft);
    node->setIgnoreAnchorPointForPosition(false);
}

void uihelper::PositionParentTopRight( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(r.size.width - paddingX, r.size.height-paddingY );
    node->setAnchorPoint( uihelper::AnchorTopRight );
    node->setIgnoreAnchorPointForPosition(false);
}


void uihelper::PositionParentCenterLeft( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(paddingX, (r.size.height/2)-paddingY );
    node->setAnchorPoint(uihelper::AnchorLeftCenter);
    node->setIgnoreAnchorPointForPosition(false);
}

void uihelper::PositionParentCenterRight( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(r.size.width - paddingX, (r.size.height/2)-paddingY );
    node->setAnchorPoint(uihelper::AnchorRightCenter);
    node->setIgnoreAnchorPointForPosition(false);
}

void uihelper::PositionParentBottomCenter( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition((r.size.width/2)+paddingX, paddingY );
    node->setAnchorPoint( uihelper::AnchorBottomCenter );
    node->setIgnoreAnchorPointForPosition(false);
}

void uihelper::PositionParentBottomLeft( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    node->setPosition(paddingX, paddingY );
    node->setAnchorPoint( uihelper::AnchorBottomLeft );
    node->setIgnoreAnchorPointForPosition(false);
    
}

void uihelper::PositionParentBottomRight( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(r.size.width - paddingX, paddingY );
    node->setAnchorPoint( uihelper::AnchorBottomRight );
    node->setIgnoreAnchorPointForPosition(false);
}

void uihelper::PositionParentCenter( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition((r.size.width/2)+paddingX, (r.size.height/2)+paddingY );
    node->setAnchorPoint( uihelper::AnchorCenter );
    node->setIgnoreAnchorPointForPosition(false);
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

Node* uihelper::AddTopRight( Node* parent, Node* node, f32 paddingX, f32 paddingY )
{
    if ( node != nullptr ) {
        parent->addChild(node);
        uihelper::PositionParentTopRight(node,paddingX,paddingY);
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

void uihelper::PositionRight( Node* node, Node* ref, f32 paddingX)
{
    if ( ref == nullptr )
        return;
    
    auto r = ref->getBoundingBox();
    auto p = ref->getPosition();
   
    node->setPosition( p.x + r.size.width + paddingX, p.y);
    node->setAnchorPoint( ref->getAnchorPoint() );
}

void uihelper::PositionCenterAnchor (Node* node, Vec2 anchor, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition((r.size.width*anchor.x)+paddingX, (r.size.height*anchor.y)-paddingY );
    node->setAnchorPoint(uihelper::AnchorCenter);
    node->setIgnoreAnchorPointForPosition(false);
}

//
// Creating
//

Button* uihelper::CreateImageButton( const std::string& name )
{
ax::ui::Button* button;
    if ( Director::getInstance()->getTextureCache()->getTextureForKey(name) != nullptr ) {
        button=ax::ui::Button::create(name,"","", ax::ui::Widget::TextureResType::LOCAL);
    }else{
        button=ax::ui::Button::create(name,"","", ax::ui::Widget::TextureResType::PLIST);
    }
    button->setLocalZOrder(ZORDER_UI);
    return uihelper::setEnabled(button,true);
}

Button* uihelper::CreateImageButton( const std::string& name, u32 id, const WidgetClickCallback& callback, f32 scale )
{
    auto defaultScale = PHONE_SCALE(scale);
    auto button = uihelper::CreateImageButton(name);
    button->setTag(id);
    button->addClickEventListener(callback);
    button->setScale(defaultScale);
    
    
    button->onFocusChanged = [button, defaultScale](Widget* widgetLostFocus, Widget* widgetGetFocus) {
        
        if (widgetGetFocus == button) {
            button->setScale(defaultScale * 1.2f);
            button->setFocused(true);
            button->setColor(_clrGreen);
        }
        if (widgetLostFocus == button) {
            button->setScale(defaultScale);
            button->setFocused(false);
            button->setColor(_clrWhite);
        }
    };
    
    
    return button;
    
}

void uihelper::FillParent( Node* node )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setContentSize(r.size);
}

Button* uihelper::CreateBoxButton( Size size )
{
    size.width = PHONE_SCALE(size.width);
    size.height = PHONE_SCALE(size.height);
    
    auto button = ax::ui::Button::create(BOX_BACKGROUND_FILENAME);
    button->setTitleFontName(FONT_FILENAME);
    button->setTitleFontSize(PHONE_SCALE(RES(FONT_SIZE_BIG)));
    button->setTitleColor(Color3B::BLUE);
    button->setTouchEnabled(true);
    button->setScale9Enabled(true);
    button->setContentSize(size);
    button->setLocalZOrder(ZORDER_UI);
    uihelper::setEnabled(button,true);
#if !defined(_OS_TVOS_)
    // Adjust for centreY without trailing character
    button->getTitleRenderer()->setLineHeight(PHONE_SCALE(RES(25)));
#endif
 
    button->onFocusChanged = [button](Widget* widgetLostFocus, Widget* widgetGetFocus) {
        if (widgetGetFocus == button) {
            button->setScale(1.2f);
            button->setFocused(true);
            button->setTitleColor(Color3B::RED);
            button->setColor(_clrGreen);
        }
        if (widgetLostFocus == button) {
            button->setScale(1.0f);
            button->setFocused(false);
            button->setTitleColor(Color3B::BLUE);
            button->setColor(_clrWhite);
        }
    };
 
    return button;
}

Button* uihelper::setEnabled( ax::ui::Button* button, bool enabled )
{
    if ( button != nullptr ) {
        button->setEnabled(enabled);
        button->setOpacity( enabled ? ALPHA(1.0f) : ALPHA(0.25f) );
    }
    return button;
}

/**
* Recursively searches for a child node
* @param nodeTag: the tag of the node searched for.
* @param parent: the initial parent node where the search should begin.
*/
Node* uihelper::getChildByTagRecursively(const int nodeTag, ax::Node* parent) {
    auto node = parent->getChildByTag(nodeTag);
    if (node==nullptr) {
        for (auto child : parent->getChildren())
        {
            node = getChildByTagRecursively(nodeTag, child);
            if (node!=nullptr) break;
        }
    }
    return node;
}


Node* uihelper::createVerticalGradient( Color3B& color, f32 height, f32 gradientHeight, f32 width, s32 dir ) {
    // top gradient
    auto node = Node::create();
    node->setContentSize(Size(width,height));
    
    auto gradient = LayerGradient::create( Color4B(color,ALPHA(1.0f)), Color4B(color,ALPHA(0.0f)), Vec2(0,dir) );
    gradient->setContentSize(Size(width,gradientHeight));
    
    auto filled = LayerColor::create(Color4B(color), width, height-gradientHeight);
    
    if ( dir == 1 ) {
        uihelper::AddTopLeft(node, gradient);
        uihelper::AddBottomLeft(node, filled);
    }else{
        uihelper::AddTopLeft(node, filled);
        uihelper::AddBottomLeft(node, gradient);
    }
    
    return node;
}

Node* uihelper::createHorizontalGradient( Color3B& color, f32 width, f32 gradientWidth, f32 height, s32 dir ) {
    // top gradient
    auto node = Node::create();
    node->setContentSize(Size(width,height));
    
    auto gradient = LayerGradient::create( Color4B(color,ALPHA(1.0f)), Color4B(color,ALPHA(0.0f)), Vec2(dir,0) );
    gradient->setContentSize(Size(gradientWidth,height));
    
    auto filled = LayerColor::create(Color4B(color), width-gradientWidth, height);
    
    if ( dir == 1 ) {
        uihelper::AddBottomRight(node, gradient);
        uihelper::AddBottomLeft(node, filled);
    }else{
        uihelper::AddBottomRight(node, filled);
        uihelper::AddBottomLeft(node, gradient);
    }
    
    return node;
}

Node* uihelper::addDebugNode(Node* node)
{
    auto background = LayerColor::create(Color4B(0,255,0,ALPHA(0.50f)));
    uihelper::AddBottomLeft(node, background);
    uihelper::FillParent(background);
    return background;
}

layoutid_t uihelper::getIdFromSender(Ref* ref)
{
    // check for escape being pressed
    // and don't pass it on so that we can close
    auto button = static_cast<Widget*>(ref);
    if ( button == nullptr )
        return ID_NONE;
    
    return static_cast<layoutid_t>(button->getTag());
}
