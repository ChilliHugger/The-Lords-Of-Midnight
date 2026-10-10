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
#include "uioutline.h"

USING_NS_AX;
USING_NS_AX_UI;

using chilli::ui::WidgetClickCallback;

Vec2 uihelper::AnchorTopLeft = Vec2(0,1);
Vec2 uihelper::AnchorTopRight = Vec2(1,1);
Vec2 uihelper::AnchorTopCenter = Vec2(0.5,1);

void uihelper::SetCascadeOpacityRecursive( Node* node )
{
    node->setCascadeOpacityEnabled(true);
    for ( auto child : node->getChildren() ) {
        SetCascadeOpacityRecursive(child);
    }
    if ( auto protectedNode = dynamic_cast<ProtectedNode*>(node) ) {
        for ( auto child : protectedNode->getProtectedChildren() ) {
            SetCascadeOpacityRecursive(child);
        }
    }
}

void uihelper::FadeVisible( Node* node, bool show, bool animate )
{
    const int FADE_TAG = 0x46494C54;
    const f32 FADE_TIME = 0.25f;

    node->stopActionByTag(FADE_TAG);
    SetCascadeOpacityRecursive(node);

    if ( !animate ) {
        node->setOpacity(255);
        node->setVisible(show);
        return;
    }

    if ( show ) {
        // fade in anything not already fully visible
        if ( !node->isVisible() ) {
            node->setVisible(true);
            node->setOpacity(0);
        }
        if ( node->getOpacity() < 255 ) {
            auto fade = FadeIn::create(FADE_TIME);
            fade->setTag(FADE_TAG);
            node->runAction(fade);
        }
    } else if ( node->isVisible() ) {
        auto fade = Sequence::createWithTwoActions(
            FadeOut::create(FADE_TIME),
            CallFunc::create( [node] {
                node->setVisible(false);
                node->setOpacity(255);
            })
        );
        fade->setTag(FADE_TAG);
        node->runAction(fade);
    }
}

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
TTFConfig uihelper::font_config_button;



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
    
    font_config_button.fontFilePath = BUTTON_TEXT_FONT;
    font_config_button.fontSize = RES(BUTTON_TEXT_SIZE);
    font_config_button.glyphs = GlyphCollection::DYNAMIC;
    font_config_button.outlineSize = 0;
    font_config_button.distanceFieldEnabled = false;

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

Button* uihelper::CreateImageButton( const std::string& name, u32 id, const WidgetClickCallback& callback )
{
    auto button = uihelper::CreateImageButton(name);
    button->setTag(id);
    button->addClickEventListener(callback);
    button->setScale(PHONE_SCALE(scale_normal));
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
    // Adjust for centreY without trailing character
    button->getTitleRenderer()->setLineHeight(PHONE_SCALE(RES(25)));
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


void uihelper::addGlow( Button* button, const std::string& image, const Color3B& color, s32 radius )
{
    auto renderer = button->getRendererNormal();
    auto outline = uioutline::create(image, Color4F(color), RES(radius));
    if ( outline != nullptr ) {
        outline->setPosition(renderer->getContentSize()/2);
        renderer->addChild(outline, -1);
    }
}

Label* uihelper::addButtonText( Button* button, const std::string& text, ButtonTextAlign align, f32 offset, s32 outline )
{
    auto renderer = button->getRendererNormal();
    auto size = renderer->getContentSize();

    auto label = Label::createWithTTF( uihelper::font_config_button, text );
    label->getFontAtlas()->setAntiAliasTexParameters();
    label->setTextColor(Color4B(_clrWhite));
    label->enableOutline(Color4B(_clrBlack), RES(outline));
    label->setAdditionalKerning(RES(BUTTON_TEXT_SIZE) * BUTTON_TEXT_TRACKING / 1000.0f);

    switch ( align ) {
        case ButtonTextAlign::Top:
            label->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
            label->setPosition(Vec2(size.width/2, size.height + RES(offset)));
            break;
        case ButtonTextAlign::Centre:
            label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
            label->setPosition(Vec2(size.width/2, size.height/2 + RES(offset)));
            break;
        default:
            label->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
            label->setPosition(Vec2(size.width/2, RES(offset)));
            break;
    }
    renderer->addChild(label);
    return label;
}

// colour slots for <colour:N>, built from the system default colours (uihelper.h)
std::vector<Color3B> uihelper::text_colours = {
    _clrYellow, _clrRed, _clrGreen, _clrBlue, _clrCyan, _clrMagenta, _clrWhite, _clrBlack,
    _clrDarkYellow, _clrDarkRed, _clrDarkGreen, _clrDarkBlue, _clrDarkCyan, _clrDarkMagenta, _clrDarkWhite, _clrGrey
};

void uihelper::setColouredText( Label* label, const std::string& markup, const Color3B& base )
{
    // strip the tags, recording the colour of each letter (by character index)
    static const std::string open_tag = "<colour:";
    static const std::string close_tag = "</colour>";

    std::string text;
    std::vector<Color3B> colours;
    Color3B current = base;

    for ( size_t i = 0; i < markup.size(); ) {
        if ( markup.compare(i, open_tag.size(), open_tag) == 0 ) {
            auto end = markup.find('>', i);
            if ( end != std::string::npos ) {
                auto slot = static_cast<size_t>(atoi(markup.c_str() + i + open_tag.size()));
                current = slot < text_colours.size() ? text_colours[slot] : base;
                i = end + 1;
                continue;
            }
        }
        if ( markup.compare(i, close_tag.size(), close_tag) == 0 ) {
            current = base;
            i += close_tag.size();
            continue;
        }
        char ch = markup[i++];
        text += ch;
        // only count the first byte of each utf8 character
        if ( (static_cast<unsigned char>(ch) & 0xC0) != 0x80 ) {
            colours.push_back(current);
        }
    }

    // the text colour is multiplied by the letter colour, so keep it white and tint each letter
    label->setTextColor(Color4B::WHITE);
    label->setString(text);

    for ( size_t i = 0; i < colours.size(); i++ ) {
        auto letter = label->getLetter(static_cast<int>(i));
        if ( letter != nullptr ) {
            letter->setColor(colours[i]);
        }
    }
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
