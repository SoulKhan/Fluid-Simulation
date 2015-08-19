#include "assert.h"
#include "Math.h"
#include "Vector.h"

//============================================================================

#define DONT_USE_ISLAND

//============================================================================

inline float Lerp  (float a, float b, float t);

inline float BLerp (float a, float b, float c, float d, float tx, float ty);


float BLerpPoint (float scalarField, Vector <float> point, Vector <int> fieldSize);


Vector <float> intToFloat (Vector <int> vec);

//----------------------------------------------------------------------------

float Advect (Vector <float> ** velocityField, float ** quantityField, Vector <int> coords, float timeStep, Vector <int> fieldSize);


/*Vector <float> Jacobi (Vector <float> ** vectorFieldOfValues, Vector <int> coords,
                       Vector <float> ** vectorFieldOfConstants, float alpha, float beta);

float Jacobi (float ** scalarFieldOfValues, Vector <int> coords,
              float ** scalarFieldOfConstants, float alpha, float beta);*/


/*template <typename Type>
Vector <Type> ** subtractPressureFromVelocity (float ** scalarField, Vector <int> fieldSize,
                                               Vector <float> ** velocityField, float spaceStep);*/


//inline float SetBoundary (Vector <int> currentPoint, Vector <int> offset, float scale, Vector <float> ** stateField);

void SetBoundary (Vector <float> ** stateField, Vector <int> fieldSize);

//----------------------------------------------------------------------------

float computeNextHeight (float ** scalarFieldA, float ** scalarFieldB, float ** scalarFieldC,
                         Vector <int> fieldSize, float spaceStep, float timeStep, float alpha, float vis);

//============================================================================

inline float Lerp (float a, float b, float t)
{
    return a + (b - a) * t;
}

inline float BLerp (float a, float b, float c, float d, float tx, float ty)
{
    return Lerp (Lerp (b, c, tx), Lerp (a, d, tx), ty);
}

float BLerpPoint (float ** scalarField, Vector <float> point, Vector <int> fieldSize);
float BLerpPoint (float ** scalarField, Vector <float> point, Vector <int> fieldSize)
{
    Vector <int> a, b, c, d;

    float gridCellSize = scalarField [1][0] - scalarField [0][0];

    int roundX = (int) roundf (point.x / gridCellSize),
        roundY = (int) roundf (point.y / gridCellSize);

    if (roundX >= point.x)
    {
        c.x = (int) roundX;
        d.x = (int) roundX;

        b.x = (int) roundX - 1;
        a.x = (int) roundX - 1;
    }

    else
    {
        c.x = (int) roundX + 1;
        d.x = (int) roundX + 1;

        b.x = (int) roundX;
        a.x = (int) roundX;
    }

    if (roundY >= point.y)
    {
        b.x = (int) roundY - 1;
        c.x = (int) roundY - 1;

        a.x = (int) roundY;
        d.x = (int) roundY;
    }

    else
    {
        b.x = roundX;
        c.x = roundX;

        a.x = roundX + 1;
        d.x = roundX + 1;
    }

    float tx = (float) (((double)point.x - a.x) / (a.x - d.x));
    float ty = (float) (((double)point.y - c.y) / (d.y - c.y));

    assert (a.x < fieldSize.x && a.x >= 0);
    assert (a.y < fieldSize.y && a.y >= 0);

    assert (b.x < fieldSize.x && b.x >= 0);
    assert (b.y < fieldSize.y && b.y >= 0);

    assert (c.x < fieldSize.x && c.x >= 0);
    assert (c.y < fieldSize.y && c.y >= 0);

    assert (d.x < fieldSize.x && d.x >= 0);
    assert (d.y < fieldSize.y && d.y >= 0);

    return BLerp (scalarField [a.x][a.y], scalarField [b.x][b.y],
                  scalarField [c.x][c.y], scalarField [d.x][d.y], tx, ty);
}

//----------------------------------------------------------------------------

Vector <float> intToFloat (Vector <int> vec)
{
    Vector <float> tmpVec;

    tmpVec.x = (float) vec.x;
    tmpVec.y = (float) vec.y;

    return tmpVec;
}

//============================================================================

float Advect (Vector <float> ** velocityField, float ** quantityField, Vector <int> coords, float timeStep, Vector <int> fieldSize)
{
    assert (coords.x < fieldSize.x && coords.x >= 0);
    assert (coords.y < fieldSize.y && coords.y >= 0);

    Vector <float> oldPos = intToFloat (coords) - velocityField [coords.x][coords.y] * timeStep;

    return BLerpPoint (quantityField, oldPos, fieldSize);
}

