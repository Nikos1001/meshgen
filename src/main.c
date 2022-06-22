
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "mesh.h"
#include "renderer.h"
#include "texture.h"
#include "polygon.h"
#include "material.h"
#include "framebuffer.h"

int main() {

    renderer r;
    r.fov = 80.0f;
    initRenderer(&r);

    shader s = loadShaderFromFile("../res/shader/shad.vert", "../res/shader/shad.frag");
    shader screen = loadShaderFromFile("../res/shader/screen.vert", "../res/shader/screen.frag");

    useShader(&s);
    mat4 view;
    glm_mat4_identity(view);
    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3){0.0f, 0.0f, -5.0f});
    
    setMat4Uniform(&s, "uView", view);
    setMat4Uniform(&s, "uModel", model);

    polygon p;
    initBuildablePolygon(&p);

    int v = addChainToPolygon(&p, (vec2s){0.75f, 0.0f});
    int first = v;
    for(float a = 0.1f; a <= 6.28f; a += 0.05f) {
        v = addPointToPolygon(&p, (vec2s){0.75 * cosf(a), -0.5f * sinf(a)}, v);
    }
    closeChain(&p, first, v);

    v = addChainToPolygon(&p, (vec2s){0.25f, 0.0f});
    first = v;
    for(float a = 0.1f; a <= 6.28f; a += 0.05f) {
        v = addPointToPolygon(&p, (vec2s){0.25 * cosf(a), 0.25f * sinf(a)}, v);
    }
    closeChain(&p, first, v);


    material wood;
    wood.meshType = MATERIAL_MESH_TYPE_SQUARE_BEVEL;
    wood.bevelThickness = 0.02f;
    wood.bevelOffset = 0.02f;
    initMaterialTextures(&wood.textures, "../res/texture/materials/wood");


    mesh m;
    initBuildableMesh(&m);

    generatePolygonMesh(&m, &wood, &p);

    pushMeshToGPU(&m);
    freeBuildablePolygon(&p);
    freeBuildableMesh(&m);

    vec3 camPos = (vec3){0.0f, 0.0f, 0.0f};
    float yaw = 0.0f;
    float pitch = 0;

    glfwSetInputMode(r.win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); 

    framebuffer fb;
    initFramebuffer(&fb, 1280 * 2, 720 * 2);
    addColorAttachment(&fb, false);
    addColorAttachment(&fb, true);
    addColorAttachment(&fb, true);
    completeFramebuffer(&fb);

    mesh screenMesh;
    initBuildableMesh(&screenMesh);
    int topLeft     = addVert(&screenMesh, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    int topRight    = addVert(&screenMesh,  1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    int bottomLeft  = addVert(&screenMesh, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    int bottomRight = addVert(&screenMesh,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    addTri(&screenMesh, topLeft, bottomLeft, topRight);
    addTri(&screenMesh, topRight, bottomLeft, bottomRight);
    pushMeshToGPU(&screenMesh);
    freeBuildableMesh(&screenMesh);

    useShader(&screen);
    setIntUniform(&screen, "uColor", 0);
    setIntUniform(&screen, "uNorm", 1);
    setIntUniform(&screen, "uPos", 2);

    while(shouldContinue(&r)) {
        beginFrame(&r);

        renderToFramebuffer(&fb);
        glViewport(0, 0, 1280 * 2, 720 * 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        useShader(&s);

        setMat4Uniform(&s, "uProj", r.proj);

        glm_mat4_identity(view);
        glm_euler((vec3){pitch, yaw, 0}, view);

        vec4 fwd = (vec4){0, 0, 1, 0};
        glm_mat4_mulv(view, fwd, fwd);

        glm_translate(view, camPos);
        setMat4Uniform(&s, "uView", view);
        useMaterialTextures(&wood.textures, &s);
        renderMesh(&m);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 2 * 1280, 2 * 720);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        useShader(&screen);
        setVec3Uniform(&screen, "uCamPos", camPos);
        useFramebuffer(&fb);
        renderMesh(&screenMesh);

        double mx, my;
        glfwGetCursorPos(r.win, &mx, &my);
        yaw += 0.0008f * (mx - 1280.0f / 2.0f);
        pitch += 0.0008f * (my - 720.0f / 2.0f);
        if(pitch > 3.14f / 2.0f)
            pitch = 3.14f / 2.0f;
        if(pitch < -3.14f / 2.0f)
            pitch = -3.14f / 2.0f;
        glfwSetCursorPos(r.win, 1280 / 2, 720 / 2);

        fwd[1] *= 0;
        fwd[0] *= -1;
        glm_vec3_normalize(fwd);
        glm_vec3_scale(fwd, 0.1f, fwd);

        if(glfwGetKey(r.win, GLFW_KEY_W)) {
            glm_vec3_add(camPos, fwd, camPos);
        }
        if(glfwGetKey(r.win, GLFW_KEY_S)) {
            glm_vec3_sub(camPos, fwd, camPos);
        }
        if(glfwGetKey(r.win, GLFW_KEY_SPACE)) {
            camPos[1] -= 0.1f;
        }
        if(glfwGetKey(r.win, GLFW_KEY_RIGHT_SHIFT)) {
            camPos[1] += 0.1f;
        }

        endFrame(&r);
    }

    freeMesh(&m);
    freeMesh(&screenMesh);
    glfwTerminate();
}
