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
class uipopup;

class uipanel : public cocos2d::Scene
{
public:
    
    virtual bool init();
    
    //CREATE_FUNC(uipanel);
    
protected:
    
    void setBackground( LPCSTR background );
    void fadeExit( f32 delay );
    void AreYouSure ( LPCSTR text, MXVoidCallback ok );
    
    uipopup*    popupWindow;
    
};


#endif /* uipanel_h */
