//
//  uipopup.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef uipopup_h
#define uipopup_h

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "uielement.h"
#include "uishortcutkeys.h"

FORWARD_REFERENCE(uipanel);
FORWARD_REFERENCE(uifocuscontroller);

using namespace chilli::types;

class uipopup :
    public chilli::ui::Element,
    public uishortcutkeys
{
protected:
    using Layout = ax::ui::Layout;
    using Scene = ax::Scene;
    
private:
    uipopup();
    ~uipopup();
    
public:
    static uipopup* create( uipanel* parent, point pos, f32 width, LPCSTR text );
    
    void Show();
    void Close();
    
    MXVoidCallback onCancel;
    MXVoidCallback onOk;
    
protected:
    bool initWithParent( uipanel* parent, point pos, f32 width, LPCSTR text );
    void addTouchListener();
    void addKeyboardListener();

    void addFocusController();
    void removeFocusController();

    void OnYes();
    void OnNo();
    
protected:
    uipanel*    parent;
    Layout*     layout;
    uifocuscontroller*  focusController;
};


#endif /* uipopup_h */
