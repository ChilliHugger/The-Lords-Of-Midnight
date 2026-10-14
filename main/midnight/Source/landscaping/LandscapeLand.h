//
//  LandscapeLand.hpp
//  citadel
//
//  Created by Chris Wild on 22/08/2017.
//
//

#ifndef LandscapeLand_hpp
#define LandscapeLand_hpp

#include "LandscapeNode.h"
#include "LandscapeGenerator.h"
#include "../system/moonring.h"

FORWARD_REFERENCE(SimpleShader);

class LandscapeLand : public LandscapeNode
{
    using Sprite = ax::Sprite;
    using DrawNode = ax::DrawNode;
    using Vec2 = ax::Vec2;
public:
    static LandscapeLand* create( LandscapeOptions* options );

    void Build() override;
    void RefreshPositions() override;

protected:
    bool initWithOptions( LandscapeOptions* options );

    bool CalcQuadCorners( LandscapeItem* item, Vec2* corners );
    bool QuadIsValid( Vec2* corners );
    void DrawWater();

    SimpleShader* floorShader;
};


class FloorTile : public ax::Node
{
    using V3F_C4B_T2F = ax::V3F_C4B_T2F;
    using Tex2F       = ax::Tex2F;
    using Color4B     = ax::Color4B;

public:
    static FloorTile* create(const ax::Color4F& colour)
    {
        auto tile = new (std::nothrow) FloorTile();
        if (tile && tile->init(colour))
        {
            tile->autorelease();
            return tile;
        }
        AX_SAFE_DELETE(tile);
        return nullptr;
    }

    void setCorners(const ax::Vec2* corners)
    {
        // corners: [0]=left-far, [1]=right-far, [2]=right-near, [3]=left-near
        for (int ii = 0; ii < 4; ii++)
            _verts[ii].vertices = ax::Vec3(corners[ii].x, corners[ii].y, 0);
    }

void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override
{
    _triangles.verts      = _verts;
    _triangles.vertCount  = 4;
    _triangles.indices    = _indices;
    _triangles.indexCount = 6;

    const auto& matrixP = ax::Director::getInstance()->getMatrix(ax::MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    ax::Mat4 matrixMVP  = matrixP * transform;
    auto mvpLocation    = _programState->getUniformLocation("u_MVPMatrix");
    _programState->setUniform(mvpLocation, matrixMVP.m, sizeof(matrixMVP.m));

    _cmd.init(
        0,
        _texture,
        ax::BlendFunc::ALPHA_PREMULTIPLIED,
        _triangles,
        transform,
        flags
    );
    renderer->addCommand(&_cmd);
}

protected:
    // A flat-colour quad: a 1x1 white texture tinted by the vertex colour,
    // drawn with the engine's POSITION_TEXTURE_COLOR program.
    bool init(const ax::Color4F& colour)
    {
        if (!Node::init())
            return false;

        _texture = whiteTexture();
        if (_texture == nullptr)
            return false;

        Color4B c(colour);

        for (int ii = 0; ii < 4; ii++)
            _verts[ii] = { ax::Vec3(0,0,0), c, Tex2F(0.5f, 0.5f) };

        auto program = ax::ProgramManager::getInstance()->getBuiltinProgram(
            ax::ProgramType::POSITION_TEXTURE_COLOR
        );
        _programState = new ax::backend::ProgramState(program);

        // bind the texture to the program state — the POSITION_TEXTURE_COLOR
        // shader's sampler is named u_tex0 (see positionTextureColor.frag;
        // SpriteBatchNode/ParticleSystemQuad look it up the same way). Using
        // the wrong name here leaves the sampler unbound, so it silently
        // reads whatever texture unit 0 last held — showing up as solid
        // black or a stray texture instead of the tile.
        auto textureLocation = _programState->getUniformLocation("u_tex0");
        _programState->setTexture(textureLocation, 0, _texture->getBackendTexture());

        _cmd.getPipelineDescriptor().programState = _programState;


        return true;
    }

private:
    static ax::Texture2D* whiteTexture()
    {
        static const char* key = "water_flat_white";
        auto cache = ax::Director::getInstance()->getTextureCache();
        auto texture = cache->getTextureForKey(key);
        if (texture == nullptr) {
            static const uint8_t white[4] = { 255, 255, 255, 255 };
            ax::Image* image = new (std::nothrow) ax::Image();
            if (image && image->initWithRawData(white, sizeof(white), 1, 1, 8))
                texture = cache->addImage(image, key);
            AX_SAFE_RELEASE(image);
        }
        return texture;
    }

    ax::Texture2D*                  _texture  = nullptr;
    ax::backend::ProgramState*      _programState = nullptr;
    ax::TrianglesCommand            _cmd;
    ax::TrianglesCommand::Triangles _triangles;
    V3F_C4B_T2F                     _verts[4];
    unsigned short                  _indices[6] = { 0, 1, 2, 0, 2, 3 };
};


#endif /* LandscapeLand_hpp */

/*
    
    // Pano = 5062
    // points = 0 - 1080 - 2160
    if (!QuadIsValid(corners)) {
        //if (item->id == 162) {
             UIDEBUG("BEFORE ID: %d, loc(x=%.2f, y=%.2f), h=%.2f, p=%.2f, mx=%d, [0]=%.4f,%.4f, [1]=%.4f,%.4f, [2]=%.4f,%.4f, [3]=%.4f,%.4ff",
                item->id, gx, gy, h, p,
                x,
                corners[0].x, corners[0].y,
                corners[1].x, corners[1].y,
                corners[2].x, corners[2].y,
                corners[3].x, corners[3].y
            );
        //}
 */
