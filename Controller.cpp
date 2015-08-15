#include "TXLib.h"
#include "Model.cpp"
#include "Viewer.cpp"

//============================================================================

const int SIZE_X     = 127,
          SIZE_Y     = 127;

const float TIME_STEP  = 0.01f,
            SPACE_STEP = 0.1f,
            ALPHA      = 0.25f,
            VIS        = 0.001f;

//============================================================================

float** initialize2dArr (Vector <int> arrSize);

void free2dArr (float** arr, int sizeX);

void equateArrs (float** arrA, float** arrB, Vector <int> arrSize);

//----------------------------------------------------------------------------

//inline float lerp (float a, float b, float t);

//COLORREF lerp (COLORREF colorA, COLORREF colorB, float t);

//----------------------------------------------------------------------------

void adjustHeight (float** heightField, Vector <int> fieldSize, float adjustRadius, float newHeight);

//============================================================================

int main ()
{
    txCreateWindow (SIZE_X, SIZE_Y);

    Vector <int> dlv (SIZE_X, SIZE_Y);

    float** currentHeight       = initialize2dArr (dlv);
    float** preCurrentHeight    = initialize2dArr (dlv);
    float** prePreCurrentHeight = initialize2dArr (dlv);

    int sleep = 0;

    int draw = 0;

    float adjustRadius = 10;
    float newHeight = -1;

    while (!GetAsyncKeyState (VK_ESCAPE))
    {
        computeNextHeight (currentHeight, preCurrentHeight, prePreCurrentHeight,
                           dlv, SPACE_STEP, TIME_STEP, ALPHA, VIS);

        if (txMouseButtons () == 1)
        {
            adjustHeight (currentHeight, dlv, adjustRadius, newHeight);
        }

        equateArrs (prePreCurrentHeight, preCurrentHeight, dlv);
        equateArrs (preCurrentHeight,    currentHeight,    dlv);

        if (draw % 15 == 0)
        {
            render (currentHeight, dlv);

            draw = 0;
        }

        txSleep (sleep);
        draw ++;
    }

    free2dArr (currentHeight,       SIZE_X);
    free2dArr (preCurrentHeight,    SIZE_X);
    free2dArr (prePreCurrentHeight, SIZE_X);

    return 0;
}

//============================================================================

float** initialize2dArr (Vector <int> arrSize)
{
    float ** arr = (float**) calloc (arrSize.x, sizeof (float*));

    assert (arr);

    for (int x = 0; x < arrSize.x; x ++)
    {
        assert (x < arrSize.x && x >= 0);

        arr [x] = (float*) calloc (arrSize.y, sizeof (float));

        assert (arr [x]);
    }

    return arr;
}

void free2dArr (float** arr, int sizeX)
{
    for (int x = 0; x < sizeX; x ++)
    {
        assert (x < sizeX && x >= 0);

        free (arr [x]);
    }

    assert (arr);

    free (arr);
}

void equateArrs (float** arrA, float** arrB, Vector <int> arrSize)
{
    for (int x = 0; x < arrSize.x; x ++)
    {
        for (int y = 0; y < arrSize.y; y ++)  // memcpy
        {
            assert (x < arrSize.x && x >= 0);
            assert (y < arrSize.y && y >= 0);

            arrA [x][y] = arrB [x][y];
        }
    }
}

//============================================================================

void adjustHeight (float** heightField, Vector <int> fieldSize, float adjustRadius, float newHeight)
{
    int mouseX = txMouseX (),
        mouseY = txMouseY ();

    for (int x = 0; x < fieldSize.x; x++)
    {
        for (int y = 0; y < fieldSize.y; y++)
        {
            float distanceFromCenter = sqrt (pow (mouseX - x, 2) + pow (mouseY - y, 2));

            assert (x < fieldSize.x && x >= 0);
            assert (y < fieldSize.y && y >= 0);

            if (distanceFromCenter <= adjustRadius)
            {
                heightField [x][y] += newHeight;

                if (heightField [x][y] > MAX_HEIGHT) heightField [x][y] = MAX_HEIGHT;
            }
        }
    }
}
