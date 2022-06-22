
#ifndef GEOUTIL_H
#define GEOUTIL_H

#include "cglm.h"
#include <stdbool.h>

float distance(vec2s a, vec2s b);

typedef struct {
    bool vertical;
    float m, b;
} line;

line connect(vec2s a, vec2s b);
vec2s intersect(line l1, line l2);

typedef struct {
    bool intersect;
    vec2s pt;
} segmentIntersectionResult;
segmentIntersectionResult segmentIntersect(vec2s a1, vec2s a2, vec2s b1, vec2s b2);

float triArea(vec2s a, vec2s b, vec2s c);
bool ptInsideTri(vec2s pt, vec2s a, vec2s b, vec2s c);

#endif