//----------------------------------------------------------------------------

/*Vector <float> Jacobi (Vector <float> ** vectorFieldOfValues, Vector <int> coords,
                       Vector <float> ** vectorFieldOfConstants, float alpha, float beta)
{
    Vector <float> newVectorOfValue = Divergence (vectorFieldOfValues, coords, beta / 2);

    newVectorOfValue += alpha * vectorFieldOfConstants [coords.x][coords.y] / beta;

    return newVectorOfValue;
}

float Jacobi (float ** scalarFieldOfValues, Vector <int> coords,
              float ** scalarFieldOfConstants, float alpha, float beta)
{
    float newScalarValue = Divergence (scalarFieldOfValues, coords, beta / 2);

    newScalarValue += alpha * scalarFieldOfConstants [coords.x][coords.y] / beta;

    return newScalarValue;
}*/

//----------------------------------------------------------------------------

/*template <typename Type>
Vector <Type> ** subtractPressureFromVelocity (float ** scalarField, Vector <int> fieldSize,
                                               Vector <float> ** velocityField, float spaceStep)
{
    Vector <float> ** newVelocityField;

    for (int x = 0; x < fieldSize.x; x ++)
    {
        for (int y = 0; y < fieldSize.y; y ++)
        {
            newVelocityField [x][y] = velocityField [x][y] - GradientForField (scalarField, fieldSize, spaceStep) [x][y];
        }
    }

    return newVelocityField;
}*/

//----------------------------------------------------------------------------

/*inline Vector <float> SetBoundary (Vector <int> currentPoint, Vector <int> offset, float scale, Vector <float> ** stateField)
{
    return scale * stateField [currentPoint.x + offset.x][currentPoint.y + offset.y];
}*/

void SetBoundary (float ** scalarField, Vector <int> fieldSize);
void SetBoundary (float ** scalarField, Vector <int> fieldSize)
{
    for (int x = 0; x < fieldSize.x;  x ++)
    {
        assert (x < fieldSize.x && x >= 0);
        assert (fieldSize.y - 1 < fieldSize.y && fieldSize.y - 1 >= 0);

        scalarField [x][fieldSize.y - 1] = 0;
        scalarField [x][0]               = 0;
    }

    for (int y = 0; y < fieldSize.y; y ++)
    {
        assert (y < fieldSize.y && y >= 0);
        assert (fieldSize.x - 1 < fieldSize.y && fieldSize.x - 1 >= 0);

        scalarField [fieldSize.x - 1][y] = 0;
        scalarField [0]              [y] = 0;
    }
}

float computeNextHeight (float ** scalarFieldA, float ** scalarFieldB, float ** scalarFieldC,
                         Vector <int> fieldSize, float spaceStep, float timeStep, float alpha, float vis)
{
    float maxHeight = 0;

    for (int x = 1; x + 1 < fieldSize.x; x ++)
    {
        for (int y = 1; y + 1 < fieldSize.y; y ++)
        {
            #ifdef USE_ISLAND

            float distanceFromCenter = (float) sqrt (pow (fieldSize.x / 2 - x, 2) + pow (fieldSize.y / 2 - y, 2));

            if (distanceFromCenter >= 20)
            {
                assert (x < fieldSize.x && x >= 0);
                assert (y < fieldSize.y && y >= 0);

                scalarFieldA [x][y] += alpha * Laplacian (scalarFieldB, Vector <int> (x, y), spaceStep);
                scalarFieldA [x][y] *= powf (timeStep, 2);

                scalarFieldA [x][y] += (2.0f - 2 * vis) * scalarFieldB [x][y];
                scalarFieldA [x][y] -= (1.0f - vis) *  scalarFieldC [x][y];

            }

            #endif

            #ifdef DONT_USE_ISLAND

            assert (x < fieldSize.x && x >= 0);
            assert (y < fieldSize.y && y >= 0);

            scalarFieldA [x][y] += alpha * Laplacian (scalarFieldB, Vector <int> (x, y), spaceStep);
            scalarFieldA [x][y] *= powf (timeStep, 2);

            scalarFieldA [x][y] += (2.0f - 2 * vis) * scalarFieldB [x][y];
            scalarFieldA [x][y] -= (1.0f - vis) *  scalarFieldC [x][y];

            if (scalarFieldA [x][y] > maxHeight) maxHeight = scalarFieldA [x][y];

            #endif
        }
    }

    return maxHeight;
}
