//
// Created by Jorge Guerra on 8/7/23.
//

#include "hx711_t.h"
#include "pico/stdlib.h"

bool hx711_t::is_ready() const {
    return !gpio_get(_data_pin);
}

void hx711_t::power_up() const {
    gpio_put(_sck_pin, false);
}

void hx711_t::power_down() const {
    gpio_put(_sck_pin, true);
}

void hx711_t::set_gain() {

}

std::optional<std::uint32_t> hx711_t::read() {
    if(!is_ready()){
        return {};
    }
    return 0;
}

void hx711_t::wait_for_ready() {

}

std::uint8_t hx711_t::shift_in() const {
    return 0;
}


