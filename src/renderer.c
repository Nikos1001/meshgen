
#include "renderer.h"

#include <stdbool.h>
#include "util.h"

void initRenderer(renderer* r) {
    glewExperimental = true;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    r->win = glfwCreateWindow(1280, 720, "Renderer", NULL, NULL);
    if(r->win == NULL) {
        FATAL_ERROR("Cannot open window.")
        return;
    }

    glfwMakeContextCurrent(r->win);

    if(glewInit() != GLEW_OK) {
        FATAL_ERROR("Cannot initialize GLEW.")
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void beginFrame(renderer* r) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    int w, h;
    glfwGetWindowSize(r->win, &w, &h);
    glm_perspective(3.14 * r->fov / 180.f, (float)w / (float)h, 0.01f, 100.f, r->proj);

}

void endFrame(renderer* r) {
    glfwSwapBuffers(r->win);
    glfwPollEvents();
}

bool shouldContinue(renderer* r) {
    return !glfwWindowShouldClose(r->win);
}
