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
    using Vec2 = ax::Vec2;

public:
    //
    // Creates a sprite of the named frame (or texture file) that renders as a flat colour
    // silhouette, fading out over 'radius' (in the sprite's own point units) around the image.
    // The sprite is expanded by the radius on every side, so centre it on the real image
    // and order it behind. 'power' shapes the fade (1 = smooth, higher hugs the image).
    // Returns nullptr if the image can't be found.
    //
    static Sprite* create(const std::string& name, const Color4F& colour, f32 radius, f32 power = 1.0f, f32 alpha = 1.0f);

    //
    // As create, for a frame you already have, e.g. the picture a button is showing. 'flipY' is for a frame
    // whose sprite is drawn flipped (rendered pictures on OpenGL). 'rendered' is for a frame made by rendering
    // (see imagebuilder) rather than loaded from an atlas, which is stored the other way up on Metal.
    //
    static Sprite* createFromFrame(ax::SpriteFrame* frame, const Color4F& colour, f32 radius, f32 power = 1.0f, f32 alpha = 1.0f, bool flipY = false, bool rendered = false);

private:
    static SimpleShader* getShader();
    static Sprite* applyShader(Sprite* sprite, Vec2 uvRadius, const Color4F& colour, f32 power, f32 alpha);
};
