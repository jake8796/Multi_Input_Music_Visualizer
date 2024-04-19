#include "LedMatrix.h"

/***********************************************************
The LED matrix is structured in a snaking pattern as shown below
0 | 15 | 16 |...
1 | 14 | 17 |...
. | .  | .  |...
7 | 8  | 23 |...
************************************************************/

int LEDMatrix::LEDMatrix(MatrixVars)
{
    _NUM_OF_ROWS = MatrixVars.NUM_OF_ROWS;
    _NUM_OF_COLS = MatrixVars.NUM_OF_COLS;
}

// Converts a coordinate to an index on the LED Matrix
uint32_t LEDMatrix::CoordToLedIndex(coord coord)
{

  uint32_t index;
  index = _NUM_OF_ROWS * coord.x;
  if ((coord.x % 2) == 0)
  {
    index += coord.y;
  }
  else
  {
    index = (_NUM_OF_ROWS - 1) - coord.y + index;
  }
  return index;
}

// Converts an index on the LED Matrix to a coordinate
void LEDMatrix::LedIndexToCoord(uint32_t index, coord *coord)
{
  if ((index / _NUM_OF_ROWS) % 2 == 0)
  {
    coord->y = index % 8;
  }
  else
  {
    coord->y = (_NUM_OF_ROWS-1) - (index % _NUM_OF_ROWS);
  }
  coord->x = index / _NUM_OF_ROWS;
}