//
// Created by Jorge Guerra on 8/11/23.
//
#include <pico/printf.h>
#include "actuator.h"
#include "hardware/adc.h"
#include "pico/stdio.h"

#define ACTUATOR_LIMIT 4000
#define ACT_OPEN_POS 20

typedef enum {
    AT_OP_LIMIT,
    AT_CL_LIMIT,
    IND
}pot_state_t;

void actuator_io_setup(){
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
}

pot_state_t get_pot_state(uint16_t pot_raw){
    if(pot_raw >= ACTUATOR_LIMIT){
        return AT_CL_LIMIT;
    } else if (pot_raw <=ACT_OPEN_POS){
        return AT_OP_LIMIT;
    }
    return IND;
}

const uint8_t open_pin = 12;
const uint8_t close_pin = 13;

void actuator_off(){
    gpio_put(close_pin, false);
    gpio_put(open_pin, false);
}

void actuator(operator_t op){
    const uint16_t pot_raw = adc_read();
    const pot_state_t pot_state = get_pot_state(pot_raw);
    if(pot_state == AT_OP_LIMIT){
        gpio_put(open_pin, false);
    }

    if (pot_state == AT_CL_LIMIT){
        gpio_put(close_pin, false);
    }

    if(op == OPEN){
        gpio_put(open_pin, true);
    }
    if (op == CLOSE){
        gpio_put(close_pin, true);
    }
}