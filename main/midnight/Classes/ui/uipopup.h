//
//  uipopup.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipopup_h
#define uipopup_h

#include "../cocos.h"
#include "../library/libinc/mxtypes.h"
#include "uielement.h"
#include "uishortcutkeys.h"

using namespace chilli::types;

class uipopup :
    public chilli::ui::Element,
    public uishortcutkeys
{
protected:
    using Layout = cocos2d::ui::Layout;
    using Scene = cocos2d::Scene;
    
private:
    uipopup();
    ~uipopup();
    
public:
    static uipopup* create( Scene* parent, point pos, f32 width, LPCSTR text );
    
    void Show();
    void Close();
    
    MXVoidCallback onCancel;
    MXVoidCallback onOk;
    
protected:
    bool initWithParent( Scene* parent, point pos, f32 width, LPCSTR text );
    void addTouchListener();
    void addKeyboardListener();

    void OnYes();
    void OnNo();
    
protected:
    Scene*  parent;
    Layout* layout;
    
};



#endif /* uipopup_h */
