
#include "shader.h"
#include "util.h"

#include <stdlib.h>

#include <GL/glew.h>

static unsigned int makeShader(int type, const char* src) {

    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        FATAL_ERROR("Could not compile %s shader:\n %s", type == GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog);
        return 0;
    }

    return shader;
}

shader loadShaderFromFile(const char* vertPath, const char* fragPath) {

    const char* vertSrc = readFile(vertPath);
    if(vertSrc == NULL) {
        FATAL_ERROR("Can't open vertex shader file %s", vertPath);
    }
    unsigned int vertShader = makeShader(GL_VERTEX_SHADER, vertSrc);
    free((void*)vertSrc);

    const char* fragSrc = readFile(fragPath);
    if(fragSrc == NULL) {
        exit(-1);
    }
    unsigned int fragShader = makeShader(GL_FRAGMENT_SHADER, fragSrc);
    free((void*)fragSrc);

    shader s;
    s.program = glCreateProgram();
    glAttachShader(s.program, vertShader);
    glAttachShader(s.program, fragShader);
    glLinkProgram(s.program);

    int success;
    glGetProgramiv(s.program, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(s.program, 512, NULL, infoLog);
        FATAL_ERROR("Could not link shader program:\n %s", infoLog);
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return s;
}

void useShader(shader* s) {
    glUseProgram(s->program);
}

static int getUniformLocation(shader* s, const char* name) {
    int loc = glGetUniformLocation(s->program, name);
    if(loc == -1) {
        FATAL_ERROR("Could not find uniform %s.", name);
    }
    return loc;
}

void setMat4Uniform(shader* s, const char* name, mat4 m) {
    int loc = getUniformLocation(s, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, m[0]);
}

void setIntUniform(shader* s, const char* name, int i) {
    int loc = getUniformLocation(s, name);
    glUniform1i(loc, i);
}

void setVec3Uniform(shader* s, const char* name, vec3 v) {
    int loc = getUniformLocation(s, name);
    glUniform3f(loc, v[0], v[1], v[2]);
}
