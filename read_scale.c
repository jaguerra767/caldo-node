//
// Created by Jorge Guerra on 8/11/23.
//
#include "read_scale.h"
#include "common.h"
#include "pico/stdio.h"
#include <stdio.h>

#define SCALE_BUFFER_LENGTH 100


#define PRINT_ARR(arr, len) \
    do { \
        for(size_t i = 0; i < len; ++i) { \
            printf("hx711_multi_t chip %i: %li\n", i, arr[i]); \
        } \
    } while(0)



hx711_multi_t hxm;
int32_t buffer[4];

void setup_scales(){
    hx711_multi_config_t hxmcfg;
    hx711_multi_get_default_config(&hxmcfg);
    hxmcfg.clock_pin = 14;
    hxmcfg.data_pin_base = 15;
    hxmcfg.chips_len = 4;
    hxmcfg.pio_irq_index = 1;
    hxmcfg.dma_irq_index = 1;



    // 1. initialise
    hx711_multi_init(&hxm, &hxmcfg);

    // 2. Power up the HX711 chips and set gain on each chip
    hx711_multi_power_up(&hxm, hx711_gain_128);

    //3. This step is optional. Only do this if you want to
    //change the gain AND save it to each HX711 chip
    //
    //hx711_multi_set_gain(&hxm, hx711_gain_64);
    //hx711_multi_power_down(&hxm);
    //hx711_wait_power_down();
    //hx711_multi_power_up(&hxm, hx711_gain_64);

    // 4. Wait for readings to settle
    hx711_wait_settle(hx711_rate_80);

}



void scale_measure(){
    hx711_multi_get_values(&hxm, buffer);
}

void send_weight(){
    PRINT_ARR(buffer,4);
}

