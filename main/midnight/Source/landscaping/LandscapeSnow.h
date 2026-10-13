#pragma once

#include "../axmol_sdk.h"

// Falling snow overlay made of two particle layers (far: small/slow,
// near: large/fast) to give a sense of depth.
class LandscapeSnow : public ax::Node
{
public:
    static LandscapeSnow* create( const ax::Size& size );

    // Fades the snow in or out. Safe to call every frame with same value
    void setSnowing( bool snowing );
    bool isSnowing() const { return snowing; }

    // Call as the view pans; live flakes are moved immediately, opposite to the pan
    void setLook( float lookAmount );

    void update( float delta ) override;

protected:
    bool initWithSize( const ax::Size& size );
    class SnowLayer* createLayer( int count, float size, float sizeVar,
                                         float speed, float life, float wind );

protected:
    ax::Size                size;
    bool                    snowing{};
    class SnowLayer*        layers[2]{};
    float                   baseRate[2]{};
    float                   baseWind[2]{};
    float                   time{};
    float                   lastLook{};
    bool                    hasLook{};
};
