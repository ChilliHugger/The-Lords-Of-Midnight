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
#include <unordered_map>

constexpr int HELP_VERSION = 2;


enum HELPFLAGS
{
    hf_none         = 0,
    hf_important    = MXBIT(0),
    hf_always       = MXBIT(1),
};

typedef struct helpitem_t
{
    helpid_t        id;
    int             flags;
    const char*     text;
} helpitem_t ;


class helpmanager : public ringcontroller
{
public:
    helpmanager();
    virtual ~helpmanager();
    
    bool isShown ( helpid_t id );
    bool isImportant( helpid_t id);
    bool isAlways( helpid_t id);
    
    bool Save ( storyid_t id );
    bool Load ( storyid_t id );
    
    void Shown ( helpid_t id );
    
    std::string Get( helpid_t id );

private:
    const helpitem_t* GetItem(helpid_t id);

private:
    std::unordered_map<helpid_t, const helpitem_t*> lookup;
    std::unordered_map<helpid_t, bool> displayed;
};


#endif /* help_h */
