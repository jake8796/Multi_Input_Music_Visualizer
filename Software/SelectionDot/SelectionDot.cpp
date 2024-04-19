#include <SelectionDot.h>
#include <FastLED.h>

// Constructor
Dot::Dot(int x, int y, int sizeOfDot)
{
  _sizeOfDot = sizeOfDot;
  MoveDot(x, y);
}
// Move Dot to an XY Coordinate on the screen
void Dot::MoveDot(int x, int y)
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      lastCorners[i][j] = corners[i][j];
    }
  }
  int offset = 0;
  for (int i = 0; i < 4; i += 2)
  {
    corners[i][0] = x + offset;
    corners[i + 1][0] = x + offset;
    offset = _sizeOfDot;
  }
  for (int i = 0; i < 4; i += 2)
  {
    corners[i][1] = y;
    corners[i + 1][1] = y + _sizeOfDot;
  }
}
// Put the LEDS in the Dot
int SetLedsFromDotCorner(int corners[4][2], CRGB * leds)
{
  for (int i = corners[0][0]; i <= corners[3][0]; i++)
  {
    for (int j = corners[0][1]; j <= corners[3][1]; j++)
    {
      leds[ledMap[j][i]] = CRGB::Blue;
    }
  }
  return 0;
}

int ClearLedsFromDotCorner(int corners[4][2], CRGB * leds)
{
  for (int i = corners[0][0]; i <= corners[3][0]; i++)
  {
    for (int j = corners[0][1]; j <= corners[3][1]; j++)
    {
      leds[ledMap[j][i]] = CRGB::Black;
    }
  }
  return 0;
}