
#ifndef MESH_H
#define MESH_H

#include "dynarr.h"

typedef struct {
    unsigned int VAO;
    unsigned int vertVBO;
    unsigned int uvVBO;
    unsigned int normVBO;
    unsigned int tangVBO;
    unsigned int texVBO;
    unsigned int indexVBO;

    int nVerts;
    int nTris;
    float* verts;
    float* uvs;
    float* norms;
    float* tangs;
    float* texs;
    int* indices;
} mesh;

void initBuildableMesh(mesh* m);
void freeMesh(mesh* m);
void freeBuildableMesh(mesh* m);

int addVert(mesh* m, float x, float y, float z, float u, float v, float nx, float ny, float nz, float tx, float ty, float tz, int tex);
void addTri(mesh* m, int v1, int v2, int v3);

void pushMeshToGPU(mesh* m);

void renderMesh(mesh* m);

#endif
