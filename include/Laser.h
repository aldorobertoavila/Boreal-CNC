#pragma once

#include <Arduino.h>

enum InlineMode
{
    ON,
    OFF
};

class Laser 
{
public:
    Laser(uint8_t pwmPin);

    InlineMode getInlineMode();

    uint8_t getMaxPower();

    uint8_t getPower();

    void setInlineMode(InlineMode mode);

    void setMaxPower(uint8_t maxPower);

    void setPower(uint8_t power);

    void turnOn();

    void turnOff();

private:
    uint8_t _pwmPin;
    uint8_t _power;
    uint8_t _maxPower;
    InlineMode _mode;
};