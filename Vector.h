#ifndef _VECTOR_H_
#define _VECTOR_H_

//============================================================================

template <typename Type = float>
struct Vector
{
    Type x, y;

    Vector ();
    Vector (Type componentX, Type componentY);
};


template <typename Type>
Vector <Type> :: Vector () :
    x (0),
    y (0)
{}

template <typename Type>
Vector <Type> :: Vector (Type componentX, Type componentY) :
    x (componentX),
    y (componentY)
{}

//============================================================================

template <typename Type>
inline Vector <Type> & operator += (Vector <Type> & vecA, const Vector <Type> & vecB);

template <typename Type>
inline Vector <Type> operator + (const Vector <Type> & vecA, const Vector <Type> & vecB);


template <typename Type>
inline Vector <Type> & operator -= (Vector <Type> & vecA, const Vector <Type> & vecB);

template <typename Type>
inline Vector <Type> operator - (const Vector <Type> & vecA, const Vector <Type> & vecB);


template <typename Type>
inline Vector <Type> & operator *= (Vector <Type> & vecA, const Vector <Type> & vecB);

template <typename Type>
inline Vector <Type> operator * (const Vector <Type> & vecA, const Vector <Type> & vecB);


template <typename TypeA, typename TypeB>
inline Vector <TypeA> & operator *= (Vector <TypeA> & vec, const TypeB val);

template <typename TypeA, typename TypeB>
inline Vector <TypeA> operator * (const Vector <TypeA> & vec, const TypeB val);


template <typename TypeA, typename TypeB>
inline Vector <TypeB> & operator *= (const TypeA val, Vector <TypeB> & vec);

template <typename TypeA, typename TypeB>
inline Vector <TypeB> operator * (const TypeA val, const Vector <TypeB> & vec);


template <typename Type>
inline Vector <Type> & operator /= (Vector <Type> & vec, const Type val);

template <typename Type>
inline Vector <Type> operator / (const Vector <Type> & vec, const Type val);

//----------------------------------------------------------------------------

template <typename Type>
Vector <Type> Laplacian (Vector <Type> ** vectorField, Vector <int> coords, float spaceStep);

template <typename Type>
Type Laplacian (Type ** scalarField, Vector <int> coords, float spaceStep);


template <typename Type>
Vector <Type> ** LaplacianForField (Vector <Type> ** vectorField, Vector <int> fieldSize, float spaceStep);

template <typename Type>
Type ** LaplacianForField (Type ** scalarField, Vector <int> fieldSize, float spaceStep);

//----------------------------------------------------------------------------

template <typename Type>
Type Divergence (Vector <Type> ** vectorField, Vector <int> coords, float spaceStep);

template <typename Type>
Type ** DivergenceForField (Vector <Type> ** vectorField, Vector <int> fieldSize, float spaceStep);

//----------------------------------------------------------------------------

template <typename Type>
Vector <Type> Gradient (Type ** scalarField, Vector <int> coords, float spaceStep);

template <typename Type>
Vector <Type> ** GradientForField (Type ** scalarField, Vector <int> fieldSize, float spaceStep);

//============================================================================

template <typename Type>
inline Vector <Type> & operator += (Vector <Type> & vecA, const Vector <Type> & vecB)
{
    vecA.x += vecB.x;
    vecA.y += vecB.y;

    return vecA;
}

template <typename Type>
inline Vector <Type> operator + (const Vector <Type> & vecA, const Vector <Type> & vecB)
{
    Vector <Type> vecC = vecA;

    return vecC += vecB;
}

//----------------------------------------------------------------------------

template <typename Type>
inline Vector <Type> & operator -= (Vector <Type> & vecA, const Vector <Type> & vecB)
{
    vecA.x -= vecB.x;
    vecA.y -= vecB.y;

    return vecA;
}

template <typename Type>
inline Vector <Type> operator - (const Vector <Type> & vecA, const Vector <Type> & vecB)
{
    Vector <Type> vecC = vecA;

    return vecC -= vecB;
}

//----------------------------------------------------------------------------

template <typename Type>
inline Vector <Type> & operator *= (Vector <Type> & vecA, const Vector <Type> & vecB)
{
    vecA.x *= vecB.x;
    vecA.y *= vecB.y;

    return vecA;
}

template <typename Type>
inline Vector <Type> operator * (const Vector <Type> & vecA, const Vector <Type> & vecB)
{
    Vector <Type> vecC = vecA;

    return vecC *= vecB;
}

//----------------------------------------------------------------------------

template <typename TypeA, typename TypeB>
inline Vector <TypeA> & operator *= (Vector <TypeA> & vec, const TypeB val)
{
    vec.x *= val;
    vec.y *= val;

    return vec;
}

template <typename TypeA, typename TypeB>
inline Vector <TypeA> operator * (const Vector <TypeA> & vec, const TypeB val)
{
    Vector <TypeA> vecA = vec;

    return vecA *= val;
}


template <typename TypeA, typename TypeB>
inline Vector <TypeB> & operator *= (const TypeA val, Vector <TypeB> & vec)
{
    return vec *= val;
}

template <typename TypeA, typename TypeB>
inline Vector <TypeB> operator * (const TypeA val, const Vector <TypeB> & vec)
{
    Vector <TypeB> vecA = vec;

    return vecA * val;
}

//----------------------------------------------------------------------------

