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

#include <string.h>
#include <pico/printf.h>
#include "pico/stdio.h"
#include "../include/hx711_scale_adaptor.h"
#include "../include/scale.h"
#include "tusb.h"
#include "ring_buffer.h"


#define BUFFER_LEN 1000
const uint8_t led_pin = 25;

typedef enum read_process_t{
    READING,
    MSG_COMPLETE
}read_process_t;

typedef enum diag_t {
    SUCCESS,
    UNKNOWN_CMD
}diag_t;

read_process_t read_message(ring_buffer_t* buffer){
    int ch = getchar_timeout_us(0);
    if(ch != -1){
        //Leaving '\n' out of the buffer because we process messages right away... revisit if this sucks
        if(ch == '\n'){
            return MSG_COMPLETE;
        }
        ring_buffer_write(buffer, ch);
    }
    return READING;
}

diag_t parse_msg(ring_buffer_t *buffer){
    uint8_t device = ring_buffer_read(buffer);
    printf("Device: %c , %d\n",device, device);
    uint8_t device_id = ring_buffer_read(buffer);
    printf("Device_id: %c , %d\n",device_id, device_id);
    switch(device){
        case 'q':
            printf("Query Sent for device: %d\n", device_id);
            break;
        case 'a':
            printf("Actuator command sent for act: %d\n", device_id);
            uint8_t cmd = ring_buffer_read(buffer);
            if(cmd == 'o'){
                printf("OPEN");
            }else if(cmd == 'c'){
                printf("CLOSE");
            }else{
                printf("Invalid cmd\n");
            }
            break;
        default:
            printf("Invalid Cmd\n");
            return UNKNOWN_CMD;
    }
    return SUCCESS;
}



