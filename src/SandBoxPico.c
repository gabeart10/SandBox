#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "3DRenderer.h"

#define WINDOW_WIDTH (128)
#define WINDOW_HEIGHT (96)

int32_t zbuffer[WINDOW_WIDTH*WINDOW_HEIGHT];
RGBData framebuffer[WINDOW_WIDTH*WINDOW_HEIGHT];
View v;

extern ModelData suzanne;

int main()
{
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    InitView(&v, WINDOW_WIDTH, WINDOW_HEIGHT, framebuffer, zbuffer);
    suzanne.scale = V3F(5, 5, 5);
    suzanne.world_loc = V3F(0, 0, -20);
    v.look_pos = V3F(0, 0, -20);
    v.eye_pos = V3F(0, 0, 0);
    v.fov = 45;

    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        absolute_time_t start = get_absolute_time();
        RenderModel(&v, 1, suzanne);
        int64_t diff = absolute_time_diff_us(start, get_absolute_time());
        printf("Render Time: %lldus (%dfps)\n", diff, (1000*1000)/diff);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(1000);
    }
}
