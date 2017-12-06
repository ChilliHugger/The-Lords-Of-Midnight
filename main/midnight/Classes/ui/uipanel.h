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
    
    virtual void OnNotification( Ref* element, u32 tag );

    
protected:
    
    void setBackground( LPCSTR background );
    void fadeExit( f32 delay );
    void AreYouSure ( LPCSTR text, MXVoidCallback ok );
    void OpenPDF(LPCSTR pdf);

private:
    uipopup*    popupWindow;
    
};


#endif /* uipanel_h */
