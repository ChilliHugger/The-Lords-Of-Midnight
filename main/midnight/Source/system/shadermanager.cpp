//
//  shadermanager.cpp
//  midTime
//
//  Created by Chris Wild on 17/04/2020.
//

#include "shadermanager.h"
#include "../utils/SimpleShader.h"
#include "../ui/uihelper.h"

USING_NS_AX;

shadermanager::shadermanager() :
    terrainTimeShader(nullptr),
    characterTimeShader(nullptr)
{
}

shadermanager::~shadermanager()
{
    SAFEDELETE(terrainTimeShader);
    SAFEDELETE(characterTimeShader);
}

void shadermanager::Init()
{
    CreateCharacterTimeShader();
    CreateTerrainTimeShader();
}

template<typename T>
void setUniform(ax::backend::ProgramState* state, std::string uniform, T value)
{
    auto uniformLocation = state->getUniformLocation(uniform);
    state->setUniform(uniformLocation, &value, sizeof(value));
}

void shadermanager::CreateCharacterTimeShader()
{
    characterTimeShader = SimpleShader::createWithFragmentShader("custom/characterTimeShader_fs");

    characterTimeShader->setUniform("p_colour", Vec4(0,(5.0f/255.0f),(78.0f/255.0f),alpha_normal));
    characterTimeShader->setUniform("p_alpha", alpha_normal);
    characterTimeShader->setUniform("p_fade", 0.25f);
    characterTimeShader->programState->updateBatchId();

}

void shadermanager::UpdateCharacterTimeShader(Node* node, f32 alpha, f32 fade, Vec4 colour)
{
    auto state = node->getProgramState();
    setUniform(state,"p_colour", colour);
    setUniform(state,"p_alpha", alpha);
    setUniform(state,"p_fade", fade);
    state->updateBatchId();
    
}

SimpleShader* shadermanager::GetCharacterTimeShader()
{
    return characterTimeShader;
}

Node* shadermanager::AddCharacterTimeShader(Node* node)
{
    return AttachShader(node, characterTimeShader);
}



void shadermanager::CreateTerrainTimeShader()
{
    terrainTimeShader = SimpleShader::createWithFragmentShader("custom/terrainTimeShader_fs");
    terrainTimeShader->setUniform("p_left", Vec4(0,0,(165.0f/255.0f),alpha_normal));   // Outline
    terrainTimeShader->setUniform("p_right", Vec4(1,1,1,alpha_normal));                // Body
    terrainTimeShader->setUniform("p_alpha", alpha_normal);                            // Alpha
    terrainTimeShader->programState->updateBatchId();
}

void shadermanager::UpdateTerrainTimeShader(Node* node, f32 alpha, Color4F& outline, Color4F& body)
{
    auto state = node->getProgramState();
    setUniform(state,"p_alpha", alpha);                                             // alpha
    setUniform(state,"p_left", Vec4(outline.r,outline.g,outline.b,outline.a));      // outline
    setUniform(state,"p_right", Vec4(body.r,body.g,body.b,body.a));                 // body
    state->updateBatchId();
}

SimpleShader* shadermanager::GetTerrainTimeShader()
{
    return terrainTimeShader;
}

Node* shadermanager::AddTerrainTimeShader(Node* node)
{
    return AttachShader(node, terrainTimeShader);
}

Node* shadermanager::AttachShader(Node* node, SimpleShader* shader)
{
    auto ref = shader->programState->clone();
    node->setProgramState(ref, true);
    return node;
}
