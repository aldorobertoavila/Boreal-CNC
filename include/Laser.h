#pragma once

#include <Arduino.h>

enum InlineMode
{
    OFF,
    ON
};

class Laser
{
public:
    Laser(uint8_t pwmPin);

    InlineMode getInlineMode();

    uint8_t getMaxPower();

    uint8_t getPower();

    bool isTurnOn();

    void setInlineMode(InlineMode mode);

    void setMaxPower(uint8_t maxPower);

    void setPower(uint8_t power);

    void turnOn();

    void turnOff();

private:
    uint8_t _maxPower;
    InlineMode _mode;
    uint8_t _pwmPin;
    uint8_t _power;
    bool _turnOn;
};