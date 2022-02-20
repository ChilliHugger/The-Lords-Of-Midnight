//
//  shadermanager.h
//  midTime
//
//  Created by Chris Wild on 17/04/2020.
//

#pragma once

#include "../cocos.h"
#include "../library/inc/mxtypes.h"
#include "ringcontroller.h"

FORWARD_REFERENCE(SimpleShader);

class shadermanager : public ringcontroller
{
    using Node = cocos2d::Node;
    using Vec4 = cocos2d::Vec4;
    using ProgramState = cocos2d::backend::ProgramState;
    using Program = cocos2d::backend::Program;
    using Color4F = cocos2d::Color4F;

private:
    void CreateTerrainTimeShader();
    void CreateCharacterTimeShader();
    
public:
    
    void Init();
    
    Node* AttachShader(Node* node, SimpleShader* state);

    //
    // Terrain Day and Time shader
    //
    void UpdateTerrainTimeShader(Node* node, f32 alpha, Color4F& outline, Color4F& body);
    Node* AddTerrainTimeShader(Node* node);
    SimpleShader* GetTerrainTimeShader();

    //
    // Character Day and Time shader
    //
    void UpdateCharacterTimeShader(Node* node, f32 alpha, f32 fade, Vec4 colour);
    Node* AddCharacterTimeShader(Node* node);
    SimpleShader* GetCharacterTimeShader();

protected:
    SimpleShader*         terrainTimeShader;
    SimpleShader*         characterTimeShader;
};

#include <stdio.h>
