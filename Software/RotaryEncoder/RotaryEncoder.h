#ifndef RotaryEncoder_h
#define RotaryEncoder_h
#define CLOCKWISE 1
#define COUNTERCLOCKWISE 2
#include "Arduino.h"
class RotaryEncoder 
{
    public:
        RotaryEncoder(int pinA, int pinB, int pinSw, int pinRst);
        int SwitchPressed();
        int SwitchReleased();
        int Rotate(void)
        int GetLastRotationState(void);
        int GetRotationCounter(void);

    private:
        int _pinA;
        int _pinB;
        int _pinSw;
        unsigned long _swPressed;
        unsigned long _swReleased;
    
}

#endif