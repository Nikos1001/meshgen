
#include "material.h"
#include "geoutil.h"

static texture loadTexFromPath(const char* path, const char* name) {
    char pathbuf[256];
    sprintf(pathbuf, "%s/%s", path, name);
    return loadTexture(pathbuf);
}

void initMaterialTextures(materialTextures* textures, const char* path) {
    textures->color[0] = loadTexFromPath(path, "frontcolor.png");
    textures->color[1] = loadTexFromPath(path, "bevelcolor.png");
    textures->color[2] = loadTexFromPath(path, "topcolor.png");
    textures->norm[0] = loadTexFromPath(path, "frontnorm.png");
    textures->norm[1] = loadTexFromPath(path, "bevelnorm.png");
    textures->norm[2] = loadTexFromPath(path, "topnorm.png");
}

void useMaterialTextures(materialTextures* textures, shader* shader) {
    setIntUniform(shader, "uColor[0]", 0);
    setIntUniform(shader, "uColor[1]", 1);
    setIntUniform(shader, "uColor[2]", 2);
    setIntUniform(shader, "uNormMap[0]", 3);
    setIntUniform(shader, "uNormMap[1]", 4);
    setIntUniform(shader, "uNormMap[2]", 5);
    useTexture(&textures->color[0], 0);
    useTexture(&textures->color[1], 1);
    useTexture(&textures->color[2], 2);
    useTexture(&textures->norm[0], 3);
    useTexture(&textures->norm[1], 4);
    useTexture(&textures->norm[2], 5);
}



// front  - tex 0
// bevel  - tex 1
// top    - tex 2

