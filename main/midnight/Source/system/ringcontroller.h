//
//  ringcontroller.hpp
//  midnight
//
//  Created by Chris Wild on 13/06/2018.
//

#ifndef ringcontroller_hpp
#define ringcontroller_hpp

#include "../library/inc/mxtypes.h"

FORWARD_REFERENCE( moonring );


class ringcontroller
{
public:
    virtual ~ringcontroller() {};
    void InjectMoonRing( moonring* mr) { this->mr = mr; }
    
    
protected:
    moonring* mr;
};

#endif /* ringcontroller_hpp */
