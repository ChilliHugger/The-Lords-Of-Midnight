//
//  configmanager.h
//  midnight
//
//  Created by Chris Wild on 07/06/2019.
//  Copyright © 2019 Chilli Hugger Software. All rights reserved.
//

#ifndef configmanager_hpp
#define configmanager_hpp

#include "../library/libinc/mxtypes.h"
#include "ringcontroller.h"

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
};

#endif /* configmanager_h */
