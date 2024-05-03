#line 1 "C:\\Users\\CHALLENGER\\Documents\\KeXieData\\Project\\12864\\src\\BSP\\Pins.hpp"
#pragma once

#include "Arduino.h"

struct Pins {
    static constexpr std::uint8_t BTN_0 = 0;
    static constexpr std::uint8_t LCD_RESET = 1;
    static constexpr std::uint8_t CHARGE_STATE = 2;
    static constexpr std::uint8_t VBAT_SCENE = 3;
    static constexpr std::uint8_t BACKLIGHT_CONTROL = 4;
    static constexpr std::uint8_t LCD_CONTROL = 5;
    static constexpr std::uint8_t LCD_CLK = 6;
    static constexpr std::uint8_t LCD_DIN = 7;
    static constexpr std::uint8_t LCD_CS = 8;
    static constexpr std::uint8_t BTN_1 = 9;
    static constexpr std::uint8_t LED = 10;

    inline static void init() {
        gpio_hold_dis(gpio_num_t::GPIO_NUM_4);
        gpio_hold_dis(gpio_num_t::GPIO_NUM_5);

        pinMode(BTN_0, INPUT_PULLUP);
        pinMode(LCD_RESET, OUTPUT);
        pinMode(CHARGE_STATE, INPUT_PULLUP);
        pinMode(VBAT_SCENE, ANALOG);
        pinMode(BACKLIGHT_CONTROL, OUTPUT);
        pinMode(LCD_CONTROL, OUTPUT);
        pinMode(LCD_CLK, OUTPUT);
        pinMode(LCD_DIN, OUTPUT);
        pinMode(LCD_CS, OUTPUT);
        pinMode(BTN_1, INPUT_PULLUP);
        pinMode(LED, OUTPUT);

        digitalWrite(BACKLIGHT_CONTROL, LOW);
        digitalWrite(LCD_CONTROL, HIGH);
        digitalWrite(LED, LOW);
    }
};