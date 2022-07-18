#include <Arduino.h>
#include <Unit.h>

Unit::Unit(float millis)
{
    this->_millis = millis;
}

uint16_t Unit::toSteps(float u, uint8_t steps)
{
    return (uint16_t) u * _millis * steps;
}