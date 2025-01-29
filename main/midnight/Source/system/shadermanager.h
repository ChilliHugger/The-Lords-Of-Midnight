//
//  shadermanager.h
//  midTime
//
//  Created by Chris Wild on 17/04/2020.
//

#pragma once

#include "../axmol_sdk.h"
#include "../library/inc/mxtypes.h"
#include "ringcontroller.h"

FORWARD_REFERENCE(SimpleShader);

class shadermanager : public ringcontroller
{
    using Node = ax::Node;
    using Vec4 = ax::Vec4;
    using ProgramState = ax::backend::ProgramState;
    using Program = ax::backend::Program;
    using Color4F = ax::Color4F;

private:
    void CreateTerrainTimeShader();
    void CreateCharacterTimeShader();
    
public:
    shadermanager();
    virtual ~shadermanager();
    
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