template <typename Type>
inline Vector <Type> & operator /= (Vector <Type> & vec, const Type val)
{
    vec.x /= val;
    vec.y /= val;

    return vec;
}

template <typename Type>
inline Vector <Type> operator / (const Vector <Type> & vec, const Type val)
{
    Vector <Type> vecA = vec;

    return vecA /= val;
}

//============================================================================

template <typename Type>
Vector <Type> Laplacian (Vector <Type> ** vectorField, Vector <int> coords, float spaceStep)
{
    Vector <Type> vectorLaplacian;

    vectorLaplacian.x  =     vectorField.x [coords.x + 1][coords.y];
    vectorLaplacian.x -= 2 * vectorField.x [coords.x]    [coords.y];
    vectorLaplacian.x +=     vectorField.x [coords.x - 1][coords.y];

    vectorLaplacian.x +=     vectorField.x [coords.x]    [coords.y + 1];
    vectorLaplacian.x -= 2 * vectorField.x [coords.x]    [coords.y];
    vectorLaplacian.x +=     vectorField.x [coords.x]    [coords.y - 1];

    vectorLaplacian.x /= powf (spaceStep, 2);


    vectorLaplacian.y  =     vectorField.y [coords.x + 1][coords.y];
    vectorLaplacian.y -= 2 * vectorField.y [coords.x]    [coords.y];
    vectorLaplacian.y +=     vectorField.y [coords.x - 1][coords.y];

    vectorLaplacian.y +=     vectorField.y [coords.x]    [coords.y + 1];
    vectorLaplacian.y -= 2 * vectorField.y [coords.x]    [coords.y];
    vectorLaplacian.y +=     vectorField.y [coords.x]    [coords.y - 1];

    vectorLaplacian.y /= powf (spaceStep, 2);


    return vectorLaplacian;
}

template <typename Type>
Type Laplacian (Type ** scalarField, Vector <int> coords, float spaceStep)
{
    Type scalarLaplacian  =     scalarField [coords.x + 1][coords.y];
         scalarLaplacian -= 2 * scalarField [coords.x]    [coords.y];
         scalarLaplacian +=     scalarField [coords.x - 1][coords.y];

    scalarLaplacian +=     scalarField [coords.x][coords.y + 1];
    scalarLaplacian -= 2 * scalarField [coords.x][coords.y];
    scalarLaplacian +=     scalarField [coords.x][coords.y - 1];

    scalarLaplacian /= powf (spaceStep, 2);


    return scalarLaplacian;
}


template <typename Type>
Vector <Type> ** LaplacianForField (Vector <Type> ** vectorField, Vector <int> fieldSize, float spaceStep)
{
    Vector <Type> ** newVectorField;

    for (int x = 0; x < fieldSize.x; x ++)
    {
        for (int y = 0; y < fieldSize.y; y ++)
        {
            newVectorField [x][y] = Laplacian (vectorField, Vector <int> (x, y), spaceStep);
        }
    }
}

template <typename Type>
Type ** LaplacianForField (Type ** scalarField, Vector <int> fieldSize, float spaceStep)
{
    Type ** newScalarField;

    for (int x = 0; x < fieldSize.x; x ++)
    {
        for (int y = 0; y < fieldSize.y; y ++)
        {
            newScalarField [x][y] = Laplacian (scalarField, Vector <int> (x, y), spaceStep);
        }
    }
}

//----------------------------------------------------------------------------

template <typename Type>
Type Divergence (Vector <Type> ** vectorField, Vector <int> coords, float spaceStep)
{
    Type scalarDivergence  = vectorField.x [coords.x + 1][coords.y];
         scalarDivergence -= vectorField.x [coords.x - 1][coords.y];


    scalarDivergence += vectorField.y [coords.x][coords.y + 1];
    scalarDivergence -= vectorField.y [coords.x][coords.y - 1];

    scalarDivergence /= 2 * spaceStep;


    return scalarDivergence;
}


template <typename Type>
Type ** DivergenceForField (Vector <Type> ** vectorField, Vector <int> fieldSize, float spaceStep)
{
    Type ** newScalarField;

    for (int x = 0; x < fieldSize.x; x ++)
    {
        for (int y = 0; y < fieldSize.y; y ++)
        {
            newScalarField [x][y] = Divergence (vectorField, Vector <int> (x, y), spaceStep);
        }
    }
}

//----------------------------------------------------------------------------

template <typename Type>
Vector <Type> Gradient (Type ** scalarField, Vector <int> coords, float spaceStep)
{
    Vector <Type> vectorGradient;

    vectorGradient.x  = scalarField [coords.x + 1][coords.y];
    vectorGradient.x -= scalarField [coords.x - 1][coords.y];

    vectorGradient.x /= 2 * spaceStep;


    vectorGradient.y += scalarField [coords.x][coords.y + 1];
    vectorGradient.y -= scalarField [coords.x][coords.y - 1];

    vectorGradient.y /= 2 * spaceStep;


    return vectorGradient;
}


template <typename Type>
Vector <Type> ** GradientForField (Type ** scalarField, Vector <int> fieldSize, float spaceStep)
{
    Vector <Type> ** newVectorField;

    for (int x = 0; x < fieldSize.x; x ++)
    {
        for (int y = 0; y < fieldSize.y; y ++)
        {
            newVectorField [x][y] = Gradient (scalarField, Vector <int> (x, y), spaceStep);
        }
    }
}

#endif
