//
//  uifilterbutton.hp
//  midnight
//
//  Created by Chris Wild on 28/10/2018.
//  Copyright © 2018 Chilli Hugger Software. All rights reserved.
//

#ifndef uifilterbutton_hp
#define uifilterbutton_hp

#include <stdio.h>

#include "../ui/uielement.h"

class uifilterbutton : public cocos2d::ui::Button {
    
    using WidgetEventCallback = chilli::ui::WidgetEventCallback;
    
public:
    CREATE_FUNC( uifilterbutton );

    static uifilterbutton* createWithImage(const std::string& image);
 
    bool isSelected();
    void setSelected(bool selected);
    void addEventListener( WidgetEventCallback callback );
    
protected:
    uifilterbutton();
    virtual bool init() override;
    
    void initChildren();
    void dispatchSelectChangedEvent(bool selected);
    
protected:
    bool selected;
    WidgetEventCallback _checkBoxEventCallback;
};




#endif /* uifilterbutton_h */
