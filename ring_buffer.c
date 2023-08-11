//
// Created by Jorge Guerra on 8/9/23.
//
#include "ring_buffer.h"



int8_t ring_buffer_write(ring_buffer_t* rb, uint8_t data){
    uint32_t next = rb->write_index + 1; //next is where pointer will point to after following write
    if(next >= rb->max_length){
        next = 0;
    }
    if(next == rb->read_index){
        return -1; //Error Buffer is full
    }
    rb->buffer[rb->write_index] = data;
    rb->write_index = next;
    return 0;
}
//This could be bad
int8_t ring_buffer_read(ring_buffer_t* rb){
    uint8_t data = rb->buffer[rb->read_index];
    if(rb->read_index == rb->write_index){
        return -1;
    }

    if(rb->read_index == rb->max_length){
        rb->read_index = 0;
    }else{
        rb->read_index++;
    }
    return (int8_t)data; //There's no way this would be negative
}