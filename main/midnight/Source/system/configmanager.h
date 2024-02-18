//
//  configmanager.h
//  midnight
//
//  Created by Chris Wild on 07/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef configmanager_hpp
#define configmanager_hpp

#include <stdio.h>

#include "../library/inc/mxtypes.h"
#include "ringcontroller.h"

enum UNDOHISTORY {
    UH_NOTSET = -1,
    UH_NORMAL = 1,
    UH_EASY = 10,
    UH_MEDIUM = 5,
    UH_HARD = 0
};

class configmanager : public ringcontroller
{
public:
    configmanager();
    virtual ~configmanager();
    
    bool LoadXmlConfig ( const std::string& scenario );
    
public:
    bool skip_dedication;
    bool skip_adverts;
    bool debug_map;
    bool keep_full_save_history;
    bool always_undo;
    u32 undo_history;
    
    std::string start_on_panel;
};

#endif /* configmanager_h */
