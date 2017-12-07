//
//  DefaultScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipanel_h
#define uipanel_h

#include "cocos2d.h"
#include "../library/libinc/mxtypes.h"

using namespace chilli::types;

// forward references
FORWARD_REFERENCE( uipopup );
FORWARD_REFERENCE( moonring );

class uipanel : public cocos2d::Scene
{
public:
    
    virtual bool init();
    
    //CREATE_FUNC(uipanel);
    
    virtual void OnNotification( Ref* element, u32 tag );

    
protected:
    
    void SetBackground( LPCSTR background );
    void FadeExit( f32 delay = 0);
    void AreYouSure ( LPCSTR text, MXVoidCallback ok );
    void OpenPDF(LPCSTR pdf);
    void FillBackground();
    
protected:
    uipopup*    popupWindow;
    moonring*   mr;
};


#endif /* uipanel_h */
