//
//  uipopup.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipopup_h
#define uipopup_h

#include "cocos2d.h"
#include "../library/libinc/mxtypes.h"

using namespace chilli::types;

USING_NS_CC;


class uipopup : Node {

private:
    uipopup();

public:
    uipopup( Scene* parent, point pos, f32 width, LPCSTR text );

    void Show();
    void Close();
    
    MXVoidCallback onCancel;
    MXVoidCallback onOk;
    
protected:
    void OnYes();
    void OnNo();
    
protected:
    Scene*  parent;
    
};



#endif /* uipopup_h */
