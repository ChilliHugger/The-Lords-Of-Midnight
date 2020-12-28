//
//  uihelper.h
//  midnight
//
//  Created by Chris Wild on 05/12/2017.
//

#ifndef __UIHELPER_H_INCLUDED__
#define __UIHELPER_H_INCLUDED__

#include "../library/inc/mxtypes.h"
#include "uielement.h"

using namespace chilli::types;

FORWARD_REFERENCE( uipanel );

constexpr auto BOX_BACKGROUND_FILENAME = "misc/box_16.png";
constexpr auto FONT_FILENAME           = "fonts/Midnight.ttf";
constexpr auto FONT_SIZE_SHORTCUT      = 25;
constexpr auto FONT_SIZE_BIG           = 30;
constexpr auto FONT_SIZE_MEDIUM        = 20;
constexpr auto FONT_SIZE_SMALL         = 12;
constexpr auto FONT_SIZE_DEBUG         = 8;
constexpr auto IMAGE_LOGO              = "misc/logo.png";


using cocos2d::Color3B;

class uihelper
{
    using TTFConfig = cocos2d::TTFConfig;
    using Node = cocos2d::Node;
    using Button = cocos2d::ui::Button;
    using Size = cocos2d::Size;
    using Vec2 = cocos2d::Vec2;
    using WidgetClickCallback = chilli::ui::WidgetClickCallback;

public:
    static void initialiseFonts();

    // Fonts
    static TTFConfig font_config_big;
    static TTFConfig font_config_medium;
    static TTFConfig font_config_small;
    static TTFConfig font_config_shortcut;
    static TTFConfig font_config_debug;
    
    // Anchors
    static Vec2 AnchorTopLeft;
    static Vec2 AnchorTopRight;
    static Vec2 AnchorTopCenter;
    static Vec2 AnchorCenter;
    static Vec2 AnchorLeftCenter;
    static Vec2 AnchorRightCenter;
    static Vec2 AnchorBottomLeft;
    static Vec2 AnchorBottomRight;
    static Vec2 AnchorBottomCenter;
    
    // Parent Node Positioning
    static void PositionParentTopCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentTopLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentTopRight( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    static void PositionParentBottomCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentBottomLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentBottomRight( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    static void PositionParentCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0  );
    static void PositionParentCenterLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentCenterRight( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    static void PositionCenterAnchor (Node* node, Vec2 anchor, f32 paddingX=0, f32 paddingY=0 );

    //
    static Node* AddTopCenter( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddTopLeft( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddTopRight( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddCenterLeft( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddCenterRight( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddBottomCenter( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddBottomLeft( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddBottomRight( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddCenter( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    
    // Size
    static void FillParent( Node* node );

    // create UI Elements
    static Button* CreateBoxButton( Size size );
    static Button* CreateImageButton( const std::string& name );
    static Button* CreateImageButton( const std::string& name, u32 id, const WidgetClickCallback& callback  );

    // layers
    static Node* createVerticalGradient( Color3B& color, f32 height, f32 gradientHeight, f32 width, s32 dir );
    static Node* createHorizontalGradient( Color3B& color, f32 width, f32 gradientWidth, f32 height, s32 dir );

    static Node* addDebugNode(Node* node);
    
    // positioning
    
    static void PositionBelow( Node* node, Node* ref, f32 paddingY = 0 );
    static void PositionRight( Node* node, Node* ref, f32 paddingX = 0);
    
    //
    static Button* setEnabled( Button* button, bool enabled );

    static Node* getChildByTagRecursively(const int nodeTag, Node* parent) ;

    template <typename T>
    static T getChildByTagRecursively(const int nodeTag, Node* parent)
        { return static_cast<T>(getChildByTagRecursively(nodeTag,parent)); }


};

typedef Color3B rgb_t ;


//
// System default colours
//
const rgb_t _clrBlack         = Color3B(0x00,0x00,0x00);
const rgb_t _clrBlue          = Color3B(0x00,0x00,0xff);
const rgb_t _clrRed           = Color3B(0xff,0x00,0x00);
const rgb_t _clrMagenta       = Color3B(0xff,0x00,0xff);
const rgb_t _clrGreen         = Color3B(0x00,0xff,0x00);
const rgb_t _clrCyan          = Color3B(0x00,0xff,0xff);
const rgb_t _clrYellow        = Color3B(0xff,0xff,0x00);
const rgb_t _clrWhite         = Color3B(0xff,0xff,0xff);

const rgb_t _clrDarkBlue      = Color3B(0x00,0x00,0xcd);
const rgb_t _clrDarkRed       = Color3B(0xcd,0x00,0x00);
const rgb_t _clrDarkMagenta   = Color3B(0xcd,0x00,0xcd);
const rgb_t _clrDarkGreen     = Color3B(0x00,0xcd,0x00);
const rgb_t _clrDarkCyan      = Color3B(0x00,0xcd,0xcd);
const rgb_t _clrDarkYellow    = Color3B(0xcd,0xcd,0x00);
const rgb_t _clrDarkWhite     = Color3B(0xcd,0xcd,0xcd);

const rgb_t _clrGrey          = Color3B(0x80,0x80,0x80);

#ifdef WIN32
#undef _ALPHA
#endif // WIN32


#define _RED(rgb)            ((BYTE)(rgb))
#define _GREEN(rgb)            ((BYTE)(((WORD)(rgb)) >> 8))
#define _BLUE(rgb)            ((BYTE)((rgb)>>16))
#define _ALPHA(rgb)            ((BYTE)((rgb)>>24))


#endif /* __UIHELPER_H_INCLUDED__ */
