
#include "mesh.h"

#include <GL/glew.h>

static void initMesh(mesh* m) {
    glGenVertexArrays(1, &m->VAO);
    glBindVertexArray(m->VAO);

    glGenBuffers(1, &m->vertVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m->uvVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m->uvVBO);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m->normVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m->normVBO);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &m->tangVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m->tangVBO);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &m->texVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m->texVBO);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &m->indexVBO);
}

void initBuildableMesh(mesh* m) {
    initMesh(m);
    m->nVerts = 0;
    m->nTris = 0;
    m->verts = dynarr(float);
    m->uvs = dynarr(float);
    m->norms = dynarr(float);
    m->tangs = dynarr(float);
    m->texs = dynarr(float);
    m->indices = dynarr(int);
}

void freeMesh(mesh* m) {
    glDeleteBuffers(1, &m->vertVBO);
    glDeleteBuffers(1, &m->uvVBO);
    glDeleteBuffers(1, &m->indexVBO);
    glDeleteBuffers(1, &m->normVBO);
    glDeleteBuffers(1, &m->texVBO);
    glDeleteVertexArrays(1, &m->VAO);
    m->nTris = 0;
    m->verts = 0;
}

void freeBuildableMesh(mesh* m) {
    dynarrFree(&m->verts);
    dynarrFree(&m->uvs);
    dynarrFree(&m->norms);
    dynarrFree(&m->texs);
    dynarrFree(&m->indices);
}

int addVert(mesh* m, float x, float y, float z, float u, float v, float nx, float ny, float nz, float tx, float ty, float tz, int tex) {
    dynarrPush(&m->verts, x);
    dynarrPush(&m->verts, y);
    dynarrPush(&m->verts, z);
    dynarrPush(&m->uvs, u);
    dynarrPush(&m->uvs, v);
    dynarrPush(&m->norms, nx);
    dynarrPush(&m->norms, ny);
    dynarrPush(&m->norms, nz);
    dynarrPush(&m->tangs, tx);
    dynarrPush(&m->tangs, ty);
    dynarrPush(&m->tangs, tz);
    dynarrPush(&m->texs, tex);
    m->nVerts++;
    return m->nVerts - 1;
}

void addTri(mesh* m, int v1, int v2, int v3) {
    dynarrPush(&m->indices, v1);
    dynarrPush(&m->indices, v2);
    dynarrPush(&m->indices, v3);
    m->nTris++;
}

void pushMeshToGPU(mesh* m) {
    glBindVertexArray(m->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m->vertVBO);
    glBufferData(GL_ARRAY_BUFFER, m->nVerts * 3 * sizeof(float), m->verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, m->uvVBO);
    glBufferData(GL_ARRAY_BUFFER, m->nVerts * 2 * sizeof(float), m->uvs, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, m->normVBO);
    glBufferData(GL_ARRAY_BUFFER, m->nVerts * 3 * sizeof(float), m->norms, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, m->tangVBO);
    glBufferData(GL_ARRAY_BUFFER, m->nVerts * 3 * sizeof(float), m->tangs, GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, m->texVBO);
    glBufferData(GL_ARRAY_BUFFER, m->nVerts * sizeof(float), m->texs, GL_STATIC_DRAW);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->nTris * 3 * sizeof(int), m->indices, GL_STATIC_DRAW);
}

void renderMesh(mesh* m) {
    glBindVertexArray(m->VAO);
    glDrawElements(GL_TRIANGLES, m->nTris * 3, GL_UNSIGNED_INT, (void*)0);
}
