#include "RotaryEncoder.h"

static unsigned long swTimePressed;
static int rotationCounter;
static int lastRotation;

// Contructor function
RotaryEncoder::RotaryEncoder(int pinA, int pinB, int pinSw)
{

    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);
    pinMode(pinSw, INPUT);

    _pinA = pinA;
    _pinB = pinB;
    _pinSw = pinSw;

}
// Call on the falling edge of the switch being pressed
int RotaryEncoder:: SwitchPressed() 
{

    _swPressed = millis();
    return 0;

}
//Call on the rising edge of the switch being pressed
int RotaryEncoder:: SwitchReleased() 
{

    _swReleased = millis();
    swTimePressed = _swReleased - _swPressed;
    return 0;

}

//Should be used on rising edge of one pin
int RotaryEncoder:: Rotate()
{
    bool pinAState = digitalRead(_pinA);
    bool pinBState = digitalRead(_pinB);
    if(pinAState != pinBState){

        rotationCounter++;
        lastRotation = CLOCKWISE;

    }else{

        rotationCounter--;
        lastRotation = COUNTERCLOCKWISE;

    }
    return lastRotation;
}

int RotaryEncoder::GetLastRotationState(void)
{
    return lastRotation;
}

int RotaryEncoder::GetRotationCounter(void)
{
    return rotationCounter;
}
