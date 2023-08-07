#include "pico/stdlib.h"
#include <cstdio>
#include <cstdint>

constexpr std::uint8_t led_pin = 25;



void setup(){
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    stdio_init_all();
}

int main() {
    setup();
    while(true){
        printf("Blinking\r\n");
        gpio_put(led_pin, true);
        sleep_ms(500);
        gpio_put(led_pin, false);
        sleep_ms(500);
    }
}
