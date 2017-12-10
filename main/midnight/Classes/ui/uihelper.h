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

class uihelper
{
public:
    static void Init();

    // Fonts
    static TTFConfig font_config_big;
    
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

    // Size
    static void FillParent( Node* node );

    // create UI Elements
    static ui::Button* CreateBoxButton( Size size );
    static ui::Button* CreateImageButton( const std::string& name );
    static ui::Button* CreateImageButton( const std::string& name, u32 id,  const cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback& callback  );

    // positioning
    
    static void PositionBelow( Node* node, Node* ref, f32 paddingY = 0 );
    
    
};


//
// System default colours
//
const Color3B _clrBlack         = Color3B(0x00,0x00,0x00);
const Color3B _clrBlue          = Color3B(0x00,0x00,0xff);
const Color3B _clrRed           = Color3B(0xff,0x00,0x00);
const Color3B _clrMagenta       = Color3B(0xff,0x00,0xff);
const Color3B _clrGreen         = Color3B(0x00,0xff,0x00);
const Color3B _clrCyan          = Color3B(0x00,0xff,0xff);
const Color3B _clrYellow        = Color3B(0xff,0xff,0x00);
const Color3B _clrWhite         = Color3B(0xff,0xff,0xff);

const Color3B _clrDarkBlue      = Color3B(0x00,0x00,0xcd);
const Color3B _clrDarkRed       = Color3B(0xcd,0x00,0x00);
const Color3B _clrDarkMagenta   = Color3B(0xcd,0x00,0xcd);
const Color3B _clrDarkGreen     = Color3B(0x00,0xcd,0x00);
const Color3B _clrDarkCyan      = Color3B(0x00,0xcd,0xcd);
const Color3B _clrDarkYellow    = Color3B(0xcd,0xcd,0x00);
const Color3B _clrDarkWhite     = Color3B(0xcd,0xcd,0xcd);

const Color3B _clrGrey          = Color3B(0x80,0x80,0x80);




#endif /* __UIHELPER_H_INCLUDED__ */
