//
// Created by Jorge Guerra on 8/9/23.
//

#import <stdlib.h>
#import <pico/stdlib.h>
#define MAX_BUFFER 100

typedef struct  {
    uint8_t* const buffer;
    uint32_t read_index;
    uint32_t write_index;
    const uint32_t max_length;
}ring_buffer_t;

uint8_t ring_buffer_write(ring_buffer_t* rb, uint8_t data){
    uint32_t next;
    next = rb->write_index + 1; //next is where pointer will point to after following write
    if(next >= rb->max_length){
        next = 0;
    }
    if(next == rb->read_index){
        return -1; //Error Buffer is full
    }
    rb->buffer[next] = data;
    rb->write_index = next;
    return 0;
}