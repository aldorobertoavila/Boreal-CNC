#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Resolution.h>
#include <Rotation.h>

// Emerging from Reset delay (ms)
#define RESET_DELAY 1

// PWM per Step delay (us)
#define STEP_DELAY 10

// PWM per Step delay (ms)
#define DEBUG_STEP_DELAY 1000

// Emerging from Sleep delay (ms)
#define SLEEP_DELAY 1

// Parallel Data Outputs
// Q0
#define DIR_OUT 0
// Q1
#define STEP_OUT 1
// Q2
#define SLEEP_OUT 2
// Q3
#define RESET_OUT 3
// Q4
#define MS3_OUT 4
// Q5
#define MS2_OUT 5
// Q6
#define MS1_OUT 6
// Q7
#define ENABLE_OUT 7

class MotorInterface
{
public:
    virtual void setEnable(bool enable);

    virtual void setReset(bool reset);

    virtual void setResolution(Resolution res);

    virtual void setRotation(Rotation rot);

    virtual void setSleep(bool sleep);

    virtual void step();
};

class BilateralMotorInterface : public MotorInterface
{
public:
    BilateralMotorInterface(MotorInterface &motorA, MotorInterface &motorB);

    void setEnable(bool enable) override;

    void setReset(bool reset) override;

    void setResolution(Resolution res) override;

    void setRotation(Rotation rot) override;

    void setSleep(bool sleep) override;

    void step() override;

private:
    MotorInterface &_motorA;
    MotorInterface &_motorB;
};

class ShiftRegisterMotorInterface : public MotorInterface
{
public:
    ShiftRegisterMotorInterface(SPIClass &spi, uint8_t csPin, uint8_t bitOrder = MSBFIRST, long spiClk = 20000000L);

    void setEnable(bool enable) override;

    void setReset(bool reset) override;

    void setResolution(Resolution res) override;

    void setRotation(Rotation rot) override;

    void setSleep(bool sleep) override;

    void step() override;

protected:
    void updateShiftOut();

private:
    long _spiClk;
    uint8_t _bitOrder;
    uint8_t _csPin;
    uint8_t _dataIn;
    SPIClass &_spi;
};