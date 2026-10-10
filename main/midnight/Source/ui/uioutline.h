//
//  uioutline.h
//  midnight
//
//  Flat colour outline / glow behind an image, faded out towards its edges
//

#pragma once

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"

FORWARD_REFERENCE(SimpleShader);

class uioutline
{
    using Sprite = ax::Sprite;
    using Color4F = ax::Color4F;

public:
    //
    // Creates a sprite of the named frame (or texture file) that renders as a flat colour
    // silhouette, fading out over 'radius' (in the sprite's own point units) around the image.
    // The sprite is expanded by the radius on every side, so centre it on the real image
    // and order it behind. 'power' shapes the fade (1 = smooth, higher hugs the image).
    // Returns nullptr if the image can't be found.
    //
    static Sprite* create(const std::string& name, const Color4F& colour, f32 radius, f32 power = 1.0f, f32 alpha = 1.0f);

private:
    static SimpleShader* getShader();
};
