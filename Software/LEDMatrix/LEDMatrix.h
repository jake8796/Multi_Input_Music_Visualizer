#ifndef LEDMatrix_h
#define LEDMatrix_h
#include "Arduino.h"
#include <FastLED.h>


// XY Coord structure
typedef struct coord
{
  int x;
  int y;
} coord;

class LEDMatrix
{
    public:
        LEDMatrix(uint32_t NUM_OF_ROWS, uint32_t NUM_OF_COLUMNS)

    private:
        int CoordToLedIndex(coord coord, int length);
        void LedIndexToCoord(int index, coord *coord);
        CRGB *rgbValues;
        uint32_t _resolution;
    
}


#endif