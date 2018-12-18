//
//  LandscapeColour.hpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#ifndef LandscapeColour_hpp
#define LandscapeColour_hpp

#include "cocos2d.h"
#include <stdio.h>
#include "../tme_interface.h"

USING_NS_CC;

FORWARD_REFERENCE(LandscapeOptions);

class  LandscapeColour
{
public:
    Color4B  CalcCurrentMovementTint ( u32 index );
    Color4B CreateTimeBrightness ( mxtime_t time );
    void SetTimeOfDay ( mxtime_t time );
    void SetShaderColour ( void );
    
    static void OnXmlInit( chilli::xml::node* node );
  
    void SetMovementColour(mxtime_t start, mxtime_t end);
    void SetLookColour(mxtime_t time);
    
    Color4B Adjust( Color4B source, Color4F tint );
    Color3B GetPersonColour();
    
    void updateNode( Node* node );

public:
    LandscapeOptions*   options;
    
    mxtime_t    timeofday;

    Color4B     startTint[4];
    Color4B     endTint[4];
};



#endif /* LandscapeColour_hpp */
