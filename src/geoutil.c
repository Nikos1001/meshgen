
#include "geoutil.h"

#include "math.h"


float distance(vec2s a, vec2s b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}



line connect(vec2s a, vec2s b) {
    if(a.x == b.x) {
        line l;
        l.vertical = true;
        l.b = a.x;
        return l;
    }
    line l;
    l.vertical = false;
    l.m = (b.x != a.x) ? ((b.y - a.y) / (b.x - a.x)) : 999999.0f;
    l.b = a.y - l.m * a.x;
    return l;
}

vec2s intersect(line l1, line l2) {
    if(l1.vertical) {
        vec2s v;
        v.x = l1.b;
        v.y = l2.m * v.x + l2.b;
        return v;
    }
    if(l2.vertical) {
        vec2s v;
        v.x = l2.b;
        v.y = l1.m * v.x + l1.b;
        return v;
    }
    float x = (l2.b - l1.b) / (l1.m - l2.m);
    float y = l1.m * x + l1.b;
    return (vec2s){x, y};
}

segmentIntersectionResult segmentIntersect(vec2s a1, vec2s a2, vec2s b1, vec2s b2) {
    segmentIntersectionResult result;
    line a = connect(a1, a2);
    line b = connect(b1, b2);
    result.pt = intersect(a, b);
    float minAX = fmin(a1.x, a2.x);
    float maxAX = fmax(a1.x, a2.x);
    float minBX = fmin(b1.x, b2.x);
    float maxBX = fmax(b1.x, b2.x);
    float minAY = fmin(a1.y, a2.y);
    float maxAY = fmax(a1.y, a2.y);
    float minBY = fmin(b1.y, b2.y);
    float maxBY = fmax(b1.y, b2.y);
    result.intersect = result.pt.x >= minAX && result.pt.x <= maxAX && result.pt.x >= minBX && result.pt.x <= maxBX &&
                       result.pt.y >= minAY && result.pt.y <= maxAY && result.pt.y >= minBY && result.pt.y <= maxBY;
    return result;
}



float triArea(vec2s a, vec2s b, vec2s c) {
    return fabs(
        a.x * (b.y - c.y) +
        b.x * (c.y - a.y) +
        c.x * (a.y - b.y)
    );
}

bool ptInsideTri(vec2s pt, vec2s a, vec2s b, vec2s c) {
    float A  = triArea(a, b, c);
    float A1 = triArea(a, b, pt);
    float A2 = triArea(b, c, pt);
    float A3 = triArea(a, c, pt);
    return fabs(A - (A1 + A2 + A3)) < 0.00001f;
}
