#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"

int main()
{
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    bool state = true;
    while (true) {
        printf("Hello, world!\n");
        gpio_put(PICO_DEFAULT_LED_PIN, state);
        state = !state;
        sleep_ms(1000);
    }
}
