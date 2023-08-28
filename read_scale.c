//
// Created by Jorge Guerra on 8/11/23.
//
#include "read_scale.h"
#include "../include/hx711_scale_adaptor.h"
#include "../include/scale.h"
#include "comms.h"
#include <math.h>
#include <pico/printf.h>
#include <string.h>
#include <stdlib.h>

#define SCALE_BUFFER_LENGTH 100
#define MASS_TO_STR_BUFF_SIZE 64
#define READ_BUFFER_LEN 1000

int32_t buffer[SCALE_BUFFER_LENGTH];

hx711_t hx = {0};
hx711_config_t hxcfg = {0};
hx711_scale_adaptor_t hxsa = {0};

scale_t sc = {0};
scale_options_t opt = {0};

const mass_unit_t unit = mass_g;
int32_t refUnit = 41;//
int32_t offset = 291579;// change


static mass_t mass;

void setup_scales(){
    //Initialize scale options
    scale_options_get_default(&opt);
    opt.buffer = buffer;
    opt.bufflen = SCALE_BUFFER_LENGTH;
    //Config hx711
    hx711_get_default_config(&hxcfg);
    hxcfg.clock_pin = 14;
    hxcfg.data_pin = 15;
    //Initialize hx711
    hx711_init(&hx, &hxcfg);
    hx711_power_up(&hx, hx711_gain_128);
    hx711_wait_settle(hx711_rate_10);
    //Scale adaptor init
    hx711_scale_adaptor_init(&hxsa, &hx);
    //Scale init
    scale_init(&sc, hx711_scale_adaptor_get_base(&hxsa), unit, refUnit, offset);
    opt.strat = strategy_type_samples;
    opt.samples = 1000;

}

void tare(){
    opt.strat = strategy_type_time;
    opt.timeout = 10000000;
    if(scale_zero(&sc, &opt)) {
        printf("Scale zeroed successfully\n");
    }
    else {
        printf("Scale failed to zero\n");
    }
    //Change timeout back to regular after we tare
    opt.strat = strategy_type_samples;
}

char str[MASS_TO_STR_BUFF_SIZE];

void scale_measure(){
    memset(str, 0, MASS_TO_STRING_BUFF_SIZE);
    if(!scale_weight(&sc, &mass, &opt)) {
        printf("Failed to read weight\n");
    }
}

void send_weight(){
    mass_to_string(&mass, str);
    printf("%s\n", str);
}

void calibrate(){
    char cal_buffer[SCALE_BUFFER_LENGTH];
    double raw = 0;

    opt.samples = 1000;

    printf("****************** Starting Calibration Procedure ******************\n");

    printf("\n1. Pick an object and enter its weight in grams.\n");
    get_line(cal_buffer, sizeof(cal_buffer));
    const double known_weight = atof(cal_buffer);
    if(known_weight < 1){
        printf("\nPlease select a weight higher than 1 gram and relaunch process.\n");
        return;
    }
    printf("\nWeight entered: %f.\n", known_weight);

    printf("\n2. Remove all items from the scale, then press enter.\n");
    getchar();

    printf("\nCalibration in process, getting 'zero value'...\n");
    if(!scale_read(&sc, &raw, &opt)){
        printf("\nUnable to read from scale, check wiring.\n");
        return;
    }
    const int32_t zero_value = (int32_t)round(raw);

    printf("\n3. Place known weight object on scale and then press enter.\n");
    getchar();

    printf("\nMeasuring known weight object...\n");
    if(!scale_read(&sc, &raw, &opt)){
        printf("\nUnable to read from scale, check wiring.\n");
        return;
    }
    const double ref_unit_float = (raw - zero_value)/known_weight;
    refUnit = (int32_t)round(ref_unit_float);
    if(refUnit == 0){
        refUnit++;
    }
    printf("\nNew reference unit: %ld, new offset %ld\n", refUnit, offset);
    printf("****************** Finished Calibration Procedure ******************\n");
    printf("Raw value: %ld\n", (int32_t)raw);
    printf("reference unit: %ld\n", refUnit);
    printf("Zero Value: %ld\n", zero_value);
    printf("Provide Zero value as offset and reference unit to program and recompile.");
}