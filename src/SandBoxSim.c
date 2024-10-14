#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "SDL.h"
#include "OBJReader.h"
#include "Pub_Renderer.h"
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

    Renderer *r = CreateRenderer(WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Surface *screenSurface = SDL_GetWindowSurface(window);

    OBJData data = ReadOBJ("../objs/head.obj");
    Vec3f camera_center = {0, 0, 0};
    Vec3f camera_eye = {0, 0, 1};
    Vec3f camera_up = {0, 1, 0};
    float eye_delta_z = -0.1;
    SDL_Event event;
    while (1) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) break;
        }

        clock_t start = clock();
        ClearBuffers(r);
        for (uint32_t i = 0; i < data.numFaces; i++) {
            Vec3i scords[3];
            Vec3f wcords[3];
            OBJFace face = data.faces[i];
            bool keep_face = TRUE;
            for (uint32_t j = 0; j < 3; j++) {
                Vec3f cord = data.verts[face.vertIdxs[j]];
                wcords[j] = cord;
                TformMatrix look = LookAtTform(camera_eye, camera_center, camera_up);
                TformMatrix per = PerspectiveTform(Vec3f_Magnitude(Vec3f_Subtract(camera_eye, camera_center)));
                TformMatrix view = ViewportTform(0, 0, r->w, r->h);
                TformPoint lookp = TformMatrix_Apply(look, Vec3f_to_TformPoint(cord));
                TformPoint perp = TformMatrix_Apply(per, lookp);
                TformPoint viewp = TformMatrix_Apply(view, perp);
                Vec3f scordf = TformPoint_to_Vec3f(viewp);
                scords[j] = (Vec3i) {scordf.x, scordf.y, scordf.z};
            }

            Vec3f n = Vec3f_CrossProduct(Vec3f_Subtract(wcords[2], wcords[0]), Vec3f_Subtract(wcords[1], wcords[0]));
            n = Vec3f_Normalize(n);
            Vec3f light_dir = {-camera_eye.x, -camera_eye.y, -camera_eye.z};
            float inten = Vec3f_DotProduct(n, Vec3f_Normalize(light_dir));
            if (inten > 0) {
                uint32_t color = 205*inten + 50;
                DrawTriangle(r, scords[0], scords[1], scords[2], (RGBData) {color, color, color});
            }
        }
        if (camera_center.z <= -4 || camera_center.z >= 4) {
            eye_delta_z *= -1;
        }
        camera_center.z += eye_delta_z;
        camera_eye.z += eye_delta_z;

        float render_time = ((double) (clock() - start))/CLOCKS_PER_SEC;
        printf("Time Taken: %fsec\nCenter Z: %f\n", render_time, camera_center.z);
        float sleep_time = 1.0f/15.0f - render_time;
        if (sleep_time > 0) {
            SDL_Delay(sleep_time*1000);
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