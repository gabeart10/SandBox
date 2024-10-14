#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "SDL.h"
#include "OBJReader.h"
#include "Pub_Renderer.h"

#define WINDOW_WIDTH (128*10)
#define WINDOW_HEIGHT (96*10)

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "SandBoxSim",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    Renderer *r = CreateRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

    //RenderWireframe(r, ReadOBJ("../objs/suzanne.obj"));
    OBJData data = ReadOBJ("../objs/suzanne.obj");
    Vec3f light_dir = {0, -0.5, -0.5};
    clock_t start = clock();
    for (uint32_t i = 0; i < data.numFaces; i++) {
        Vec2i scords[3];
        Vec3f wcords[3];
        OBJFace face = data.faces[i];
        for (uint32_t j = 0; j < 3; j++) {
            Vec3f cord = data.verts[face.vertIdxs[j]];
            scords[j] = (Vec2i) {(cord.x+2.0)*r->w/4.0, r->h - (cord.y+2.0)*r->h/4.0};
            wcords[j] = cord;
        }
        Vec3f n = Vec3f_CrossProduct(Vec3f_Subtract(wcords[2], wcords[0]), Vec3f_Subtract(wcords[1], wcords[0]));
        n = Vec3f_Normalize(n);
        float inten = Vec3f_DotProduct(n, light_dir);
        if (inten > 0) {
            DrawTriangle(r, scords[0], scords[1], scords[2], (RGBData) {255*inten, 255*inten, 255*inten});
        }
    }
    printf("Time Taken: %fsec\n", ((double) (clock() - start))/CLOCKS_PER_SEC);
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) break;
        }
        SDL_LockSurface(screenSurface);
        for (uint32_t y = 0; y < r->h; y++) {
            for (uint32_t x = 0; x < r->w; x++) {
                uint32_t * const target = (uint32_t *) ((uint8_t *) screenSurface->pixels
                                                        + y * screenSurface->pitch
                                                        + x * screenSurface->format->BytesPerPixel);
                const RGBData c = r->framebuf[GetPixelOffset(r, x, y)];
                SDL_PixelFormat * const f = screenSurface->format;
                *target = (c.r << f->Rshift) | (c.g << f->Gshift) | (c.b << f->Bshift);
            }
        }
        SDL_UnlockSurface(screenSurface);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}