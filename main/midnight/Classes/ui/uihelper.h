//
//  uihelper.h
//  midnight
//
//  Created by Chris Wild on 05/12/2017.
//

#ifndef __UIHELPER_H_INCLUDED__
#define __UIHELPER_H_INCLUDED__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "../library/libinc/mxtypes.h"
#include "uielement.h"

using namespace chilli::types;

FORWARD_REFERENCE( uipanel );

USING_NS_CC;

#define BOX_BACKGROUND_FILENAME "misc/box_16.png"
#define FONT_FILENAME           "fonts/celtic.ttf"
#define FONT_SIZE_BIG           30
#define FONT_SIZE_MEDIUM        20
#define IMAGE_LOGO              "lom_logo"

class uihelper
{
public:
    static void Init();

    // Fonts
    static TTFConfig font_config_big;
    static TTFConfig font_config_medium;
    
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

    static void PositionParentBottomCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentBottomLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentBottomRight( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    static void PositionParentCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0  );
    static void PositionParentCenterLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentCenterRight( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    //
    static Node* AddTopCenter( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddTopLeft( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddCenterLeft( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddCenterRight( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddBottomCenter( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddBottomLeft( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddBottomRight( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    static Node* AddCenter( Node* parent, Node* node, f32 paddingX=0, f32 paddingY=0 );
    
    // Size
    static void FillParent( Node* node );

    // create UI Elements
    static ui::Button* CreateBoxButton( Size size );
    static ui::Button* CreateImageButton( const std::string& name );
    static ui::Button* CreateImageButton( const std::string& name, u32 id,  const cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback& callback  );

    // positioning
    
    static void PositionBelow( Node* node, Node* ref, f32 paddingY = 0 );
    static void PositionRight( Node* node, Node* ref, f32 paddingX = 0);
    
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

//typedef u32                    rgb_t ;
//#define _RGB(r,g,b)         _RGBA(r,g,b,255)
//#define _RGBA(r,g,b,a)      ((r)|(g<<8)|(b<<16)|(a<<24))
#define _RED(rgb)            ((BYTE)(rgb))
#define _GREEN(rgb)            ((BYTE)(((WORD)(rgb)) >> 8))
#define _BLUE(rgb)            ((BYTE)((rgb)>>16))
#define _ALPHA(rgb)            ((BYTE)((rgb)>>24))


#endif /* __UIHELPER_H_INCLUDED__ */
