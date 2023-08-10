// MIT License
//
// Copyright (c) 2023 Daniel Robertson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdlib.h>
#include <stdio.h>
#include "pico/stdio.h"
#include "tusb.h"
#include "../include/common.h"

const uint8_t led_pin = 25;
int main(void) {

    stdio_init_all();
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    while (!tud_cdc_connected()) {
        sleep_ms(1);
    }
    gpio_put(led_pin, true);
    hx711_config_t hxcfg;
    hx711_get_default_config(&hxcfg);

    hxcfg.clock_pin = 14;
    hxcfg.data_pin = 15;

    hx711_t hx;

    // 1. Initialise
    hx711_init(&hx, &hxcfg);

    //2. Power up the hx711 and set gain on chip
    hx711_power_up(&hx, hx711_gain_128);

    //3. This step is optional. Only do this if you want to
    //change the gain AND save it to the HX711 chip
    //
    //hx711_set_gain(&hx, hx711_gain_64);
    //hx711_power_down(&hx);
    //hx711_wait_power_down();
    //hx711_power_up(&hx, hx711_gain_64);

    // 4. Wait for readings to settle
    hx711_wait_settle(hx711_rate_80);

    // 5. Read values
    // You can now...

    // wait (block) until a value is obtained
    // cppcheck-suppress invalidPrintfArgType_sint
    printf("blocking value: %li\n", hx711_get_value(&hx));

    // or use a timeout
    int32_t val;
    const uint timeout = 250000; //microseconds
    if(hx711_get_value_timeout(&hx, &val, timeout)) {
        // value was obtained within the timeout period,
        // in this case within 250 milliseconds
        // cppcheck-suppress invalidPrintfArgType_sint
        printf("timeout value: %li\n", val);
    }
    else {
        printf("value was not obtained within the timeout period\n");
    }

    // or see if there's a value, but don't block if there isn't one ready
    if(hx711_get_value_noblock(&hx, &val)) {
        // cppcheck-suppress invalidPrintfArgType_sint
        printf("noblock value: %li\n", val);
    }
    else {
        printf("value was not present\n");
    }

    //6. Stop communication with HX711


    printf("Closed communication with single HX711 chip\n");
    while(1){
        printf("Blinking!\r\n");
        gpio_put(led_pin, true);
        sleep_ms(100);
        printf("blocking value: %li\n", hx711_get_value(&hx));
        gpio_put(led_pin, false);
        sleep_ms(100);
    }
    hx711_close(&hx);
    return EXIT_SUCCESS;

}

//#include "pico/stdlib.h"
//#include "common.h"
//#include "tusb.h"
//#include <stdio.h>
//
//
//
//const uint8_t led_pin = 25;
//int main(){
//    while (!tud_cdc_connected()) {
//        sleep_ms(1);
//    }
//    //Init LED Pin
//    gpio_init(led_pin);
//    gpio_set_dir(led_pin, GPIO_OUT);
//
//    //Init chosen serial port
//    stdio_init_all();
//    hx711_t hx;
//    hx711_config_t hxcnfg;
//    hx711_get_default_config(&hxcnfg);
//    hxcnfg.clock_pin=14;
//    hxcnfg.data_pin=15;
//
//    hx711_init(&hx, &hxcnfg);
//    hx711_power_up(&hx, hx711_gain_128);
//    hx711_wait_settle(hx711_rate_10);
//    while(true) {
//        printf("Blinking!\r\n");
//        gpio_put(led_pin, true);
//        sleep_ms(1000);
//        printf("blocking value: %li\n", hx711_get_value(&hx));
//        gpio_put(led_pin, false);
//        sleep_ms(1000);
//    }
//}
//
//
//
