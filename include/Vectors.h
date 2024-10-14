#ifndef VECTORS_H
#define VECTORS_H

#include <stdint.h>
#include <math.h>

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
    int32_t x;
    int32_t y;
} Vec2i;

static inline Vec3f Vec3f_CrossProduct(Vec3f a, Vec3f b) {
    Vec3f ret_vec;
    ret_vec.x = a.y*b.z - a.z*b.y;
    ret_vec.y = a.z*b.x - a.x*b.z;
    ret_vec.z = a.x*b.y - a.y*b.x;
    return ret_vec;
}

static inline float Vec3f_Magnitude(Vec3f a) {
    return sqrtf(powf(a.x, 2) + powf(a.y, 2) + pow(a.z, 2));
}

static inline Vec3f Vec3f_Normalize(Vec3f a) {
    float mag = Vec3f_Magnitude(a);
    a.x /= mag;
    a.y /= mag;
    a.z /= mag;
    return a;
}

static inline float Vec3f_DotProduct(Vec3f a, Vec3f b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline Vec3f Vec3f_Subtract(Vec3f a, Vec3f b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

#endif