//
// Created by Jorge Guerra on 8/17/23.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "comms.h"


size_t get_line(char* buffer, size_t len) {
    memset(buffer, 0, len);
    size_t i = 0;
    while(i < len){
        uint8_t c = getchar();
        if(c == '\n') break;
        buffer[i] = (char)c;
        i++;
    }
    return i + 1;
}