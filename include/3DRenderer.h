#ifndef PUB_3DRenderer_H
#define PUB_3DRenderer_H

#include <stdint.h>
#include "Vectors.h"
#include "Matrix.h"

#define DEFAULT_MODEL ((ModelData) {0, NULL, 0, 0, NULL, {0}, {0}, {1, 1, 1}})

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
    int32_t *zbuffer;
    float fov;
    Vec3f eye_pos;
    Vec3f look_pos;
} View;

typedef struct {
    uint32_t numVerts;
    Vec3f *verts;
    uint32_t numPrimatives;
    uint32_t primativesSize;
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

static inline TformMatrix TranslateTform(Vec3f t) {
    TformMatrix out = {{{ 1, 0, 0, t.x},
                        { 0, 1, 0, t.y},
                        { 0, 0, 1, t.z},
                        { 0, 0, 0,   1}}};
    return out; 
}

static inline TformMatrix LookAtTform(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = Vec3f_Normalize(Vec3f_Subtract(eye, center));
    Vec3f x = Vec3f_Normalize(Vec3f_CrossProduct(up, z));
    Vec3f y = Vec3f_Normalize(Vec3f_CrossProduct(z, x));
    TformMatrix Minv = {{{x.x, x.y, x.z, 0},
                         {y.x, y.y, y.z, 0},
                         {z.x, z.y, z.z, 0},
                         {0,   0,   0,   1}}};
    return TformMatrix_Multiply(Minv, TranslateTform(Vec3f_Negate(eye)));
}

static inline TformMatrix ViewportTform(int32_t x, int32_t y, int32_t w, int32_t h) {
    TformMatrix out = {{{w/2,    0,            0, x + w/2},
                        {  0, -h/2,            0, y + h/2},
                        {  0,    0,    INT32_MAX,      -1},
                        {  0,    0,            0,       1}}};
    return out;
}

static inline TformMatrix PerspectiveTform(float fov, float aspect, float zNear, float zFar) {
    float f = 1/tanf(fov);
    float zscale = (zFar+zNear)/(zNear-zFar);
    float ztrans = 2*zFar*zNear/(zNear-zFar);
    TformMatrix out = {{{f/aspect, 0,      0,      0},
                        {       0, f,      0,      0},
                        {       0, 0, zscale, ztrans},
                        {       0, 0,     -1,      0}}};
    return out;
}

static inline TformMatrix ScaleTform(Vec3f s) {
    TformMatrix out = {{{s.x,   0,   0, 0},
                        {  0, s.y,   0, 0},
                        {  0,   0, s.z, 0},
                        {  0,   0,   0, 1}}};
    return out; 
}

static inline TformMatrix RotationXTform(float rot) {
    rot = rot*2*M_PI/360;
    TformMatrix out = {{{1,         0,          0, 0},
                        {0, cosf(rot), -sinf(rot), 0},
                        {0, sinf(rot),  cosf(rot), 0},
                        {0,         0,          0, 1}}};
    return out;
}

static inline TformMatrix RotationYTform(float rot) {
    rot = rot*2*M_PI/360;
    TformMatrix out = {{{ cosf(rot), 0, sinf(rot), 0},
                        {         0, 1,         0, 0},
                        {-sinf(rot), 0, cosf(rot), 0},
                        {         0, 0,         0, 1}}};
    return out;
}

static inline TformMatrix RotationZTform(float rot) {
    rot = rot*2*M_PI/360;
    TformMatrix out = {{{cosf(rot), -sinf(rot), 0, 0},
                        {sinf(rot),  cosf(rot), 0, 0},
                        {        0,          0, 1, 0},
                        {        0,          0, 0, 1}}};
    return out;
}

void InitView(View *v, uint32_t width, uint32_t height, RGBData *framebuff, int32_t *zbuffer);

void ClearViewBuffers(View *v);

void RenderModel(View *views, uint32_t view_count, ModelData m);

void DrawLine(View *v, int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBData color);

void DrawTriangle(View *v, Vec3i v0, Vec3i v1, Vec3i v2, RGBData color);

#endif