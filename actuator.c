////
//// Created by Jorge Guerra on 8/11/23.
////
#include <pico/printf.h>
#include "actuator.h"
#include "hardware/adc.h"
#include "pico/stdio.h"
#include "timing.h"

#define ACTUATOR_LIMIT 4000
#define ACT_OPEN_POS 20


const uint8_t open_pin = 12;
const uint8_t close_pin = 13;

clock_t open_pin_on_time = {0};
clock_t close_pin_on_time = {0};

void actuator_io_setup(){
    gpio_init(open_pin);
    gpio_init(close_pin);
    gpio_set_dir(open_pin, GPIO_OUT);
    gpio_set_dir(close_pin, GPIO_OUT);
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

void actuator_off(){
    gpio_put(close_pin, false);
    gpio_put(open_pin, false);
}



pot_state_t actuator_limits(){
    const uint16_t pot_raw = adc_read();
    const pot_state_t pot_state = get_pot_state(pot_raw);
    if(pot_state == AT_OP_LIMIT){
        gpio_put(open_pin, false);
    }
    if (pot_state == AT_CL_LIMIT){
        gpio_put(close_pin, false);
    }
    return pot_state;
}

void timeout_pins(){
    const clock_t current_time = clock();
    const double open_pin_on_time_secs = (double)(current_time - open_pin_on_time)/CLOCKS_PER_SEC;
    const double close_pin_on_time_secs = (double)(current_time - close_pin_on_time)/CLOCKS_PER_SEC;
    if(open_pin_on_time_secs > 2.3){
        gpio_put(open_pin, false);
    }
    if(close_pin_on_time_secs > 2.3){
        gpio_put(close_pin, false);
    }
}


uint16_t actuator(operator_t op){
    const pot_state_t pot_state =  actuator_limits();
    if(op == OPEN && pot_state != AT_OP_LIMIT){
        gpio_put(open_pin, true);
        gpio_put(close_pin, false);
        open_pin_on_time = clock();
        printf("opening\n");
    }
    if (op == CLOSE && pot_state != AT_CL_LIMIT){
        gpio_put(close_pin, true);
        gpio_put(open_pin, false);
        close_pin_on_time = clock();
        printf("closing\n");
    }
    return adc_read();
}