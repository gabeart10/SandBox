#ifndef PUB_RENDERER_H
#define PUB_RENDERER_H

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t test;
} RGBData;

typedef struct {
    uint32_t w, h;
    RGBData *framebuf; 
} Renderer;

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t z;
} Vec3ui;

typedef struct {
    float x;
    float y;
    float z;
} Vec3f;

typedef struct {
    uint32_t x;
    uint32_t y;
} Vec2ui;

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

Renderer * CreateRenderer(uint32_t width, uint32_t height);

void DrawLine(Renderer *r, int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBData color);

void RenderWireframe(Renderer *r, OBJData data);

void DrawTriangle(Renderer *r, Vec2ui v0, Vec2ui v1, Vec2ui v2, RGBData color);

#endif