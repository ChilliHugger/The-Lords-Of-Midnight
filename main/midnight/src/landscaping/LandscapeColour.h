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
#include "../tme/tme_interface.h"

FORWARD_REFERENCE(LandscapeOptions);
FORWARD_REFERENCE(moonring);

enum class TINT {
    Normal = 0,
    TerrainOutline = 1,
    TerrainFill = 2,
    Tunnel = 3,
    Person = 4,
};

const int MAX_SHADES = 5;
const int MAX_TIME = 32;

class  LandscapeColour
{
    using Color4B = cocos2d::Color4B;
    using Color3B = cocos2d::Color3B;
    using Color4F = cocos2d::Color4F;
    using Node = cocos2d::Node;
public:
    LandscapeColour(LandscapeOptions* options);

    Color4B  CalcCurrentMovementTint ( TINT index );
    f32  CalcCurrentMovementFade ( TINT index );
    
    
    Color4B CreateTimeBrightness ( mxtime_t time );
    void SetShaderColour ( void );
    
    static void OnXmlInit( XmlNode* node );
  
    void SetMovementColour(mxtime_t start, mxtime_t end);
    void SetLookColour(mxtime_t time);

    Color4B Adjust( Color4B source, Color4F tint );
    Color3B GetPersonColour();

    void updateTerrainNode( Node* node );
    void updateCharacterNode( Node* node );
    
public:
    LandscapeOptions*   options;
    moonring*           mr;
    
protected:
    mxtime_t    timeofday;

    Color4B     startTint[MAX_SHADES];
    Color4B     endTint[MAX_SHADES];
};

#endif /* LandscapeColour_hpp */