static void generateSquareBevelMesh(mesh* m, material* mat, polygon* poly) {
    polygon inset;
    polygon p;
    initBuildablePolygon(&inset);
    initBuildablePolygon(&p);
    generateInsetPolygon(poly, &inset, &p, mat->bevelThickness);

    // generate front
    int* tris = malloc((inset.pts - 1 + p.chains) * 3 * sizeof(int));
    triangulate(&inset, tris);
    int interiorFirstVertIdx = m->nVerts;
    for(int i = 0; i < inset.pts; i++) {
        vec2s pt = inset.points[i];
        addVert(m, pt.x, pt.y, -mat->bevelOffset,
                   pt.x, pt.y,
                   0.0f, 0.0f, 1.0f,
                   1.0f, 0.0f, 0.0f,
                   0);
    }
    for(int i = 0; i < inset.pts - 1 + p.chains; i++) {
        int a = tris[i * 3 + 2] + interiorFirstVertIdx;
        int b = tris[i * 3 + 1] + interiorFirstVertIdx;
        int c = tris[i * 3 + 0] + interiorFirstVertIdx;
        addTri(m, a, b, c);
    }

    // generate rim
    for(int i = 0; i < inset.chains; i++) {
        int curr = inset.chainFirst[i];

        float currUvX = 0.0f;
        do {
            int next = inset.next[curr];
            float nextUvX = currUvX + distance(inset.points[curr], inset.points[next]);

            vec3s a;
            glm_vec2_sub(inset.points[next].raw, inset.points[curr].raw, a.raw);
            glm_vec2_normalize(a.raw);
            vec3s b = (vec3s){0.0f, 0.0f, 1.0f};
            vec3s norm;
            glm_vec3_cross(a.raw, b.raw, norm.raw);

            int ci = addVert(m, inset.points[curr].x, inset.points[curr].y, -mat->bevelOffset,
                                currUvX, 0.0f,
                                norm.x, norm.y, norm.z,
                                -a.x, -a.y, -a.z,
                                1);
            int ni = addVert(m, inset.points[next].x, inset.points[next].y, -mat->bevelOffset,
                                nextUvX, 0.0f,
                                norm.x, norm.y, norm.z,
                                -a.x, -a.y, -a.z,
                                1);
            int cr = addVert(m, inset.points[curr].x, inset.points[curr].y, 0,
                             currUvX, 1.0f,
                             norm.x, norm.y, norm.z,
                             -a.x, -a.y, -a.z,
                             1);
            int nr = addVert(m, inset.points[next].x, inset.points[next].y, 0,
                             nextUvX, 1.0f,
                             norm.x, norm.y, norm.z,
                             -a.x, -a.y, -a.z,
                             1);

            addTri(m, ni, cr, ci);
            addTri(m, ni, nr, cr);

            curr = next;
            currUvX = nextUvX;
        } while(curr != inset.chainFirst[i]);
    }

    // generate bevel
    for(int i = 0; i < p.chains; i++) {

        int curr = p.chainFirst[i];
        float currUvX = 0.0f;
        do {
            int next = p.next[curr];
            float innerDist = distance(inset.points[curr], inset.points[next]);
            float nextUvX = currUvX + innerDist;

            int ci = addVert(m, inset.points[curr].x, inset.points[curr].y, 0.0f,
                             currUvX, 0.0f,
                             0.0f, 0.0f, 1.0f,
                             1.0f, 0.0f, 0.0f,
                             1);
            int ni = addVert(m, inset.points[next].x, inset.points[next].y, 0.0f,
                             nextUvX, 0.0f,
                             0.0f, 0.0f, 1.0f,
                             1.0f, 0.0f, 0.0f,
                             1);
            int co = addVert(m, p.points[curr].x, p.points[curr].y, 0.0f,
                             currUvX, 1.0f,
                             0.0f, 0.0f, 1.0f,
                             1.0f, 0.0f, 0.0f,
                             1);
            int no = addVert(m, p.points[next].x, p.points[next].y, 0.0f,
                             nextUvX, 1.0f,
                             0.0f, 0.0f, 1.0f,
                             1.0f, 0.0f, 0.0f,
                             1);

            addTri(m, no, co, ci);
            addTri(m, ci, ni, no);

            curr = next;
            currUvX = nextUvX;
        } while(curr != p.chainFirst[i]);

    }

    // generate top
    for(int i = 0; i < p.chains; i++) {
        int curr = p.chainFirst[i];

        float currUvX = 0.0f;

        do {
            int next = p.next[curr];
            float nextUvX = currUvX + distance(p.points[curr], p.points[next]);

            vec3s a;
            glm_vec2_sub(inset.points[curr].raw, inset.points[next].raw, a.raw);
            glm_vec2_normalize(a.raw);
            vec3s b = (vec3s){0.0f, 0.0f, 1.0f};
            vec3s norm;
            glm_vec3_cross(a.raw, b.raw, norm.raw);

            int cf = addVert(m, p.points[curr].x, p.points[curr].y, 0.0f,
                             currUvX,  1.0f,
                             norm.x, norm.y, norm.z,
                             -a.x, -a.y, -a.z,
                             2);
            int nf = addVert(m, p.points[next].x, p.points[next].y, 0.0f,
                             nextUvX,  1.0f,
                             norm.x, norm.y, norm.z,
                             -a.x, -a.y, -a.z,
                             2);
            int cb = addVert(m, p.points[curr].x, p.points[curr].y, -1.0f,
                             currUvX, 0.0f,
                             norm.x, norm.y, norm.z,
                             -a.x, -a.y, -a.z,
                             2);
            int nb = addVert(m, p.points[next].x, p.points[next].y, -1.0f,
                             nextUvX, 0.0f,
                             norm.x, norm.y, norm.z,
                             -a.x, -a.y, -a.z,
                             2);

            addTri(m, nf, cb, cf);
            addTri(m, nf, nb, cb);

            curr = next;
            currUvX = nextUvX;
        } while(curr != p.chainFirst[i]);
    }

    freeBuildablePolygon(&inset);
    freeBuildablePolygon(&p);
    free(tris);
}

void generatePolygonMesh(mesh* m, material* mat, polygon* p) {
    switch(mat->meshType) {
        case MATERIAL_MESH_TYPE_SQUARE_BEVEL: {
            generateSquareBevelMesh(m, mat, p);
            break;
        }
    }
}