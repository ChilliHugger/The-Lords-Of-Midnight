//
//  CreditsScene.hpp
//  midnight
//
//  Created by Chris Wild on 02/12/2017.
//

#ifndef __PANEL_CREDITS_H_
#define __PANEL_CREDITS_H_

#include "../ui/uipanel.h"

class panel_credits : public uipanel
{
protected:
    using Label = cocos2d::Label;
    using ScrollView = cocos2d::ui::ScrollView;
    
public:
    
    virtual bool init();
    
    CREATE_FUNC(panel_credits);

};



#endif /* __PANEL_CREDITS_H_ */
