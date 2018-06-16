//
//  ProjectConfig.hpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#ifndef ProjectConfig_hpp
#define ProjectConfig_hpp

#include "../library/libinc/mxtypes.h"
#include "ringcontroller.h"

using namespace chilli::types;

FORWARD_REFERENCE(progressmonitor);

class projectconfig : public ringcontroller
{
public:
    projectconfig();
    virtual ~projectconfig();
    
    bool LoadXmlConfig ( LPCSTR scenario, progressmonitor* monitor );
    
public:
    
};


#endif /* ProjectConfig_hpp */
