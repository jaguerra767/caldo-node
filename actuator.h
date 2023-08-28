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

typedef enum {
    AT_OP_LIMIT,
    AT_CL_LIMIT,
    IND
}pot_state_t;

void actuator_io_setup();
pot_state_t actuator_limits();
void timeout_pins();
uint16_t actuator(operator_t op);

#endif //CALDO_NODE_ACTUATOR_H
