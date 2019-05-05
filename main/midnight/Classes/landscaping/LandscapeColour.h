//
//  LandscapeColour.hpp
//  midnight
//
//  Created by Chris Wild on 14/06/2018.
//

#ifndef LandscapeColour_hpp
#define LandscapeColour_hpp

#include "../cocos.h"
#include <stdio.h>
#include "../tme_interface.h"

FORWARD_REFERENCE(LandscapeOptions);

class  LandscapeColour
{
    using Color4B = cocos2d::Color4B;
    using Color3B = cocos2d::Color3B;
    using Color4F = cocos2d::Color4F;
    using Node = cocos2d::Node;
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
