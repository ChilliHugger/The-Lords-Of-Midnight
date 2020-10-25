//
//  shadermanager.cpp
//  midnight
//
//  Created by Chris Wild on 17/04/2020.
//

#include "shadermanager.h"
#include "../utils/SimpleShader.h"
#include "../ui/uihelper.h"

USING_NS_CC;

void shadermanager::Init()
{
    CreateDayNightShader();
}

template<typename T>
void setUniform(cocos2d::backend::ProgramState* state, std::string uniform, T value)
{
    auto uniformLocation = state->getUniformLocation(uniform);
    state->setUniform(uniformLocation, &value, sizeof(value));
}

void shadermanager::CreateDayNightShader()
{
    dayNightShader = SimpleShader::createWithFragmentShader("terrain/dayNight.fsh");
    dayNightShader->setUniform("p_left", Vec4(0,0,(165.0f/255.0f),alpha_normal));   // Outline
    dayNightShader->setUniform("p_right", Vec4(1,1,1,alpha_normal));                // Body
    dayNightShader->setUniform("p_alpha", alpha_normal);                            // Alpha
}

SimpleShader* shadermanager::GetDayNightShader()
{
    return dayNightShader;
}

void shadermanager::UpdateSpriteDayNightShader(Node* node, f32 alpha, Color4F& outline, Color4F& body)
{
    auto state = node->getProgramState();
    setUniform(state,"p_alpha", alpha);                                             // alpha
    setUniform(state,"p_left", Vec4(outline.r,outline.g,outline.b,outline.a));      // outline
    setUniform(state,"p_right", Vec4(body.r,body.g,body.b,body.a));                 // body
}

Node* shadermanager::AddDayNightShader(Node* node)
{
    return AttachShader(node, dayNightShader);
}

Node* shadermanager::AttachShader(Node* node, SimpleShader* shader)
{
    node->setProgramState(shader->programState->clone());
    return node;
}
