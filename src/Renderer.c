#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "Pub_Renderer.h"
#include "OBJReader.h"

#define swap(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

static inline int32_t max(int32_t a, int32_t b) {
    return (a > b) ? a : b;
}

static inline int32_t min(int32_t a, int32_t b) {
    return (a < b) ? a : b;
}

Renderer * CreateRenderer(uint32_t width, uint32_t height) {
    Renderer *r = (Renderer *) malloc(sizeof(Renderer));
    r->w = width;
    r->h = height;
    r->framebuf = (RGBData *) malloc(sizeof(RGBData) * width * height);
    r->zbuffer = (int32_t *) malloc(sizeof(int32_t) * width * height);

    memset(r->framebuf, 0, width * height * sizeof(RGBData));
    memset(r->zbuffer, INT32_MIN, width * height * sizeof(int32_t));

    return r;
}

void DrawLine(Renderer *r, int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBData color) {
    bool steep = false;
    if (abs(x0-x1) < abs(y0-y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int dx = x1-x0;
    int dy = y1-y0;
    int derror = abs(dy)*2;
    int error = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            SetPixel(r, y, x, color);
        } else {
            SetPixel(r, x, y, color);
        }
        error += derror;
        if (error > dx) {
            y += (y1 > y0) ? 1 : -1;
            error -= dx*2;
        }
    }
}

void RenderWireframe(Renderer *r, OBJData data) {
    for (uint32_t i = 0; i < data.numFaces; i++) {
        OBJFace face = data.faces[i];
        for (uint32_t j = 0; j < 3; j++) {
            Vec3f v0 = data.verts[face.vertIdxs[j]];
            Vec3f v1 = data.verts[face.vertIdxs[(j+1)%3]];
            uint32_t x0 = (v0.x+1.0)*r->w/2.0;
            uint32_t y0 = r->h - (v0.y+1.0)*r->h/2.0;
            uint32_t x1 = (v1.x+1.0)*r->w/2.0;
            uint32_t y1 = r->h - (v1.y+1.0)*r->h/2.0;
            DrawLine(r, x0, y0, x1, y1, (RGBData) {.r = 255, .g = 255, .b = 255});
        }
    }
}

static Vec3f GetBarycentric(Vec2i a, Vec2i b, Vec2i c, Vec2i p) {
    Vec3f vx = {b.x-a.x, c.x-a.x, a.x-p.x};
    Vec3f vy = {b.y-a.y, c.y-a.y, a.y-p.y};
    Vec3f u = Vec3f_CrossProduct(vx, vy);

    if (abs(u.z) < 1) {
        return (Vec3f) {-1, 1, 1};
    }
    return (Vec3f) {1.0f-(u.x+u.y)/u.z, u.x/u.z, u.y/u.z};
}

// TODO: Test other forms of rasterization on HW
// TODO: Fix bug with negative Zs
void DrawTriangle(Renderer *r, Vec3i v0, Vec3i v1, Vec3i v2, RGBData color) {
    if (v0.y == v1.y && v0.y == v2.y) return;
    if (v0.y > v1.y) swap(v0, v1);
    if (v0.y > v2.y) swap(v0, v2);
    if (v1.y > v2.y) swap(v1, v2);
    int32_t tri_height = v2.y - v0.y;
    for (uint32_t y = v0.y; y <= v2.y; y++) {
        Vec3i s0, s1;
        if (y > v1.y || v1.y == v0.y) {
            // In 2nd Segment 
            s0 = v1;
            s1 = v2;
        } else {
            // In 1st Segment 
            s0 = v0;
            s1 = v1;
        }

        int32_t seg_height = s1.y - s0.y;
        int32_t alpha_num = y - v0.y;
        int32_t beta_num = y - s0.y;
        int32_t Ax = v0.x + ((v2.x - v0.x)*alpha_num)/tri_height;
        int32_t Bx = s0.x + ((s1.x - s0.x)*beta_num)/seg_height;
        int32_t Az = v0.z + ((v2.z - v0.z)*alpha_num)/tri_height;
        int32_t Bz = s0.z + ((s1.z - s0.z)*beta_num)/seg_height;
        if (Ax > Bx) {
            swap(Ax, Bx);
            swap(Az, Bz);
        }

        // Protect for div by 0
        int32_t z_div = (Ax == Bx) ? 1 : (Bx - Ax);
        for (int32_t i = Ax; i <= Bx; i++) {
            int32_t z = Az + ((Bz - Az)*(i - Ax))/z_div;
            SetPixelZ(r, i, y, z, color);
        }
    }
}
/*void DrawTriangle(Renderer *r, Vec2i v0, Vec2i v1, Vec2i v2, RGBData color) {
    Vec2i boxmin = {max(0, min(v0.x, min(v1.x, min(v2.x, r->w-1)))),
                    max(0, min(v0.y, min(v1.y, min(v2.y, r->h-1))))};
    Vec2i boxmax = {min(r->w-1, max(v0.x, max(v1.x, max(v2.x, 0)))),
                    min(r->h-1, max(v0.y, max(v1.y, max(v2.y, 0))))};
    Vec2i p;
    for (p.x = boxmin.x; p.x <= boxmax.x; p.x++) {
        for (p.y = boxmin.y; p.y <= boxmax.y; p.y++) {
            Vec3f bc_cord = GetBarycentric(v0, v1, v2, p);
            if (bc_cord.x >= 0 && bc_cord.y >= 0 && bc_cord.z >= 0) {
                SetPixel(r, p.x, p.y, color);
            }
        }
    }
}*/