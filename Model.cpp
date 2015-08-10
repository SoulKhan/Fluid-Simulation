#include "math.h"

struct ModelPoint
{
    float x;
    float y;
};

struct GridPoint
{
    int x;
    int y;
}

struct Vector
{
    float x;
    float y;
}

struct Field
{
    float** newState;
    float** oldState;
}

struct VectorField
{
    Vector x;
    Vector y;
}

inline float Lerp (float a, float b, float t)
{
    return a + (b - a) * t;
}

inline float BLerp (float a, float b, float c, float d, float tx, float ty)
{
    return Lerp (Lerp (b, c, tx), Lerp (a, d, tx), ty);
}

inline float GetPoint (float** field, GridPoint point)
{
    return field [point.x][point.y];
}

float BLerpPoint (float** field, ModelPoint point)
{
    GridPoint a = {0, 0},
              b = {0, 0},
              c = {0, 0},
              d = {0, 0};

    float gridCellSize = field [1][0] - field [0][0];

    float roundX = roundf (point.x / gridCellSize),
          roundY = roundf (point.y / gridCellSize);

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
        b.x = (int) roundX;
        c.x = (int) roundX;

        a.x = (int) roundX + 1;
        d.x = (int) roundX + 1;
    }

    float tx = (point.x - a.x) / (a.x - d.x);
    float ty = (point.y - c.y) / (d.y - c.y);

    return BLerp (field [a.x][a.y], field [b.x][b.y],
                  field [c.x][c.y], field [d.x][d.y], tx, ty);
}

float Advect (Field velocityField, Field fieldToBeChanged, GridPoint pointToBeChanged, float timeStep)
{
    ModelPoint oldPos  =            GetPoint (velocityField.newState, pointToBeChanged);
               oldPos -= timeStep * GetPoint (velocityField.oldState, pointToBeChanged);

    BLerpPoint (fieldToBeChanged.newState, oldPos);
}

float Jacobi (Field fieldX, float** b, GridPoint point, float alpha, float rBeta)
{
    float xLeft   = GetPoint (fieldX.oldState, point - {1, 0}),
          xRight  = GetPoint (fieldX.oldState, point + {1, 0}),
          xBottom = GetPoint (fieldX.oldState, point - {0, 1}),
          xTop    = GetPoint (fieldX.oldState, point + {0, 1});

    float bC = GetPoint (b, point);

    return (xLeft + xRight + xBottom + xTop + alpha * bC) * rBeta;
}

float Divergence (VectorField vectorField, GridPoint point)
{
    float vectorFieldLeft   = GetPoint (vectorField, point - {1, 0}),
          vectorFieldRight  = GetPoint (vectorField, point + {1, 0}),
          vectorFieldBottom = GetPoint (vectorField, point - {0, 1}),
          vectorFieldTop    = GetPoint (vectorField, point + {0, 1});

    return (vectorFieldRight.x - vectorFieldLeft.x) + (vectorFieldTop.y - vectorFieldBottom.y);
}

float Gradient (Field pressureField, Field velocityField, GridPoint point)
{
    float pressureLeft   = GetPoint (pressureField, point - {1, 0});
    float pressureRight  = GetPoint (pressureField, point + {1, 0});
    float pressureBottom = GetPoint (pressureField, point - {0, 1});
    float pressureTop    = GetPoint (pressureField, point + {0, 1});

    return GetPoint (velocityField, point) - {pressureRight - pressureLeft, pressureTop - pressureBottom};
}

inline float SetBoundary (GridPoint point, GridPoint offset, float scale, Field stateField)
{
    return scale * GetPoint (stateField, point + offset);
}
