#ifndef GuitarInput_h
#define GuitarInput_h
#include "Arduino.h"

class GuitarInput 
{
    public:
        GuitarInput(uint8_t, uint32_t);

    private:
        uint8_t _pin;
        uint32_t _adcValue;
        uint32_t _averageADC;
        uint16_t _numberOfADCReadings;
        uint32_t _resolution;
    
}


#endif