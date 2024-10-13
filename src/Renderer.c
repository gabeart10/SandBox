#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "Pub_Renderer.h"
#include "OBJReader.h"

#define swap(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

Renderer * CreateRenderer(uint32_t width, uint32_t height) {
    Renderer *r = (Renderer *) malloc(sizeof(Renderer));
    r->w = width;
    r->h = height;
    r->framebuf = (RGBData *) malloc(sizeof(RGBData) * width * height);

    memset(r->framebuf, 0, width * height * sizeof(RGBData));

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

// TODO: Test other forms of rasterization on HW
void DrawTriangle(Renderer *r, Vec2ui v0, Vec2ui v1, Vec2ui v2, RGBData color) {
    if (v0.y > v1.y) swap(v0, v1);
    if (v0.y > v2.y) swap(v0, v2);
    if (v1.y > v2.y) swap(v1, v2);
    int32_t tri_height = v2.y - v0.y;
    for (uint32_t y = v0.y; y <= v2.y; y++) {
        Vec2ui s0, s1;
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
        uint32_t Ax = v0.x + (((int32_t) v2.x - (int32_t) v0.x)*alpha_num)/tri_height;
        uint32_t Bx = s0.x + (((int32_t) s1.x - (int32_t) s0.x)*beta_num)/seg_height;
        if (Ax > Bx) swap(Ax, Bx);

        for (uint32_t i = Ax; i <= Bx; i++) {
            SetPixel(r, i, y, color);
        }
    }
}