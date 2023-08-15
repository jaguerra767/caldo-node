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
#include "hardware/adc.h"
#include "pico/stdio.h"
#include "tusb.h"
#include "ring_buffer.h"
#include "read_scale.h"
#include "actuator.h"

#define BUFFER_LEN 100
const uint8_t led_pin = 25;

typedef enum {
    READING,
    MSG_COMPLETE,
    INVALID_MSG
} read_process_t;

typedef struct {
    read_process_t status;
    size_t length;
} message_status_t;

typedef struct {
    read_process_t status;
    size_t length;
    uint8_t data[BUFFER_LEN];
} message_t;

read_process_t get_full_message_status(size_t message_len) {
    if (message_len <= 1) {
        return INVALID_MSG;
    }
    return MSG_COMPLETE;
}

typedef enum {
    ACTUATOR,
    LOAD_CELL,
    UNKNOWN
} device_t;


typedef struct {
    device_t device;
    uint8_t device_id;
    operator_t operator;
} command_t;

read_process_t read_message(ring_buffer_t *buffer) {
    int ch = getchar_timeout_us(0);
    if (ch != -1) {
        //Leaving '\n' out of the buffer because we process messages right away... revisit if this sucks
        if (ch == '\n') {
            return MSG_COMPLETE;
        }
        ring_buffer_write(buffer, ch);
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
        case 'l':
            return LOAD_CELL;
        case 'a':
            return ACTUATOR;
        default:
            return UNKNOWN;
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
//    printf("Device name: %d, Device ID: %d, Operator: %d\n", result.device, result.device_id, result.operator);
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
    tare();

    for (;;) {
        actuator_limits();
        read_process_t rp = read_message(&rb);
        command_t cmd;
        if (rp == MSG_COMPLETE) {
            cmd = parse_msg(&rb);
            if (cmd.device == LOAD_CELL) {
                scale_measure();
            }
            if (cmd.device == ACTUATOR){
                uint16_t  pot_val = actuator(cmd.operator);
                printf("Current Pot value: %d", pot_val);
            }
        }
        gpio_put(led_pin, true);
        sleep_ms(200);
        gpio_put(led_pin, false);
        sleep_ms(200);
    }
}
