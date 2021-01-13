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

#define LANDSCAPE_SKY_HEIGHT    (55*LANDSCAPE_GSCALE)
#define LANDSCAPE_FLOOR_HEIGHT  (80*LANDSCAPE_GSCALE)

class LandscapeView : public ILandscape
{
private:
    LandscapeView();
    
public:
    static LandscapeView* create( LandscapeOptions* options );

protected:
    bool initWithOptions( LandscapeOptions* options );

};

#endif /* LandscapeView_hpp */
