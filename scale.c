//
// Created by Jorge Guerra on 8/11/23.
//
#include "scale.h"
#include "../include/hx711_scale_adaptor.h"
#include "../include/scale.h"
#include <pico/printf.h>
#include <string.h>

#define SCALE_BUFFER_LENGTH 100
#define MASS_TO_STR_BUFF_SIZE 64



static int32_t buffer[SCALE_BUFFER_LENGTH];

static hx711_t hx = {0};
static hx711_config_t hxcfg = {0};
static hx711_scale_adaptor_t hxsa = {0};

static scale_t sc = {0};
static scale_options_t opt = {0};

const mass_unit_t unit = mass_g;
const int32_t refUnit = 432;
const int32_t offset = -367539;

static mass_t mass;

void setup_scales(){
    //Initialize scale options
    opt.buffer = buffer;
    opt.bufflen = SCALE_BUFFER_LENGTH;
    scale_options_get_default(&opt);
    //Config hx711
    hx711_get_default_config(&hxcfg);
    hxcfg.clock_pin = 14;
    hxcfg.data_pin = 15;
    //Initialize hx711
    hx711_init(&hx, &hxcfg);
    hx711_power_up(&hx, hx711_gain_128);
    hx711_wait_settle(hx711_rate_80);
    //Scale adaptor init
    hx711_scale_adaptor_init(&hxsa, &hx);
    //Scale init
    scale_init(&sc, hx711_scale_adaptor_get_base(&hxsa), unit, refUnit, offset);
    opt.strat = strategy_type_time;
    opt.timeout = 250000;

}

void tare(){
    opt.timeout = 10000000;
    if(scale_zero(&sc, &opt)) {
        printf("Scale zeroed successfully\n");
    }
    else {
        printf("Scale failed to zero\n");
    }
    //Change timeout back to regular after we tare
    opt.timeout = 250000;
}

char str[MASS_TO_STR_BUFF_SIZE];

void scale_measure(){
    memset(str, 0, MASS_TO_STRING_BUFF_SIZE);
    if(scale_weight(&sc, &mass, &opt)) {
        mass_to_string(&mass, str);
        printf("%s\n", str);
    }
    else {
        printf("Failed to read weight\n");
    }
}