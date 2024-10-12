#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "Pub_Renderer.h"
#include "OBJReader.h"

static inline void swap(uint32_t *a, uint32_t *b) {
    uint32_t swap_var = *a;
    *a = *b;
    *b = swap_var;
}

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
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
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