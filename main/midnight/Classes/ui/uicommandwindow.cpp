//
//  uihelpwindow.cpp
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//
//

#include "uihelper.h"
#include "uicommandwindow.h"
#include "uipanel.h"
#include "../frontend/language.h"
#include "resolutionmanager.h"
#include "../system/moonring.h"

#include "../frontend/layout_id.h"
#include "../frontend/choose_id.h"


USING_NS_CC;
using namespace cocos2d::ui;

uicommandwindow::uicommandwindow( uipanel* parent )
{
    auto rect = parent->getBoundingBox();
    
    grid=size(RES(128),RES(80));
    padding = size( RES(32), RES(16) );
    
    this->parent = parent;
    this->id = id;
    
    this->setContentSize( rect.size );
    this->setPosition( Vec2::ZERO );
    
    auto rectNode = DrawNode::create();
    rectNode->drawSolidRect(Vec2(0,0), Vec2( rect.getMaxX(), rect.getMaxY()), Color4F(0,0,0,0.75f) );
    this->addChild(rectNode);
    
    //
    //f32 layout_padding = RES(32);
    f32 width = (5*grid.cx) + (2*padding.cx);
    f32 maxHeight = (4*grid.cy) + (2*padding.cy);
    //f32 innerWidth = width-(2*layout_padding);
    
    // layout
    //auto
    layout = Layout::create();
    layout->setBackGroundImage(BOX_BACKGROUND_FILENAME);
    layout->setBackGroundImageScale9Enabled(true);
    layout->setOpacity(ALPHA(0.75));
    uihelper::AddCenter( this,layout ) ;

//    auto scrollView = ui::ScrollView::create();
//    scrollView->setDirection(ui::ScrollView::Direction::VERTICAL);
//    layout->addChild(scrollView);
//
//    // add text
//    auto label = Label::createWithTTF(text, FONT_FILENAME, FONT_SIZE_BIG);
//    label->setColor(_clrDarkRed);
//    label->setAlignment(TextHAlignment::LEFT);
//    label->setLineHeight(RES(FONT_SIZE_BIG));
//    label->setLineSpacing(0);
//    label->setMaxLineWidth(innerWidth);
//    label->enableWrap(true);
//    scrollView->addChild(label);
//
//    // calc label height
//    auto textHeight = MIN(label->getContentSize().height,maxHeight) ;
//    auto boxheight = textHeight + (2*layout_padding) ;
//
    layout->setContentSize(Size(width,maxHeight));
//    uihelper::PositionParentTopLeft( label, 0, layout_padding ) ;
//
//    scrollView->setInnerContainerSize(Size(innerWidth,label->getContentSize().height));
//
//    // allow some extra padding for the scrollview
//    // so that the scrollbar is in the right padding area
//    scrollView->setContentSize(Size(innerWidth+layout_padding,textHeight));
//
//    bool scrollingEnabled = label->getContentSize().height != textHeight ;
//
//    scrollView->setBounceEnabled( scrollingEnabled );
//    scrollView->setScrollBarEnabled( scrollingEnabled );
//
//    uihelper::PositionParentTopLeft(scrollView,layout_padding,layout_padding);
//    uihelper::PositionParentTopLeft(label);
    

    // Close Button in top left corner
//    auto close = uihelper::CreateImageButton("close", 0, [&](Ref* ref)
//                                             {
//                                                 OnClose();
//                                             });
//    uihelper::AddTopLeft(layout,close, RES(8), RES(8) );
//    close->setAnchorPoint(uihelper::AnchorCenter);
    
    this->setLocalZOrder(ZORDER_NEAR);
    
    initialiseCommands();
}

