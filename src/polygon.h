
#ifndef POLYGON_H
#define POLYGON_H

#include "cglm.h"
#include "dynarr.h"
#include "mesh.h"

typedef struct {
    vec2s* points;
    int* next;
    int* prev;
    int* chainId;
    int* chainFirst;
    int pts;
    int chains;
} polygon;

void initBuildablePolygon(polygon* p);
void freeBuildablePolygon(polygon* p);
void copyBuildablePolygon(polygon* src, polygon* dst);

int addChainToPolygon(polygon* p, vec2s pt);
int addPointToPolygon(polygon* p, vec2s pt, int prev);
void closeChain(polygon* p, int first, int last);

void simplifyPolygon(polygon* p, int* cloneToVertMap);
void triangulate(polygon* p, int* tris);

void generateInsetPolygon(polygon* src, polygon* dst, polygon* dstOuter, float inset);

void dumpPolygonData(polygon* p);

#endif
