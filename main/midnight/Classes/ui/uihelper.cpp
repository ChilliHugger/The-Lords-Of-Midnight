//
//  uihelper.c
//  midnight
//
//  Created by Chris Wild on 05/12/2017.
//
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "uihelper.h"
#include "uipanel.h"
#include "../frontend/resolutionmanager.h"

Vec2 uihelper::AnchorTopLeft = Vec2(0,1);
Vec2 uihelper::AnchorTopRight = Vec2(1,1);
Vec2 uihelper::AnchorTopCenter = Vec2(0.5,1);

Vec2 uihelper::AnchorCenter = Vec2(0.5,0.5);
Vec2 uihelper::AnchorLeftCenter = Vec2(0,0.5);
Vec2 uihelper::AnchorRightCenter = Vec2(1,0.5);

Vec2 uihelper::AnchorBottomLeft = Vec2(0,0);
Vec2 uihelper::AnchorBottomRight = Vec2(1,0);
Vec2 uihelper::AnchorBottomCenter = Vec2(0.5,0);


void uihelper::PositionParentTopCenter( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    
    auto r = node->getParent()->getBoundingBox();
    node->setPosition((r.size.width/2)+paddingX, r.size.height-paddingY );
    
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

void uihelper::PositionParentTopLeft( Node* node, f32 paddingX, f32 paddingY )
{
    if ( node->getParent() == nullptr )
        return;
    auto r = node->getParent()->getBoundingBox();
    node->setPosition(paddingX, r.size.height-paddingY );
    node->setAnchorPoint(uihelper::AnchorTopLeft);
    
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

ui::Button* uihelper::CreateImageButton( const std::string& name )
{
    return ui::Button::create(name,"","", cocos2d::ui::TextureResType::PLIST);
    
}

ui::Button* uihelper::CreateImageButton( const std::string& name, u32 id, uipanel* parent )
{
    auto button = uihelper::CreateImageButton(name);
    parent->addChild(button);
    button->addClickEventListener( [parent,id](Ref* s) {
        parent->OnNotification( s, id );
    });
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
    auto button = ui::Button::create("misc/box_16.png");
    button->setTitleFontName("fonts/celtic.ttf");
    button->setTitleFontSize(RES(30));
    button->setTitleColor(Color3B::RED);
    button->setTouchEnabled(true);
    button->setScale9Enabled(true);
    button->setContentSize(size);
    // Adjust for centreY without trailing character
    button->getTitleRenderer()->setLineHeight(RES(25));
    return button;
}

