#include <MotorInterface.h>

// #define DEBUG_MODE

BilateralMotorInterface::BilateralMotorInterface(MotorInterface &motorA, MotorInterface &motorB) : _motorA(motorA), _motorB(motorB)
{
    this->_motorA = motorA;
    this->_motorB = motorB;
}

void BilateralMotorInterface::setEnable(bool enable)
{
    _motorA.setEnable(enable);
    _motorB.setEnable(enable);
}

void BilateralMotorInterface::setReset(bool reset)
{
    _motorA.setReset(reset);
    _motorB.setReset(reset);
}

void BilateralMotorInterface::setResolution(Resolution res)
{
    _motorA.setResolution(res);
    _motorB.setResolution(res);
}

void BilateralMotorInterface::setRotation(Rotation rot)
{
    _motorA.setRotation(rot);

    if(rot == CLOCKWISE)
    {
        _motorB.setRotation(COUNTERCLOCKWISE);
    }
    else
    {
        _motorB.setRotation(CLOCKWISE);
    }
}

void BilateralMotorInterface::setSleep(bool sleep)
{
    _motorA.setSleep(sleep);
    _motorB.setSleep(sleep);
}

void BilateralMotorInterface::step()
{
    _motorA.step();
    _motorB.step();
}

ShiftRegisterMotorInterface::ShiftRegisterMotorInterface(SPIClass &spi, uint8_t csPin, uint8_t bitOrder, long spiClk) : _spi(spi)
{
    this->_spiClk = spiClk;
    this->_bitOrder = bitOrder;
    this->_csPin = csPin;
    this->_spi = spi;

    bitWrite(_dataIn, ENABLE_OUT, LOW);
    bitWrite(_dataIn, RESET_OUT, HIGH);
    bitWrite(_dataIn, SLEEP_OUT, HIGH);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::setEnable(bool enable)
{
    bitWrite(_dataIn, ENABLE_OUT, enable);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::setReset(bool reset)
{
    bitWrite(_dataIn, RESET_OUT, reset);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::setResolution(Resolution res)
{
    switch (res)
    {
    case FULL:
        bitWrite(_dataIn, MS1_OUT, LOW);
        bitWrite(_dataIn, MS2_OUT, LOW);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case HALF:
        bitWrite(_dataIn, MS1_OUT, HIGH);
        bitWrite(_dataIn, MS2_OUT, LOW);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case QUARTER:
        bitWrite(_dataIn, MS1_OUT, LOW);
        bitWrite(_dataIn, MS2_OUT, HIGH);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case EIGHTH:
        bitWrite(_dataIn, MS1_OUT, HIGH);
        bitWrite(_dataIn, MS2_OUT, HIGH);
        bitWrite(_dataIn, MS3_OUT, LOW);
        updateShiftOut();
        break;
    case SIXTEENTH:
        bitWrite(_dataIn, MS1_OUT, HIGH);
        bitWrite(_dataIn, MS2_OUT, HIGH);
        bitWrite(_dataIn, MS3_OUT, HIGH);
        updateShiftOut();
        break;
    default:
        break;
    }
}

void ShiftRegisterMotorInterface::setRotation(Rotation rot)
{
    bitWrite(_dataIn, DIR_OUT, rot);
    updateShiftOut();
}

void ShiftRegisterMotorInterface::setSleep(bool sleep)
{
    bitWrite(_dataIn, SLEEP_OUT, sleep);
    updateShiftOut();
    delay(SLEEP_DELAY);
}

void ShiftRegisterMotorInterface::step()
{
    bitWrite(_dataIn, STEP_OUT, HIGH);
    updateShiftOut();

    #ifdef DEBUG_MODE
        delay(DEBUG_STEP_DELAY);
    #else
        delayMicroseconds(STEP_DELAY);
    #endif
    
    bitWrite(_dataIn, STEP_OUT, LOW);
    updateShiftOut();

    #ifdef DEBUG_MODE
        delay(DEBUG_STEP_DELAY);
    #endif
}

void ShiftRegisterMotorInterface::updateShiftOut()
{
    _spi.beginTransaction(SPISettings(_spiClk, _bitOrder, SPI_MODE0));
    digitalWrite(_csPin, LOW);
    _spi.transfer(_dataIn);
    digitalWrite(_csPin, HIGH);
    _spi.endTransaction();
}
