//
// Created by Jorge Guerra on 8/10/23.
//

#ifndef CALDO_NODE_RING_BUFFER_H
#define CALDO_NODE_RING_BUFFER_H
#include <pico/stdlib.h>

typedef struct  {
    uint8_t* const buffer;
    uint32_t read_index;
    uint32_t write_index;
    const uint32_t max_length;
}ring_buffer_t;


int8_t ring_buffer_write(ring_buffer_t* rb, uint8_t data);
int8_t ring_buffer_read(ring_buffer_t* rb);

#endif //CALDO_NODE_RING_BUFFER_H
