#include "LedMatrix.h"

int MatrixToLedString(coord coord, int length)
{

  int index;
  index = length * coord.x;
  if ((coord.x % 2) == 0)
  {
    index += coord.y;
  }
  else
  {
    index = (length - 1) - coord.y + index;
  }
  return index;
}

void LedStripToMatrix(int index, coord *coord)
{
  if ((index / 8) % 2 == 0)
  {
    coord->y = index % 8;
  }
  else
  {
    coord->y = 7 - (index % 8);
  }
  coord->x = index / 8;
}