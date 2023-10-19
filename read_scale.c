//
// Created by Jorge Guerra on 8/11/23.
//
#include "read_scale.h"
#include "common.h"
#include <stdio.h>


hx711_multi_t hxm;
int32_t buffer[4];
hx711_multi_config_t hxmcfg;
void setup_scales(){
    hx711_multi_get_default_config(&hxmcfg);
    hxmcfg.clock_pin = 16;
    hxmcfg.data_pin_base = 17;
    hxmcfg.chips_len = 4;
    hxmcfg.pio_irq_index = 1;
    hxmcfg.dma_irq_index = 1;

    // Initialise
    hx711_multi_init(&hxm, &hxmcfg);

    // Power up the HX711 chips and set gain on each chip
    hx711_multi_power_up(&hxm, hx711_gain_128);

    // Wait for readings to settle
    hx711_wait_settle(hx711_rate_10);

}

void scale_measure(){
    hx711_multi_get_values(&hxm, buffer);
    printf("%li, %li, %li, %li\n", buffer[0], buffer[1], buffer[2], buffer[3]);
}

