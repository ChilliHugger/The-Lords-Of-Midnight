//
//  DefaultScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef DefaultScene_hpp
#define DefaultScene_hpp

#include "cocos2d.h"
#include "../library/libinc/mxtypes.h"

using namespace chilli::types;

// forward references
class uipopup;

class DefaultScene : public cocos2d::Scene
{
public:
    
    virtual bool init();
    
    CREATE_FUNC(DefaultScene);
    
protected:
    
    void setBackground( LPCSTR background );
    void fadeExit( f32 delay );
    void AreYouSure ( LPCSTR text, MXVoidCallback ok );
    
    uipopup*    popupWindow;
    
};


#endif /* DefaultScene_hpp */
