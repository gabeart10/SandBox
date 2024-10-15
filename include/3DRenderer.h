#ifndef PUB_3DRenderer_H
#define PUB_3DRenderer_H

#include <stdint.h>
#include "Vectors.h"
#include "Matrix.h"

#define DEFAULT_MODEL ((ModelData) {0, NULL, 0, NULL, {0}, {0}, {1, 1, 1}})

// TODO: Support more than triangle
enum {
    PRIMATIVE_CIRCLE = 1,
    PRIMATIVE_LINE = 2,
    PRIMATIVE_TRIANGLE = 3
};
typedef uint32_t PrimativeType;

enum {
    STATUS_ERROR = 0,
    STATUS_OK = 1
};
typedef uint32_t ReturnStatus;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGBData;

typedef struct {
    uint32_t w, h;
    RGBData *framebuf; 
    uint32_t *zbuffer;
    float fov;
    Vec3f eye_pos;
    Vec3f look_pos;
} View;

typedef struct {
    uint32_t numVerts;
    Vec3f *verts;
    uint32_t numPrimatives;
    uint32_t *primatives;
    Vec3f world_rot;
    Vec3f world_loc;
    Vec3f scale;
} ModelData;

static inline uint32_t GetPixelOffset(View *v, uint32_t x, uint32_t y) {
    return v->w*y + x;
}

static inline void SetPixel(View *v, uint32_t x, uint32_t y, RGBData color) {
    v->framebuf[GetPixelOffset(v, x, y)] = color;
}

static inline void SetPixelZ(View *v, uint32_t x, uint32_t y, uint32_t z, RGBData color) {
    if (z < v->zbuffer[GetPixelOffset(v, x, y)]) {
        v->zbuffer[GetPixelOffset(v, x, y)] = z;
        SetPixel(v, x, y, color);
    }
}


View * CreateView(uint32_t width, uint32_t height);

void ClearViewBuffers(View *v);

void RenderModel(View *views, uint32_t view_count, ModelData m);

void DrawLine(View *v, int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBData color);

void DrawTriangle(View *v, Vec3i v0, Vec3i v1, Vec3i v2, RGBData color);

#endif