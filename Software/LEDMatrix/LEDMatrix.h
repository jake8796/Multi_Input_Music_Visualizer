#ifndef LEDMatrix_h
#define LEDMatrix_h
#include "Arduino.h"
#include <FastLED.h>


// XY Coord structure
typedef struct coord
{
  uint32_t x;
  uint32_t y;
} coord;

typedef struct MatrixVars
{
  uint32_t NUM_OF_ROWS;
  uint32_t NUM_OF_COLS;
}MatrixVars;

class LEDMatrix
{
    public:
        LEDMatrix(MatrixVars);
        uint32_t CoordToLedIndex(coord);
        void LedIndexToCoord(uint32_t, coord*);
    private:
        
        CRGB *rgbValues;
        CRGB _color;
        uint32_t _NUM_OF_ROWS;
        uint32_t _NUM_OF_COLS;
    
}


#endif