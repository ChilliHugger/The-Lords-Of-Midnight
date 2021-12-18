//
//  uieventargs.h
//  midnight
//
//  Created by Chris Wild on 09/12/2017.
//
//

#pragma once

struct uitextmenuitem;

class uieventargs 
{    
public:
    virtual ~uieventargs() {}

private:
    
};


class menueventargs : public uieventargs
{
public:
    uitextmenuitem*     menuitem;
};
