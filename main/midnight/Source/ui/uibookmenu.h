//
//  uibookmenu.h
//  midnight
//
//  Created by Chris Wild on 16/12/2017.
//
//

#pragma once

#include "uielement.h"
#include "../system/storymanager.h"
#include "../ui/uieventargs.h"
#include "../ui/uishortcutkeys.h"

class bookeventargs : public uieventargs
{
public:
    storyid_t   id;
};



class uibookmenu :
    public cocos2d::LayerColor,
    public chilli::ui::NotificationInterface,
    public uishortcutkeys
{
    using Widget = cocos2d::ui::Widget;
public:
    uibookmenu();
    
    static uibookmenu* createWithStory( uipanel* parent, storyinfo_t* stories );
    void Show();
    void Close();
    
protected:
    bool initWithStory( uipanel* parent, storyinfo_t* stories );
    
    uipanel* parent;
};
