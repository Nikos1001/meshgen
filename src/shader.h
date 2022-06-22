
#ifndef SHADER_H
#define SHADER_H

#include "cglm.h"

typedef struct {
    unsigned int program;
} shader;

shader loadShaderFromFile(const char* vertPath, const char* fragPath);
void useShader(shader* s);

void setMat4Uniform(shader* s, const char* name, mat4 m);
void setIntUniform(shader* s, const char* name, int i);
void setVec3Uniform(shader* s, const char* name, vec3 v);

#endif
