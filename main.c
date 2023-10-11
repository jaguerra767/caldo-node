#include <string.h>
#include <stdio.h>
#include "hardware/adc.h"
#include "pico/stdio.h"
#include "tusb.h"
#include "ring_buffer.h"
#include "read_scale.h"
#include "actuator.h"
#include "timing.h"



#define BUFFER_LEN 15
const uint8_t led_pin = 25;

typedef enum {
    READING,
    MSG_COMPLETE,
} read_process_t;


typedef enum {
    ACTUATOR,
    CALIBRATE,
    LOAD_CELL,
    TARE,
    UNKNOWN
} device_t;


typedef struct {
    device_t device;
    uint8_t device_id;
    operator_t operator;
} command_t;

read_process_t read_message(ring_buffer_t *buffer) {
    int ch = getchar_timeout_us(0);
    if (ch != PICO_ERROR_TIMEOUT) {
        ring_buffer_write(buffer, ch);
        if (ch == '\n') {
            return MSG_COMPLETE;
        }
    }
    return READING;
}



operator_t get_actuator_op_type(uint8_t op) {
    switch (op) {
        case 'o':
            return OPEN;
        case 'c':
            return CLOSE;
        default:
            return INVALID;
    }
}

device_t get_device_name(uint8_t dev) {
    switch (dev) {
        case 'c': return CALIBRATE;
        case 'l': return LOAD_CELL;
        case 't': return TARE;
        case 'a': return ACTUATOR;
        default: return UNKNOWN;
    }
}


command_t parse_msg(ring_buffer_t *buffer) {
    command_t result;
    result.device = get_device_name(ring_buffer_read(buffer));
    result.device_id = ring_buffer_read(buffer);
    if (result.device == ACTUATOR) {
        result.operator = get_actuator_op_type(ring_buffer_read(buffer));
    }else{
        result.operator = 0; //Used so that we don't have a null at operator
    }
    ring_buffer_read(buffer);//get rid of /n
    return result;
}

void setup_gpio() {
    stdio_init_all();
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    actuator_io_setup();
}



int main(void) {
    uint8_t msg_buffer[BUFFER_LEN];
    ring_buffer_t rb = {.buffer=msg_buffer, .write_index=0, .read_index=0, .max_length=BUFFER_LEN};
    memset(msg_buffer, 0, BUFFER_LEN);
    setup_gpio();
    setup_scales();
    while (!tud_cdc_connected()) {
        sleep_ms(1);
    }
    clock_t start_time = clock();


    for (;;) {
        const clock_t current_time = clock();
        const double exec_time_secs = (double)(current_time - start_time)/CLOCKS_PER_SEC;

        if(exec_time_secs < 0.2){
            gpio_put(led_pin, true);
        }else{
            gpio_put(led_pin, false);
        }
        if(exec_time_secs > 0.4){
            start_time = clock();
        }

        read_process_t rp = read_message(&rb);
        command_t cmd;
        scale_measure();
        if (rp == MSG_COMPLETE) {
            cmd = parse_msg(&rb);
            switch (cmd.device) {
                case LOAD_CELL:
                    send_weight();
                    break;
                case CALIBRATE:
                    calibrate();
                    break;
                case ACTUATOR:
                    actuator(cmd.operator);
                    printf("Actuator moved");
                    break;
                case TARE:
                    tare();
                    break;
                case UNKNOWN:
                    printf("Unknown action sent.\n");
                    break;
            }
        }
        sleep_us(100);
    }
}
