//
//  uigrouplord.h
//  midnight
//
//  Created by Chris Wild on 29/10/2018.
//
//

#pragma once
#include "../uielement.h"
#include "uilordselect.h"

class uigroupedlord : public uilordselect
{
public:
    CREATE_FUNC(uigroupedlord);
    virtual bool init() override;
    virtual bool setLord( mxid characterid ) override;
    static uigroupedlord* createWithLord ( mxid characterid );

    void setTitleAlignment(TextHAlignment align);

protected:
    uigroupedlord();

    Sprite* getFaceImage(character& c);
    virtual void onPressStateChangedToNormal() override;
    virtual void onPressStateChangedToPressed() override;
    virtual bool hitTest(const Vec2 &pt, const Camera* camera, Vec3 *p) const override;

private:
    Node* buttonNode;
    Label* title;
    LayerColor* titleBackground;

};
