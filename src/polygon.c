
#include "polygon.h"
#include "geoutil.h"
#include "util.h"

void initBuildablePolygon(polygon* p) {
    p->points = dynarr(vec2s);
    p->next = dynarr(int);
    p->prev = dynarr(int);
    p->chainId = dynarr(int);
    p->chainFirst = dynarr(int);
    p->pts = 0;
    p->chains = 0;
}

void freeBuildablePolygon(polygon* p) {
    dynarrFree(&p->points);
    dynarrFree(&p->next);
    dynarrFree(&p->prev);
    dynarrFree(&p->chainId);
    dynarrFree(&p->chainFirst);
}

void copyBuildablePolygon(polygon* src, polygon* dst) {
    initBuildablePolygon(dst);
    for(int i = 0; i < src->pts; i++) {
        dynarrPush(&dst->points, src->points[i]);
        dynarrPush(&dst->next, src->next[i]);
        dynarrPush(&dst->prev, src->prev[i]);
        dynarrPush(&dst->chainId, src->chainId[i]);
    }
    for(int i = 0; i < src->chains; i++) {
        dynarrPush(&dst->chainFirst, src->chainFirst[i]);
    }
    dst->pts = src->pts;
    dst->chains = src->chains;
}



int addChainToPolygon(polygon* p, vec2s pt) {
    dynarrPush(&p->points, pt);
    dynarrPush(&p->next, -1);
    dynarrPush(&p->prev, -1);
    dynarrPush(&p->chainId, p->chains);
    dynarrPush(&p->chainFirst, p->pts);
    p->pts++;
    p->chains++;
    return p->pts - 1;
}

int addPointToPolygon(polygon* p, vec2s pt, int prev) {
    dynarrPush(&p->points, pt);
    dynarrPush(&p->next, -1);
    p->next[prev] = p->pts;
    dynarrPush(&p->prev, prev);
    dynarrPush(&p->chainId, p->chainId[prev]);
    p->pts++;
    return p->pts - 1;
}

void closeChain(polygon* p, int first, int last) {
    p->next[last] = first;
    p->prev[first] = last;
}



static void mergeChains(polygon* p, int src, int dst, int* cloneToVertMap) {

    // find rightmost point m in src chain
    int m = p->chainFirst[src];
    int curr = p->next[m];
    while(curr != p->chainFirst[src]) {
        if(p->points[curr].x > p->points[m].x) {
            m = curr;
        }
        curr = p->next[curr];
    }
    vec2s M = p->points[m];

    // make a ray going from m to the right
    line mRay;
    mRay.m = 0;
    mRay.b = M.y;

    // find rightmost intersection point of ray and dst chain
    // i = intersection point
    // p = rightmost point on intersecting segment of dst chain
    vec2s I;
    vec2s P;
    I.x = -1000000000000000000.0f;
    curr = p->chainFirst[dst];
    do {
        vec2s a = p->points[curr];
        vec2s b = p->points[p->next[curr]];
        line seg = connect(a, b);
        vec2s inter = intersect(mRay, seg);
        if((inter.y >= a.y && inter.y <= b.y) ||
           (inter.y <= a.y && inter.y >= b.y)) {
            if(inter.x > I.x) {
                I = inter;
                P = a.x > b.x ? a : b;
            }
        }
        curr = p->next[curr];
    } while(curr != p->chainFirst[dst]);

    // find closest point A in dst chain to M
    int a = p->chainFirst[dst];
    float minDist = 999999999.9f;
    curr = a;
    do {
        if(ptInsideTri(p->points[curr], M, P, I)) {
            float dist = distance(p->points[curr], M);
            if(dist < minDist) {
                minDist = dist;
                a = curr;
            }
        }
        curr = p->next[curr];
    } while(curr != p->chainFirst[dst]);

    // we found the minimal visible pair!
    // it's a & m!
    // merge the chains
    int mClone = addPointToPolygon(p, M, p->prev[m]);
    int aClone = addPointToPolygon(p, p->points[a], mClone);
    p->next[aClone] = p->next[a];
    p->prev[p->next[a]] = aClone;

    p->next[a] = m;
    p->prev[m] = a;

    curr = p->chainFirst[0];
    do {
        curr = p->next[curr];
    } while(curr != p->chainFirst[0]);

    // update clone mapping
    cloneToVertMap[mClone] = m;
    cloneToVertMap[aClone] = a;

}

