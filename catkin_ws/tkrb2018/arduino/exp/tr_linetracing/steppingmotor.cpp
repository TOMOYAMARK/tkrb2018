#include <Arduino.h>
#include "steppingmotor.h"

SteppingMotor::SteppingMotor(steppingMotorPinAssign pin, float stepAngle) {
    this->pin = pin;
    this->stepAngle = stepAngle;
    pinMode(pin.cw, OUTPUT);
    pinMode(pin.ccw, OUTPUT);
}
