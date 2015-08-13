#include "TXLib.h"
#include "Vector.h"

//============================================================================

const int MAX_HEIGHT = 1000;

COLORREF MAX_COLOR = RGB (255, 255, 255),
         MIN_COLOR = RGB (0, 0, 0);

//============================================================================

void render (float** heightField, Vector <int> fieldSize);

inline float lerp (float a, float b, float t);

COLORREF lerp (COLORREF colorA, COLORREF colorB, float t);

//============================================================================

void render (float** heightField, Vector <int> fieldSize)
{
    for (int x = 0; x < fieldSize.x; x ++)
    {
        for (int y = 0; y < fieldSize.y; y ++)
        {
            assert (x < fieldSize.x && x >= 0);
            assert (y < fieldSize.y && y >= 0);

            float lerpParameter = heightField [x][y] / MAX_HEIGHT;

            COLORREF currentColor = lerp (MIN_COLOR, MAX_COLOR, lerpParameter);

            txSetPixel (x, y, currentColor);
        }
    }
}

//----------------------------------------------------------------------------

inline float lerp (float a, float b, float t)
{
    return (a - (a - b) * t);
}

COLORREF lerp (COLORREF colorA, COLORREF colorB, float t)
{
    int redA   = txExtractColor (colorA, TX_RED),
        greenA = txExtractColor (colorA, TX_GREEN),
        blueA  = txExtractColor (colorA, TX_BLUE);

    int redB   = txExtractColor (colorB, TX_RED),
        greenB = txExtractColor (colorB, TX_GREEN),
        blueB  = txExtractColor (colorB, TX_BLUE);

    int redC   = (int) lerp ((float) redA, (float) redB, t);
    int greenC = (int) lerp ((float) greenA, (float) greenB, t);
    int blueC  = (int) lerp ((float) blueA, (float) blueB, t);

    return RGB (redC, greenC, blueC);
}