int main(void) {
    uint8_t msg_buffer[BUFFER_LEN];
    ring_buffer_t rb = {
      .buffer=msg_buffer,
      .write_index=0,
      .read_index=0,
      .max_length=BUFFER_LEN
    };

    stdio_init_all();
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    const mass_unit_t unit = mass_g;
    const int32_t refUnit = 432;
    const int32_t offset = -367539;

    //1. declare relevant variables
    hx711_t hx = {0};
    hx711_config_t hxcfg = {0};
    hx711_scale_adaptor_t hxsa = {0};

    scale_t sc = {0};
    scale_options_t opt = {0};
    scale_options_get_default(&opt);

    //2. provide a read buffer for the scale
    //NOTE: YOU MUST DO THIS
    const size_t valbufflen = 1000;
    int32_t valbuff[valbufflen];
    opt.buffer = valbuff;
    opt.bufflen = valbufflen;

    char str[MASS_TO_STRING_BUFF_SIZE];

    //3. in this example a hx711 is used, so initialise it
    hx711_get_default_config(&hxcfg);
    hxcfg.clock_pin = 14;
    hxcfg.data_pin = 15;

    while (!tud_cdc_connected()) {
        sleep_ms(1);
    }
    hx711_init(&hx, &hxcfg);
    hx711_power_up(&hx, hx711_gain_128);
    hx711_wait_settle(hx711_rate_80);

    //4. provide a pointer to the hx711 to the adaptor
    hx711_scale_adaptor_init(&hxsa, &hx);

    //5. initalise the scale
    scale_init(&sc, hx711_scale_adaptor_get_base(&hxsa), unit, refUnit, offset);

    //6. spend 10 seconds obtaining as many samples as
    //possible to zero (aka. tare) the scale. The max
    //number of samples will be limited to the size of
    //the buffer allocated above
    opt.strat = strategy_type_time;
    opt.timeout = 10000000;

    if(scale_zero(&sc, &opt)) {
        printf("Scale zeroed successfully\n");
    }
    else {
        printf("Scale failed to zero\n");
    }

    mass_t mass;
    mass_t max;
    mass_t min;

    //change to spending 250 milliseconds obtaining
    opt.timeout = 250000;

    mass_init(&max, mass_g, 0);
    mass_init(&min, mass_g, 0);

    uint8_t cmd[5];

    for(;;) {
        gpio_put(led_pin, true);
        memset(str, 0, MASS_TO_STRING_BUFF_SIZE);
        read_process_t rp = read_message(&rb);

        if(rp==MSG_COMPLETE){
            printf("Message received\n");
            parse_msg(&rb);
        }
        //obtain a mass from the scale
        if(scale_weight(&sc, &mass, &opt)) {

            //check if the newly obtained mass
            //is less than the existing minimum mass
            if(mass_lt(&mass, &min)) {
                min = mass;
            }

            //check if the newly obtained mass
            //is greater than the existing maximum mass
            if(mass_gt(&mass, &max)) {
                max = mass;
            }

            //display the newly obtained mass...
            mass_to_string(&mass, str);
//            printf("%s", str);

            //...the current minimum mass...
            mass_to_string(&min, str);
//            printf(" min: %s", str);

            //...and the current maximum mass
            mass_to_string(&max, str);
//            printf(" max: %s\n", str);

        }
        else {
            printf("Failed to read weight\n");
        }
        sleep_ms(200);
        gpio_put(led_pin, false);
        sleep_ms(200);

    }

    return EXIT_SUCCESS;

}
//int main(void) {
//
//    stdio_init_all();
//    gpio_init(led_pin);
//    gpio_set_dir(led_pin, GPIO_OUT);
//    while (!tud_cdc_connected()) {
//        sleep_ms(1);
//    }
//    gpio_put(led_pin, true);
//    hx711_config_t hxcfg;
//    hx711_get_default_config(&hxcfg);
//
//    hxcfg.clock_pin = 14;
//    hxcfg.data_pin = 15;
//
//    hx711_t hx;
//
//    // 1. Initialise
//    hx711_init(&hx, &hxcfg);
//
//    //2. Power up the hx711 and set gain on chip
//    hx711_power_up(&hx, hx711_gain_128);
//
//    //3. This step is optional. Only do this if you want to
//    //change the gain AND save it to the HX711 chip
//    //
//    //hx711_set_gain(&hx, hx711_gain_64);
//    //hx711_power_down(&hx);
//    //hx711_wait_power_down();
//    //hx711_power_up(&hx, hx711_gain_64);
//
//    // 4. Wait for readings to settle
//    hx711_wait_settle(hx711_rate_80);
//
//    // 5. Read values
//    // You can now...
//
//    // wait (block) until a value is obtained
//    // cppcheck-suppress invalidPrintfArgType_sint
//    printf("blocking value: %li\n", hx711_get_value(&hx));
//
//    // or use a timeout
//    int32_t val;
//    const uint timeout = 250000; //microseconds
//    if(hx711_get_value_timeout(&hx, &val, timeout)) {
//        // value was obtained within the timeout period,
//        // in this case within 250 milliseconds
//        // cppcheck-suppress invalidPrintfArgType_sint
//        printf("timeout value: %li\n", val);
//    }
//    else {
//        printf("value was not obtained within the timeout period\n");
//    }
//
//    // or see if there's a value, but don't block if there isn't one ready
//    if(hx711_get_value_noblock(&hx, &val)) {
//        // cppcheck-suppress invalidPrintfArgType_sint
//        printf("noblock value: %li\n", val);
//    }
//    else {
//        printf("value was not present\n");
//    }
//
//    //6. Stop communication with HX711
//
//
//    printf("Closed communication with single HX711 chip\n");
//    while(1){
//        printf("Blinking!\r\n");
//        gpio_put(led_pin, true);
//        sleep_ms(100);
//        printf("blocking value: %li\n", hx711_get_value(&hx));
//        gpio_put(led_pin, false);
//        sleep_ms(100);
//    }
//    hx711_close(&hx);
//    return EXIT_SUCCESS;
//
//}

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
