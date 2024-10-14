#ifndef MATRIX_H
#define MATRIX_H

#include <Vectors.h>

typedef struct {
    float data[4][4];
} TformMatrix;

typedef struct {
    float data[4];
} TformPoint;

static inline TformMatrix GetIdentityMatrix(void) {
    TformMatrix m = {{{1, 0, 0, 0},
                      {0, 1, 0, 0},
                      {0, 0, 1, 0},
                      {0, 0, 0, 1}}};
    return m;
}

static inline TformMatrix TformMatrix_Multiply(TformMatrix a, TformMatrix b) {
    TformMatrix out = {0};
    for (uint32_t ar = 0; ar < 4; ar++) {
        for (uint32_t bc = 0; bc < 4; bc++) {
            for (uint32_t i = 0; i < 4; i++) {
                out.data[ar][bc] += a.data[ar][i]*b.data[i][bc];
            }
        }
    }
    return out;
}

static inline TformPoint TformMatrix_Apply(TformMatrix m, TformPoint p) {
    TformPoint out = {0};
    for (uint32_t mr = 0; mr < 4; mr++) {
        for (uint32_t i = 0; i < 4; i++) {
            out.data[mr] += m.data[mr][i]*p.data[i];
        }
    }
    return out;
}

static inline TformPoint Vec3f_to_TformPoint(Vec3f v) {
    TformPoint out = {{v.x, v.y, v.z, 1}};
    return out;
}

static inline Vec3f TformPoint_to_Vec3f(TformPoint p) {
    Vec3f out = {p.data[0]/p.data[3], p.data[1]/p.data[3], p.data[2]/p.data[3]};
    return out;
}

#endif