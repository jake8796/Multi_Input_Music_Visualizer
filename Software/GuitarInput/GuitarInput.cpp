#include "GuitarInput.h"

uint32_t adcValue;
uint32_t averageADC;
uint16_t numberOfADCReadings;

// Contructor function
GuitarInput::GuitarInput(int pin, uint32_t resolution = 12)
{

    pinMode(pin, INPUT);

    _pin = pin;


}

// Get the ADC input and convert it to an LED value
uint32_t GuitarInput:: GetInput() 
{

    

}