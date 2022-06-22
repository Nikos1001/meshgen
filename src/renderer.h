
#ifndef RENDERER_H
#define RENDERER_H

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>

#include "cglm.h"

typedef struct {
    GLFWwindow* win;
    float fov;
    mat4 proj;
} renderer;

void initRenderer(renderer* r);
void beginFrame(renderer* r);
void endFrame(renderer* r);
bool shouldContinue(renderer* r);

#endif
