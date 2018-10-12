//
//  LandscapeView.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeView_hpp
#define LandscapeView_hpp

#include <stdio.h>

#include "ILandscape.h"



class LandscapeView : public ILandscape
{
public:
    virtual void Init(LandscapeOptions* options);
};


#endif /* LandscapeView_hpp */
