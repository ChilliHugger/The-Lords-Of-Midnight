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

class bookeventargs : public uieventargs
{
public:
    storyid_t   id;
};



class uibookmenu : public cocos2d::LayerColor
                 , public uinotificationinterface
{
public:
	uibookmenu( storyinfo_t* stories );
    
public:

private:
    
};