static int comparePair(const void* a, const void* b) {
    const pair(int, float)* pa = (const pair(int, float)*)a;
    const pair(int, float)* pb = (const pair(int, float)*)b;
    if(pa->s < pb->s) {
        return -1;
    } else if(pa->s > pb->s) {
        return 1;
    } else {
        return 0;
    }
}

void simplifyPolygon(polygon* p, int* cloneToVertMap) {

    if(p->chains == 1)
        return;

    // assumes chain 0 is the outer chain
    // and all other chains are inner chains

    pair(int, float) sortedChains[p->chains - 1];
    for(int i = 0; i < p->chains - 1; i++) {
        sortedChains[i].f = i + 1;
        sortedChains[i].s = 0;
        int curr = p->chainFirst[i + 1];
        do {
            sortedChains[i].s = fmax(sortedChains[i].s, p->points[curr].x);
            curr = p->next[curr];
        } while(curr != p->chainFirst[i + 1]);
    }

    // sort the chains by their rightmost point
    qsort(sortedChains, p->chains - 1, sizeof(pair(int, float)), comparePair);

    // merge chains, rightmost first
    mergeChains(p, sortedChains[p->chains - 2].f, 0, cloneToVertMap);
    for(int i = p->chains - 2; i >= 1; i--) {
        mergeChains(p, sortedChains[i - 1].f, 0, cloneToVertMap);
    }

}

void triangulate(polygon* poly, int* tris) {
    polygon p;
    copyBuildablePolygon(poly, &p);

    int* cloneToVertMap = malloc((p.pts + (p.chains - 1) * 2) * sizeof(int));
    for(int i = 0; i < p.pts + (p.chains - 1) * 2; i++) {
        cloneToVertMap[i] = i;
    }
    simplifyPolygon(&p, cloneToVertMap);

    int triAddIdx = 0;
    // clip off one ear at a time
    int curr = p.chainFirst[0];
    for(int i = 0; i < p.pts - 3; i++) {

        while(true) {
            int i0 = p.prev[curr];
            vec2s p0 = p.points[i0];
            int i1 = curr;
            vec2s p1 = p.points[i1];
            int i2 = p.next[curr];
            vec2s p2 = p.points[i2];

            // check if (p0, p1, p2) is an ear
            bool isEar = true;
            for(int j = 0; j < p.pts; j++) {
                if(j == i0 || j == i1 || j == i2)
                    continue;
                if(ptInsideTri(p.points[j], p0, p1, p2) &&
                   distance(p.points[j], p0) > 0.0001f  &&
                   distance(p.points[j], p1) > 0.0001f  &&
                   distance(p.points[j], p2) > 0.0001f) {
                    isEar = false;
                    break;
                }
            }

            if(isEar) {

                // check if triangle is inside the polygon
                // or, more precisely, if the angle at p1 is less than 180 degrees
                vec2s p0p1;
                glm_vec2_sub(p1.raw, p0.raw, p0p1.raw);
                vec2s p1p2;
                glm_vec2_sub(p2.raw, p1.raw, p1p2.raw);
                float cross = glm_vec2_cross(p0p1.raw, p1p2.raw);

                if(cross <= 0.0f) {
                    // add triangle
                    tris[triAddIdx + 0] = cloneToVertMap[i0];
                    tris[triAddIdx + 1] = cloneToVertMap[i1];
                    tris[triAddIdx + 2] = cloneToVertMap[i2];
                    triAddIdx += 3;

                    // remove ear tip from future polygon
                    p.next[i0] = i2;
                    p.prev[i2] = i0;
                    p.next[i1] = -1;
                    p.prev[i1] = -1;
                    curr = i2;
                    break;
                }
            }

            curr = p.next[curr];
        }

    }

    // add last triangle
    tris[triAddIdx + 0] = cloneToVertMap[p.prev[curr]];
    tris[triAddIdx + 1] = cloneToVertMap[curr];
    tris[triAddIdx + 2] = cloneToVertMap[p.next[curr]];

    freeBuildablePolygon(&p);
    free(cloneToVertMap);

}



