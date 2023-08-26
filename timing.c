//
// Created by Jorge Guerra on 8/26/23.
//
#include "timing.h"

clock_t clock() {
    return (clock_t) time_us_64() / 10000;
}