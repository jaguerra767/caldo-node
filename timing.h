//
// Created by Jorge Guerra on 8/26/23.
//

#ifndef CALDO_NODE_TIMING_H
#define CALDO_NODE_TIMING_H
#include <time.h>
#include <hardware/timer.h>


inline clock_t clock() {
    return (clock_t) time_us_64() / 10000;
}

#endif //CALDO_NODE_TIMING_H
