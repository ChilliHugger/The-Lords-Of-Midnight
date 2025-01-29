//
//  uicompass.cpp
//  revenge
//
//  Created by Chris Wild on 02/04/2022.
//

#include "uicompass.h"
#include "uihelper.h"
#include "../frontend/language.h"

#include "../system/moonring.h"
#include "../system/resolutionmanager.h"

USING_NS_AX;

static LPCSTR compass_items[] = {
    DIRECTION_N,
    DIRECTION_NE,
    DIRECTION_E,
    DIRECTION_SE,
    DIRECTION_S,
    DIRECTION_SW,
    DIRECTION_W,
    DIRECTION_NW };

constexpr s32 GROUP_RADIUS = 96;
constexpr f32 GROUP_STEPS = 8;
constexpr int ItemSize = 64;
constexpr f32 SelectedItemScale = scale_normal + scale_1qtr;

#define NormalItemColour    Color4F(0.0f,0.0f,0.0f,0.25f)
#define CurrentItemColour   Color4F(1.0f,1.0f,0.0f,0.75f)
#define SelectedItemColour  Color4F(0.0f,0.0f,1.0f,0.75f)

uicompass* uicompass::create( uipanel* parent )
{
    uicompass* node = new (std::nothrow) uicompass();
    if (node && node->initWithParent(parent) )
    {
        node->autorelease();
        return node;
    }
    AX_SAFE_DELETE(node);
    return nullptr;
}

bool uicompass::initWithParent( uipanel* parent )
{
    if(!Node::init()) {
        return false;
    }
    
    this->parent = parent;
    parent->retain();
    
    auto size = Size(RES(256),RES(256));
    
    current = 0;
    selected = 0;
    
    setContentSize(size);
    
    int index = 0;
    for( auto d : compass_items ) {
    
        auto drawNode = DrawNode::create();
        drawNode->setTag(index+1);
        drawNode->setContentSize(Size(RES(ItemSize),RES(ItemSize)));
        drawNode->setPosition(calcCirclePos(index-2));
        addChild(drawNode);
        
        
        auto title = Label::createWithTTF( uihelper::font_config_big, compass_items[index] );
        title->setTextColor(Color4B(_clrWhite));
        title->enableOutline(Color4B(_clrBlack),RES(4));
        title->getFontAtlas()->setAntiAliasTexParameters();
        title->setHorizontalAlignment(TextHAlignment::CENTER);
        title->setVerticalAlignment(TextVAlignment::CENTER);
        title->setAnchorPoint(uihelper::AnchorCenter);
        drawNode->addChild(title);
        title->setPosition(RES(ItemSize),RES(ItemSize));

#if defined(ANIMATE_COMPASS)
        drawNode->setPosition(Vec2(RES(64),RES(64)));
        auto moveAction = MoveTo::create(0.5,  calcCirclePos(index));
        drawNode->stopAllActions();
        drawNode->runAction( EaseBounceOut::create(moveAction) );
#endif
            
        setMode(drawNode);
        index += 1;
    }
    
    return true;
}

void uicompass::addTouchListener(EventListenerTouchOneByOne* listener)
{
    EventListenerTouchOneByOne* localListener = nullptr;
    
    if(listener == nullptr) {
        localListener = EventListenerTouchOneByOne::create();
    }else{
        localListener = listener->clone();
    }

    localListener->onTouchBegan = [=](Touch* touch, Event* event){
        return true;
    };
  
    localListener->onTouchMoved = [=, this](Touch* touch, Event* event){
        auto pos = touch->getLocationInView() ;
        auto focus = getItem(touch->getLocationInView());
        clearCurrentFocus();
        if(focus!=nullptr) {
            current = focus->getTag();
            setMode(focus);
        }
    };
    
    // trigger when you let up
    localListener->onTouchEnded = [=, this](Touch* touch, Event* event){
        clearSelectedFocus();
        selected = current;
        clearCurrentFocus();
        
        if(selected!=0) {
            if(onOk) {
                onOk();
            }
        }else{
            if(onCancel) {
                onCancel();
            }
        }
        Close();
    };
    
    // Add listener
    _eventDispatcher->addEventListenerWithSceneGraphPriority(localListener, this);
}

void uicompass::Show(EventListenerTouchOneByOne* listener)
{
    // stop the underlying parent
    // from getting any of the events
    parent->pauseEvents();

    addTouchListener(listener);
    
    //uishortcutkeys::addKeyboardListener(this);
    
    // and show
    setLocalZOrder(ZORDER_POPUP);
    parent->addChild(this);
}

void uicompass::Close()
{
    // give the parent events back
    parent->resumeEvents();

    //
    parent->removeChild(this);
    
}

void uicompass::setSelected(mxdir_t direction)
{
    clearSelectedFocus();
    selected = direction == DR_NONE
        ? 0
        : direction + 1;
        
    auto item = getChildByTag<DrawNode*>(selected);
    if(item!=nullptr) {
        setMode(item);
    }
}
 
mxdir_t uicompass::getSelected()
{
    return (selected == 0)
        ? (mxdir_t) DR_NONE
        : (mxdir_t) (selected-1);
}

void uicompass::clearCurrentFocus()
{
    auto item = getChildByTag<DrawNode*>(current);
    current = 0;
    if(item!=nullptr) {
        setMode(item);
    }
}

void uicompass::clearSelectedFocus()
{
    auto item = getChildByTag<DrawNode*>(selected);
    selected = 0;
    if(item!=nullptr) {
        setMode(item);
    }
}

Vec2 uicompass::calcCirclePos ( f32 pos )
{
    f32 step = 360 / GROUP_STEPS ;
    f32 a = step*pos;
    f32 r = RES(GROUP_RADIUS);
    
    auto size = getContentSize();
    auto center = Vec2( HALF(size.width), HALF(size.height));
    f32 x = center.x + r * cos(DEGREETORAD(a));
    f32 y = center.y - r * sin(DEGREETORAD(a));
    
    return Vec2(x-RES(ItemSize),y-RES(ItemSize));
}

void uicompass::setMode(DrawNode* node)
{
    auto scale = scale_normal;
    auto index = node->getTag();
    Color4F colour = NormalItemColour;

    if(index==current) {
        colour = CurrentItemColour;
        scale = SelectedItemScale;
    }else if (index == selected) {
        colour = SelectedItemColour;
    }
    
    node->clear();
    node->drawSolidCircle(Vec2(RES(ItemSize),RES(ItemSize)), RES(ItemSize/2)*scale , 0, 64, 1.0f, 1.0f, colour);
}

DrawNode* uicompass::getItem( Vec2 pos )
{
    for( auto node : getChildren()) {
        auto drawNode = dynamic_cast<DrawNode*>(node);
        CONTINUE_IF_NULL(drawNode);
    
        if(getDistanceFromCentre(pos, drawNode) < RES(ItemSize/2))
            return drawNode;
    }
    return nullptr;
}


f32 uicompass::getDistanceFromCentre( Vec2 pos, Node* node ) const
{
    auto nodePos = convertToWindowSpace(node->getPosition());
    auto center = nodePos + Vec2(RES(ItemSize),-RES(ItemSize));

    // calc the distance from the center
    f32 dx = ABS(center.x - pos.x);
    f32 dy = ABS(center.y - pos.y);
    f32 distance = hypot(dx,dy);
    return distance;
}
