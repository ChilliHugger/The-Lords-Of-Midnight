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

using namespace chilli::types;

FORWARD_REFERENCE( uipanel );

USING_NS_CC;

class uihelper
{
public:
    static Vec2 AnchorTopLeft;
    static Vec2 AnchorTopRight;
    static Vec2 AnchorTopCenter;
    static Vec2 AnchorCenter;
    static Vec2 AnchorLeftCenter;
    static Vec2 AnchorRightCenter;
    static Vec2 AnchorBottomLeft;
    static Vec2 AnchorBottomRight;
    static Vec2 AnchorBottomCenter;
    
    static void PositionParentTopCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentTopLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    static void PositionParentBottomCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentBottomLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentBottomRight( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    static void PositionParentCenter( Node* node, f32 paddingX = 0, f32 paddingY = 0  );
    static void PositionParentCenterLeft( Node* node, f32 paddingX = 0, f32 paddingY = 0 );
    static void PositionParentCenterRight( Node* node, f32 paddingX = 0, f32 paddingY = 0 );

    static void FillParent( Node* node );

    static ui::Button* CreateBoxButton( Size size );
    static ui::Button* CreateImageButton( const std::string& name );
    static ui::Button* CreateImageButton( const std::string& name, u32 id, uipanel* parent );

    
};



#endif /* __UIHELPER_H_INCLUDED__ */
