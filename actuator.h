//
// Created by Jorge Guerra on 8/11/23.
//

#ifndef CALDO_NODE_ACTUATOR_H
#define CALDO_NODE_ACTUATOR_H
typedef enum {
    OPEN,
    CLOSE,
    INVALID
} operator_t;

void actuator_io_setup();
void actuator_limits();
void actuator_off();
uint16_t actuator(operator_t op);

#endif //CALDO_NODE_ACTUATOR_H