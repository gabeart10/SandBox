#ifndef PUB_RENDERER_H
#define PUB_RENDERER_H

#include <stdint.h>
#include "Vectors.h"
#include "Matrix.h"

#define VIEWPORT_DEPTH (255)

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t test;
} RGBData;

typedef struct {
    uint32_t w, h;
    RGBData *framebuf; 
    int32_t *zbuffer;
} Renderer;

typedef struct {
    uint32_t vertIdxs[3];
} OBJFace;

typedef struct {
    uint32_t numVerts;
    Vec3f *verts;
    uint32_t numFaces;
    OBJFace *faces;
} OBJData;

static inline uint32_t GetPixelOffset(Renderer *r, uint32_t x, uint32_t y) {
    return r->w*y + x;
}

static inline void SetPixel(Renderer *r, uint32_t x, uint32_t y, RGBData color) {
    r->framebuf[GetPixelOffset(r, x, y)] = color;
}

static inline void SetPixelZ(Renderer *r, uint32_t x, uint32_t y, int32_t z, RGBData color) {
    if (z > r->zbuffer[GetPixelOffset(r, x, y)]) {
        r->zbuffer[GetPixelOffset(r, x, y)] = z;
        SetPixel(r, x, y, color);
    }
}

Renderer * CreateRenderer(uint32_t width, uint32_t height);

void ClearBuffers(Renderer *r);

void DrawLine(Renderer *r, int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBData color);

void RenderWireframe(Renderer *r, OBJData data);

void DrawTriangle(Renderer *r, Vec3i v0, Vec3i v1, Vec3i v2, RGBData color);

TformMatrix LookAtTform(Vec3f eye, Vec3f center, Vec3f up);

TformMatrix ViewportTform(int32_t x, int32_t y, int32_t w, int32_t h);

TformMatrix PerspectiveTform(float c);

#endif