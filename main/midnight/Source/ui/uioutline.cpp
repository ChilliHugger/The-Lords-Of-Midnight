//
//  uioutline.cpp
//  midnight
//

#include "uioutline.h"
#include "../utils/SimpleShader.h"

USING_NS_AX;

SimpleShader* uioutline::getShader()
{
    static SimpleShader* shader = nullptr;
    if (shader == nullptr) {
        shader = SimpleShader::createWithFragmentShader("custom/outlineShader_fs");
        shader->setUniform("p_colour", Vec4(1,1,1,1));
        shader->setUniform("p_uvRect", Vec4(0,0,1,1));
        shader->setUniform("p_radius", Vec2(0,0));
        shader->setUniform("p_alpha", 1.0f);
        shader->setUniform("p_power", 1.0f);
        shader->programState->updateBatchId();
    }
    return shader;
}

template<typename T>
static void setUniform(backend::ProgramState* state, const std::string& uniform, T value)
{
    auto uniformLocation = state->getUniformLocation(uniform);
    state->setUniform(uniformLocation, &value, sizeof(value));
}

Sprite* uioutline::create(const std::string& name, const Color4F& colour, f32 radius, f32 power, f32 alpha)
{
    Sprite* sprite = nullptr;
    Vec4 uvRect(0,0,1,1);   // bounds of the image within the sprite's texture
    Vec2 uvRadius;

    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
    if (frame != nullptr) {
        //
        // Atlas frames are packed as polygons, so the frame's rectangle can contain
        // pieces of neighbouring images. Draw the frame through its own polygon into a
        // clean texture, padded by the radius, and outline that instead.
        //
        auto originalSize = frame->getOriginalSize();
        auto size = Size(originalSize.x + (radius * 2), originalSize.y + (radius * 2));

        auto image = Sprite::createWithSpriteFrame(frame);
        image->setPosition(size.width / 2, size.height / 2);

        auto renderTexture = RenderTexture::create((int)ceilf(size.width), (int)ceilf(size.height), backend::PixelFormat::RGBA8);
        renderTexture->beginWithClear(0, 0, 0, 0);
        image->visit();
        renderTexture->end();

        auto texture = renderTexture->getSprite()->getTexture();
        sprite = Sprite::createWithTexture(texture);
        sprite->setFlippedY(true);  // render textures are upside down

        auto textureSize = texture->getContentSize();
        uvRadius = Vec2(radius / textureSize.width, radius / textureSize.height);
    } else {
        //
        // Standalone texture, padded by the radius
        //
        auto texture = Director::getInstance()->getTextureCache()->addImage(name);
        if (texture == nullptr) {
            return nullptr;
        }

        auto texW = (f32)texture->getPixelsWide();
        auto texH = (f32)texture->getPixelsHigh();
        auto contentSize = texture->getContentSize();
        auto pixelsPerPoint = contentSize.width > 0 ? texW / contentSize.width : 1.0f;

        auto size = Size(contentSize.width + (radius * 2), contentSize.height + (radius * 2));
        sprite = Sprite::createWithTexture(texture);
        sprite->setTextureRect(Rect(-radius, -radius, size.width, size.height), false, Vec2(size.width, size.height));
        uvRadius = Vec2((radius * pixelsPerPoint) / texW, (radius * pixelsPerPoint) / texH);
    }

    sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    sprite->setProgramState(getShader()->programState->clone(), true);
    auto state = sprite->getProgramState();
    setUniform(state, "p_colour", Vec4(colour.r, colour.g, colour.b, colour.a));
    setUniform(state, "p_uvRect", uvRect);
    setUniform(state, "p_radius", uvRadius);
    setUniform(state, "p_alpha", alpha);
    setUniform(state, "p_power", power);
    state->updateBatchId();
    return sprite;
}
