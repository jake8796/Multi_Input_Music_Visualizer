#ifndef GuitarInput_h
#define GuitarInput_h
#include "Arduino.h"

class GuitarInput 
{
    public:
        GuitarInput(uint8_t, uint32_t);

    private:
        uint8_t _pin;
        
        uint32_t _resolution;
    
}


#endif