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



#include <string.h>
#include <pico/printf.h>
#include "pico/stdio.h"
#include "../include/hx711_scale_adaptor.h"
#include "../include/scale.h"
#include "tusb.h"
#include "ring_buffer.h"


#define BUFFER_LEN 1000
const uint8_t led_pin = 25;

typedef enum {
    READING,
    MSG_COMPLETE
}read_process_t;

typedef enum {
    ACTUATOR,
    LOAD_CELL,
    UNKNOWN
}device_t;

typedef enum {
    OPEN,
    CLOSE,
    INVALID
}operator_t;

typedef struct {
    device_t device;
    uint8_t device_id;
    operator_t operator;
}command_t;

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



command_t parse_msg(ring_buffer_t *buffer){
    command_t result;
    uint8_t device = ring_buffer_read(buffer);
    result.device_id = ring_buffer_read(buffer);
    switch(device){
        case 'q':
            result.device = LOAD_CELL;
            break;
        case 'a':
            result.device = ACTUATOR;
            uint8_t cmd = ring_buffer_read(buffer);
            if(cmd == 'o'){
                result.operator = OPEN;
            }else if(cmd == 'c'){
                result.operator = CLOSE;
            }else{
                result.operator = INVALID;
            }
            break;
        default:
            result.device = UNKNOWN;
    }
    return result;
}



int main(void) {
    uint8_t msg_buffer[BUFFER_LEN];
    memset(msg_buffer, 0, BUFFER_LEN);
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

    //5. initialise the scale
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

    //change to spending 250 milliseconds obtaining
    opt.timeout = 250000;

    for(;;) {
        gpio_put(led_pin, true);
        memset(str, 0, MASS_TO_STRING_BUFF_SIZE);
        read_process_t rp = read_message(&rb);
        command_t cmd;
        if(rp==MSG_COMPLETE){
            printf("Message received\n");
            cmd = parse_msg(&rb);
        }
        if(cmd.device==LOAD_CELL){
            if(scale_weight(&sc, &mass, &opt)) {
                mass_to_string(&mass, str);
                printf("%s\n", str);
            }
            else {
                printf("Failed to read weight\n");
            }
        }
        sleep_ms(200);
        gpio_put(led_pin, false);
        sleep_ms(200);
    }
}
