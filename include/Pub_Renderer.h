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

static inline uint32_t GetPixelOffset(Renderer *r, uint32_t x, uint32_t y) {
    return r->w*y + x;
}

static inline void SetPixel(Renderer *r, uint32_t x, uint32_t y, RGBData color) {
    r->framebuf[GetPixelOffset(r, x, y)] = color;
}

Renderer * CreateRenderer(uint32_t width, uint32_t height);

void DrawLine(Renderer *r, int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBData color);

#endif