void uicommandwindow::initialiseCommands()
{
    cocos2d::ui::AbstractCheckButton::ccWidgetClickCallback callback = [&] (Ref* ref ) {
        this->OnClose();
        this->parent->OnNotification(ref);
    };
    
    // THINK
    auto think = uihelper::CreateImageButton("i_think", ID_THINK, callback);
    addItem(think, CHOOSE_THINK);
    //think->setTitleText("He thinks again...");
    //think->getTitleLabel()->setPosition(think->get().x, think->getPosition().y);
    //think->getTitleLabel()->setAnchorPoint(uihelper::AnchorCenter);
    
#if defined(_LOM_)
    // SEEK
    auto seek = uihelper::CreateImageButton("i_seek", ID_SEEK, callback);
    addItem(seek,CHOOSE_SEEK);

    auto hide = uihelper::CreateImageButton("i_hide", ID_HIDE, callback);
    addItem(hide,CHOOSE_HIDE);

    auto unhide = uihelper::CreateImageButton("i_unhide", ID_HIDE, callback);
    addItem(unhide,CHOOSE_HIDE);
    
    // FIGHT
    auto fight = uihelper::CreateImageButton("i_fight", ID_FIGHT, callback);
    addItem(fight,CHOOSE_FIGHT);
#endif
    
#if defined(_DDR_)
    auto give = uihelper::CreateImageButton("i_give", ID_GIVE, callback);
    addItem(give,CHOOSE_GIVE);
    // TODO: Add text to give button
    
    auto take = uihelper::CreateImageButton("i_take", ID_TAKE, callback);
    addItem(take,CHOOSE_TAKE);
    
    auto use = uihelper::CreateImageButton("i_use", ID_USE, callback);
    addItem(use,CHOOSE_USE);
    
    auto rest = uihelper::CreateImageButton("i_rest", ID_REST, callback);
    addItem(rest,CHOOSE_REST);
    
    auto tunnel = uihelper::CreateImageButton("i_entertunnel", ID_TUNNEL, callback);
    addItem(tunnel, CHOOSE_TUNNEL);
    
#endif
    
    // MAP
    auto map = uihelper::CreateImageButton("i_map", ID_MAP, callback);
    addItem(map,CHOOSE_MAP);
    
    // NIGHT 9
    auto night = uihelper::CreateImageButton("i_night", ID_NIGHT, callback);
    addItem(night,CHOOSE_NIGHT);
    
    
    // APPROACH
    auto approach = uihelper::CreateImageButton("i_approach", ID_APPROACH, callback);
    addItem(approach,CHOOSE_APPROACH);
    
    // RECRUIT SOLDIERS
    auto recruitMen = uihelper::CreateImageButton("i_recruit", ID_RECRUITMEN, callback);
    addItem(recruitMen,CHOOSE_RECRUIT);
    
    // POST SOLDIERS
    auto postMen = uihelper::CreateImageButton("i_post", ID_POSTMEN, callback);
    addItem(postMen,CHOOSE_GUARD);
    
    // 7
    auto attack = uihelper::CreateImageButton("i_battle", ID_ATTACK, callback);
    addItem(attack,CHOOSE_BATTLE);
    
//    if ( SELECT_MODE == 2 ) {
//        int count=0;
//        for ( u32 ii=0; ii<i_char.Count(); ii++ ) {
//            uielement* i = i_char[ii];
//            if ( characterId != i->tag ) {
//                character c;
//                TME_GetCharacter(c, i->tag);
//                if ( Character_IsAlive(c) && Character_IsControllable(i->tag) ) {
//                    actions->addItem(i,CHOOSE_CHAR+count);
//                    count++;
//                }
//            }
//        }
//    }
    
    
    auto undoDawn = uihelper::CreateImageButton("undo_dawn", ID_UNDO_DAWN, callback);
    addItem(undoDawn,CHOOSE_UNDO_DAWN);
    
    auto undoLast = uihelper::CreateImageButton("undo", ID_UNDO, callback);
    addItem(undoLast,CHOOSE_UNDO_LAST);
    
    auto home = uihelper::CreateImageButton("i_home", ID_HOME, callback);
    addItem(home,CHOOSE_EXIT);
    
}



void uicommandwindow::show( MXVoidCallback callback )
{
    closeCallback = callback ;
    
    // stop the underlying parent
    // from getting any of the events
    parent->getEventDispatcher()->pauseEventListenersForTarget(parent,true);
    
    // now add an event listener
    // for the using cancelling
    // by touching off the main window
    auto listener1 = EventListenerTouchOneByOne::create();
    listener1->onTouchBegan = [&](Touch* touch, Event* event){
        // eat the touch in the message area
        if ( layout->getBoundingBox().containsPoint(touch->getLocation()) )
            return true;
        OnClose();
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener1, this);
    
    // and show
    parent->addChild(this);
}

void uicommandwindow::OnClose()
{    
    close();
    if ( closeCallback!=nullptr )
        closeCallback();
}

void uicommandwindow::close()
{
    //
    parent->removeChild(this);
    
    // give the parent events back
    parent->getEventDispatcher()->resumeEventListenersForTarget(parent,true);
    
}

void uicommandwindow::clearItems()
{
}

Button* uicommandwindow::findItemById( layoutid_t id )
{
    for(auto const& button: elements)
     {
        if ( button->getTag() == id ) {
            return button;
        }
    }
    return nullptr;
}

void uicommandwindow::enableItem( layoutid_t id, bool enable )
{
    auto button = findItemById(id);
    if ( button != nullptr ) {
        button->setEnabled(enable);
        button->setOpacity( enable ? ALPHA(1.0) : ALPHA(0.25) );
    }
}

void uicommandwindow::showItem( layoutid_t id, bool show )
{
    auto button = findItemById(id);
    if ( button != nullptr ) {
        button->setVisible(show);
    }
}

void uicommandwindow::addItem( Button* element, u32 index )
{
    auto width = (int)layout->getContentSize().width;
    
    int iy = index/(width/grid.cx);
    int ix = index%(width/grid.cx);
    
    
    Vec2 pos(padding.cx+(ix*grid.cx)+(grid.cx/2),padding.cy+(iy*grid.cy)+(grid.cy/2) );
    
    // invert the y
    pos.y = layout->getContentSize().height - pos.y;
    
    
    element->setPosition(pos);
    element->setAnchorPoint(uihelper::AnchorCenter);
    layout->addChild(element);
    
    //element->setUserData((void*)index);
    //element->setActionTag(index);
    elements.pushBack(element);
    
    //element->location = pos;
    //element->EnableCenter();
    //AddChild(element);
    
    //elements++;
    
}

