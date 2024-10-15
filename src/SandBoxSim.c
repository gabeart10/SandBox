#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "SDL.h"
#include "OBJReader.h"
#include "3DRenderer.h"
#include "Matrix.h"

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

    View *v = CreateView(WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

    ModelData data = ReadOBJ("../objs/head.obj");
    v->look_pos = V3F(0, 0, 0);
    v->eye_pos = V3F(0, 0, 5);
    v->fov = 45;
    float eye_delta_z = 0.1;
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) break;
        }

        clock_t start = clock();
        RenderModel(v, 1, data);
        if (v->eye_pos.z <= 0.5 || v->eye_pos.z >= 5) {
            eye_delta_z *= -1;
        }
        v->eye_pos.z += eye_delta_z;

        float render_time = ((double) (clock() - start))/CLOCKS_PER_SEC;
        printf("Time Taken: %fsec\nCenter Z: %f\n", render_time, v->eye_pos.z);
        float sleep_time = 1.0f/15.0f - render_time;
        if (sleep_time > 0) {
            SDL_Delay(sleep_time*1000);
        }

        SDL_LockSurface(screenSurface);
        for (uint32_t y = 0; y < v->h; y++) {
            for (uint32_t x = 0; x < v->w; x++) {
                uint32_t * const target = (uint32_t *) ((uint8_t *) screenSurface->pixels
                                                        + y * screenSurface->pitch
                                                        + x * screenSurface->format->BytesPerPixel);
                const RGBData c = v->framebuf[GetPixelOffset(v, x, y)];
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