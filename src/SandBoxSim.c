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

int32_t zbuffer[WINDOW_WIDTH*WINDOW_HEIGHT];
RGBData framebuffer[WINDOW_WIDTH*WINDOW_HEIGHT];
View v;

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

    InitView(&v, WINDOW_WIDTH, WINDOW_HEIGHT, framebuffer, zbuffer);
    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

    ModelData data = ReadOBJ("../objs/suzanne.obj");
    data.scale = V3F(5, 5, 5);
    data.world_loc = V3F(0, 0, -20);
    v.look_pos = V3F(0, 0, -20);
    v.eye_pos = V3F(0, 0, 0);
    v.fov = 45;
    float eye_angle = 0;
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) break;
        }

        clock_t start = clock();
        v.eye_pos = TformPoint_to_Vec3f(TformMatrix_Apply(TranslateTform(V3F(0, 0, -20)),
                                         TformMatrix_Apply(RotationYTform(eye_angle), (TformPoint) {{0, 0, 12, 1}})));
        eye_angle += 5;
        if (eye_angle >= 360) eye_angle -= 360;
        RenderModel(&v, 1, data);

        float render_time = ((double) (clock() - start))/CLOCKS_PER_SEC;
        printf("Time Taken: %fsec\nRot: %f\n", render_time, eye_angle);
        float sleep_time = 1.0f/15.0f - render_time;
        if (sleep_time > 0) {
            SDL_Delay(sleep_time*1000);
        }

        SDL_LockSurface(screenSurface);
        for (uint32_t y = 0; y < v.h; y++) {
            for (uint32_t x = 0; x < v.w; x++) {
                uint32_t * const target = (uint32_t *) ((uint8_t *) screenSurface->pixels
                                                        + y * screenSurface->pitch
                                                        + x * screenSurface->format->BytesPerPixel);
                const RGBData c = v.framebuf[GetPixelOffset(&v, x, y)];
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