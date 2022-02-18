//
//  LandscapeTerrain.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeTerrain_hpp
#define LandscapeTerrain_hpp

#include <stdio.h>

#include "LandscapeNode.h"
#include "LandscapeGenerator.h"

USING_NS_CC;

class LandscapeTerrain : public LandscapeNode
{
public:
    
    static LandscapeTerrain* create( LandscapeOptions* options );

    void Build() override;
    void RefreshPositions() override;

protected:
    bool initWithOptions( LandscapeOptions* options );

    Sprite* AddGraphic(Sprite* graphic,f32 x, f32 y, Color4F tint1, Color4F tint2, f32 alpha, f32 scale, LandscapeItem* item, f32 horizontalOffset = 0.0f);
    Sprite* GetTerrainImage( mxterrain_t terrain );
    Sprite* GetImage( std::string& imagename );
    
    void AddDoubleGraphic(
        std::string& name,f32 x, f32 y,
        Color4F tint1, Color4F tint2,
        f32 alpha, f32 scale,
        LandscapeItem* item);
};

class ImageItem : public Ref
{
public:
    ImageItem( LandscapeItem* landscapeItem, f32 horizontalOffset)
    {
        this->landscapeItem = landscapeItem;
        this->horizontalOffset = horizontalOffset;
    }

public:
    LandscapeItem*  landscapeItem;
    f32             horizontalOffset;
};

#endif /* LandscapeTerrain_hpp */
