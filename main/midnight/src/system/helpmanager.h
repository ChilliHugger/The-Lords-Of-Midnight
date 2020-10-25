//
//  help.h
//  midnight
//
//  Created by Chris Wild on 11/12/2017.
//

#ifndef help_h
#define help_h

#include "../library/inc/mxtypes.h"
#include "ringcontroller.h"

#include "storymanager.h"
#include "../frontend/help_id.h"


enum HELPFLAGS
{
    hf_none         = 0,
    hf_important    = MXBIT(0),
    hf_always       = MXBIT(1),
};

typedef struct helpitem_t
{
    int     flags;
    const char*   text;
    
} helpitem_t ;




class helpmanager : public ringcontroller
{
public:
    helpmanager();
    virtual ~helpmanager();
    
    BOOL isShown ( helpid_t id );
    BOOL isImportant( helpid_t id);
    BOOL isAlways( helpid_t id);
    
    BOOL Save ( storyid_t id );
    BOOL Load ( storyid_t id );
    
    void Shown ( helpid_t id );
    
    LPCSTR Get( helpid_t id );
    
public:
    BOOL displayed[ HELP_MAX ];
};


#endif /* help_h */
