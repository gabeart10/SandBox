#include "SDL.h"
#include <stdint.h>
#include "Pub_Renderer.h"

#define WINDOW_WIDTH (128)
#define WINDOW_HEIGHT (96)

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

    DrawLine(r, 30, 30, 100, 90, (RGBData) {.r = 255, .g = 255, .b = 255});
    DrawLine(r, 90, 30, 30, 90, (RGBData) {.r = 255, .g = 255, .b = 255});
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