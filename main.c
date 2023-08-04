#include "pico/stdlib.h"
//#include <cstdint>
#include <stdio.h>

const uint8_t led_pin = 25;

int main() {
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    stdio_init_all();

    while(true){
        printf("Blinking");
        gpio_put(led_pin, true);
        sleep_ms(500);
        gpio_put(led_pin, false);
        sleep_ms(500);
    }
}
