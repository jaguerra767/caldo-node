//
// Created by Jorge Guerra on 8/7/23.
//

#ifndef CALDO_NODE_HX711_T_H
#define CALDO_NODE_HX711_T_H
#include <cstdint>
#include <optional>


enum class hx_711_gain {
    g128,
    g32,
    g64
};

class hx711_t {
public:
    hx711_t(std::uint8_t data_pin, std::uint8_t sck_pin, hx_711_gain gain){
        _data_pin = data_pin;
        _sck_pin = sck_pin;
        _gain = gain;
    }
    void power_up() const;
    void power_down() const;
    void set_gain();
    std::optional<std::uint32_t> read();
private:
    std::uint8_t _data_pin;
    std::uint8_t _sck_pin;
    hx_711_gain _gain;
    [[nodiscard]] bool is_ready() const;
    [[nodiscard]] std::uint8_t shift_in() const;
    void wait_for_ready();
};


#endif //CALDO_NODE_HX711_T_H
