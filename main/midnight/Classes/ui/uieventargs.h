//
//  uieventargs.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#pragma once

#include "../library/libinc/mxtypes.h"

FORWARD_REFERENCE(uitextmenuitem);

class uieventargs 
{    
public:

private:
    
};


class menueventargs : public uieventargs
{
public:
    uitextmenuitem*     menuitem;
};



