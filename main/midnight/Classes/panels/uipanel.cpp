//
//  DefaultScene.cpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#include "../ui/uipopup.h"
#include "../Extensions/CustomDirector.h"

#include "uipanel.h"
#include "resolutionmanager.h"


USING_NS_CC;


bool uipanel::init()
{
    if ( !Scene::init() )
    {
        return false;
    }
    
    return true;
}

void uipanel::setBackground( LPCSTR background )
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto p = Sprite::create((LPCSTR)background);
    p->setPosition(Vec2(0,0));
    p->setAnchorPoint(Vec2(0,0));
    
    f32 scale = visibleSize.height / p->getContentSize().height ;
    
    p->setScale(scale, scale );
    addChild(p);

}

void uipanel::fadeExit(f32 delay = 0.0f )
{
    if ( delay == 0 ) {
        CustomDirector *director = (CustomDirector *)Director::getInstance();
        director->popSceneWithTransition<TransitionCrossFade>(1.0);
        return;
    }
    
    
    this->scheduleOnce( [](float) {
        
        CustomDirector *director = (CustomDirector *)Director::getInstance();
        director->popSceneWithTransition<TransitionCrossFade>(1.0);
        
    }, delay, "delayed_fade_exit" );
}

void uipanel::AreYouSure ( LPCSTR text, MXVoidCallback ok )
{
    if ( popupWindow != nullptr ) {
        popupWindow->Close();
    }
    
    popupWindow = new uipopup( this, point(0,0), size(RES(600),RES(256+64)), text );
    popupWindow->onCancel = [&] {
        popupWindow = nullptr;
    };
    popupWindow->onOk = [&,ok] {
        popupWindow = nullptr;
        if ( ok != nullptr )
            ok();
    };
    popupWindow->Show();
    
}
