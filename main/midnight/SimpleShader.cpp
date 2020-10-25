#include "SimpleShader.h"

std::string SimpleShader::defaultVert = R"(
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    attribute vec4 a_color;

    uniform mat4 u_MVPMatrix;

    #ifdef GL_ES
    varying lowp vec4 cc_FragColor;
    varying mediump vec2 cc_FragTexCoord1;
    #else
    varying vec4 cc_FragColor;
    varying vec2 cc_FragTexCoord1;
    #endif

    void main()
    {
        gl_Position = u_MVPMatrix * a_position;
        cc_FragColor = a_color;
        cc_FragTexCoord1 = a_texCoord;
    }
    )";

std::string SimpleShader::fragHead = R"(
    #ifdef GL_ES
    precision lowp float;
    #endif

    varying vec4 cc_FragColor;
    varying vec2 cc_FragTexCoord1;

    uniform sampler2D u_texture;
    uniform float cc_Time;
    )";

SimpleShader::SimpleShader(const std::string& vertSource, const std::string& fragSource)
{
    //create the shader
    program = cocos2d::backend::Device::getInstance()->newProgram(vertSource, fragSource);
    programState = new cocos2d::backend::ProgramState(program);

    currentTextureSlot = 1;

    //add instance to shader manager
    SimpleShaderManager::getInstance()->shaders.push_back(this);
}

SimpleShader::~SimpleShader()
{
    //cleanup
    delete programState;
    delete program;

    auto& vec = SimpleShaderManager::getInstance()->shaders;
    vec.erase(std::remove(vec.begin(), vec.end(), this), vec.end());
}

SimpleShader* SimpleShader::createWithFragmentShader(const std::string& fragShaderPath)
{
    //custom fragment shader 
    auto fragSourceRaw = cocos2d::FileUtils::getInstance()->getStringFromFile(fragShaderPath);

    //build full fragment shader
    auto fragSource = fragHead + fragSourceRaw;

    return new SimpleShader(defaultVert, fragSource);
}

SimpleShader* SimpleShader::createWithVertexAndFragmentShader(const std::string& vertShaderPath, const std::string& fragShaderPath)
{
    //custom vertex shader 
    auto vertSource = cocos2d::FileUtils::getInstance()->getStringFromFile(vertShaderPath);

    //custom fragment shader 
    auto fragSourceRaw = cocos2d::FileUtils::getInstance()->getStringFromFile(fragShaderPath);

    //build full fragment shader
    auto fragSource = fragHead + fragSourceRaw;

    return new SimpleShader(vertSource, fragSource);
}

void SimpleShader::setUniform(std::string uniform, cocos2d::Texture2D* value)
{
    //determine texture slot
    int slot = currentTextureSlot;
    if (textureToSlot.count(uniform) > 0)
    {
        //texture already has a slot
        slot = textureToSlot[uniform];
    }
    else
    {
        //new texture
        textureToSlot[uniform] = slot;
        currentTextureSlot++;
    }

    auto uniformLocation = programState->getUniformLocation(uniform);
    programState->setTexture(uniformLocation, slot, value->getBackendTexture());
}


//Simple Shader Manager
SimpleShaderManager::SimpleShaderManager()
{
    baseTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

SimpleShaderManager* SimpleShaderManager::getInstance()
{
    static auto instance = new SimpleShaderManager();

    return instance;
}

void SimpleShaderManager::updateShaderTime()
{
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    float seconds = (milliseconds - baseTime) / 1000.0f;

    for (auto shader : shaders)
    {
        shader->setUniform("cc_Time", seconds);
    }
}
