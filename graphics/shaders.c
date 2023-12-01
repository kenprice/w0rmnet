#include "stdlib.h"
#include "shaders.h"

#define GLSL_VERSION 330

Shader shaderOutline;

void init_outline_shader() {
    shaderOutline = LoadShader(NULL, TextFormat("resources/shaders/outline.fs", GLSL_VERSION));

    float outlineSize = 0.5f;
    float outlineColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    float textureSize[2] = { (float)128, (float)128 };

    // Get shader locations
    int outlineSizeLoc = GetShaderLocation(shaderOutline, "outlineSize");
    int outlineColorLoc = GetShaderLocation(shaderOutline, "outlineColor");
    int textureSizeLoc = GetShaderLocation(shaderOutline, "textureSize");

    // Set shader values (they can be changed later)
    SetShaderValue(shaderOutline, outlineSizeLoc, &outlineSize, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shaderOutline, outlineColorLoc, outlineColor, SHADER_UNIFORM_VEC4);
    SetShaderValue(shaderOutline, textureSizeLoc, textureSize, SHADER_UNIFORM_VEC2);
}
