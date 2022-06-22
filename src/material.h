
#ifndef MATERIAL_H
#define MATERIAL_H

#include "mesh.h"
#include "polygon.h"
#include "texture.h"
#include "shader.h"

typedef struct {
    texture color[3];
    texture norm[3];
} materialTextures;

typedef enum {
    MATERIAL_MESH_TYPE_SQUARE_BEVEL
} materialMeshType;

typedef struct {
    materialMeshType meshType;
    float bevelThickness;
    float bevelOffset;
    materialTextures textures;
} material;

void initMaterialTextures(materialTextures* textures, const char* path);
void useMaterialTextures(materialTextures* textures, shader* shader);

void generatePolygonMesh(mesh* m, material* mat, polygon* p);

#endif