static line makeOffsetLine(vec2s p1, vec2s p2, float offset) {
    vec2s off = (vec2s){p2.y - p1.y, p1.x - p2.x};
    glm_vec2_normalize(off.raw);
    glm_vec2_scale(off.raw, offset, off.raw);

    vec2s a;
    glm_vec2_add(p1.raw, off.raw, a.raw);
    vec2s b;
    glm_vec2_add(p2.raw, off.raw, b.raw);

    return connect(a, b);
}

void generateInsetPolygon(polygon* src, polygon* dst, polygon* dstOuter, float inset) {

    // we want to generate the same chains, but with an offset
    // note that clockwise chains will get inset, and counterclockwise will get outset
    // this matches what we want for generating beveled polygons with holes
    for(int i = 0; i < src->chains; i++) {
        
        int v = -1;
        int first = -1;
        int outerV = -1;
        int outerFirst = -1;

        int curr = src->chainFirst[i];
        do {
            int i0 = src->prev[curr];
            vec2s p0 = src->points[i0];
            int i1 = curr;
            vec2s p1 = src->points[i1];
            int i2 = src->next[curr];
            vec2s p2 = src->points[i2];

            line p0p1 = makeOffsetLine(p0, p1, inset);
            line p1p2 = makeOffsetLine(p1, p2, inset);

            vec2s p = intersect(p0p1, p1p2);

            if(v == -1) {
                v = first = addChainToPolygon(dst, p);
                outerV = outerFirst = addChainToPolygon(dstOuter, p1);
            } else {
                v = addPointToPolygon(dst, p, v);
                outerV = addPointToPolygon(dstOuter, p1, outerV);
            }

            curr = src->next[curr];
        } while(curr != src->chainFirst[i]);

        closeChain(dst, first, v);
        closeChain(dstOuter, outerFirst, outerV);

    }



    // collision resolution
    // when there's a collision, we'll add 2 points to the polygon instead of 1
    // that's why we need the dstOuter polygon
    for(int i = 0; i < 4; i++) {
        int i0 = dst->prev[i];
        vec2s p0 = dst->points[i0];
        int i1 = i;
        vec2s p1 = dst->points[i1];
        int i2 = dst->next[i];
        vec2s p2 = dst->points[i2];

        bool p0p1Intersects = false;
        vec2s p0p1NewPt;

        bool p1p2Intersects = false;
        vec2s p1p2NewPt;


        for(int j = 0; j < dst->pts; j++) {
            
            if(j == i0 || j == i1 || j == i2 || j == dst->prev[i0] || j == dst->next[i2])
                continue;

            vec2s q0 = dst->points[j];
            vec2s q1 = dst->points[dst->next[j]];
            
            segmentIntersectionResult qp0p1 = segmentIntersect(q0, q1, p0, p1);
            if(qp0p1.intersect) {
                if(!p0p1Intersects)
                    p0p1NewPt = qp0p1.pt;
                p0p1Intersects = true;
                if(distance(p0, p0p1NewPt) >= distance(p0, qp0p1.pt)) {
                    p0p1NewPt = qp0p1.pt;
                }
            }

            segmentIntersectionResult qp1p2 = segmentIntersect(q0, q1, p1, p2);
            if(qp1p2.intersect) {
                if(!p1p2Intersects)
                    p1p2NewPt = qp1p2.pt;
                p1p2Intersects = true;
                if(distance(p2, p1p2NewPt) > distance(p2, qp1p2.pt))
                    p1p2NewPt = qp1p2.pt;
            }

        }


        if(p0p1Intersects && p1p2Intersects) {
            p0p1NewPt.y += 0.001f;
            p1p2NewPt.y += 0.001f;
            dst->points[i] = p0p1NewPt;
            int nextPt = dst->next[i];
            int newVert = addPointToPolygon(dst, p1p2NewPt, i);
            dst->next[newVert] = nextPt;
            dst->prev[nextPt] = newVert;

            vec2s newOuterCoords = dstOuter->points[i];
            int newOuterVert = addPointToPolygon(dstOuter, newOuterCoords, i);
            dstOuter->next[newOuterVert] = nextPt;
            dstOuter->prev[nextPt] = newOuterVert;
        }
    }

}

void dumpPolygonData(polygon* p) {
    int curr = p->chainFirst[0];
    do {
        printf("%f %f %d\n", p->points[curr].x, p->points[curr].y, curr);
        curr = p->prev[curr];
    } while(curr != p->chainFirst[0]);
}
