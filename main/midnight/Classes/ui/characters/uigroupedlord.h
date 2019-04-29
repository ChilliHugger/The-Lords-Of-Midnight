//
//  uigrouplord.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "../uielement.h"
#include "uilordselect.h"

class uigroupedlord : public uilordselect
{
public:
    CREATE_FUNC(uigroupedlord);
    virtual bool init() override;
    virtual bool setLord( mxid characterid ) override;
    static uigroupedlord* createWithLord ( mxid characterid );

    void setTitleAlignment(cocos2d::TextHAlignment align);

protected:
    uigroupedlord();

    cocos2d::Sprite* getFaceImage(character& c);
    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
 
private:
    Node* buttonNode;
    cocos2d::Label* title;
    cocos2d::LayerColor* titleBackground;

};
