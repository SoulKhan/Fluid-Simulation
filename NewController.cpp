//============================================================================
// g++ cmdline +=
// X:\gl\ijl15.lib X:\gl\glut32.lib X:\gl\glew32.lib -lopengl32 -lglu32 -lgdi32 -lwinmm
//============================================================================

#include "TXLib.h"

#include "gl/glew.h"
#include <gl/gl.h>
#include "gl/glut.h"

#include "gl/ijl.h"
#include "gl/load_jpg.cpp"

#include "gl/glaux.h"

#include "Model.cpp"

//============================================================================

const int SIZE_X = 127,
          SIZE_Y = 127;

const float TIME_STEP  = 0.01f,
            SPACE_STEP = 0.1f,
            ALPHA      = 0.25f,
            VIS        = 0.0005f,
            NEW_HEIGHT = 0.0002f;

//============================================================================

struct Vertex
{
    float coords [3];
    float normal [3];
};

//============================================================================

Vertex Vertices [SIZE_X][SIZE_Y];

float ** CurrentHeight;
float ** PreviousHeight;
float ** PrePreviousHeight;

float ModelAngleX  = 0.0f;
float ModelAngleY  = 0.0f;
float ModelAngleZ  = 0.0f;

float ModelZoomZ  = 0.0f;

float NextWave = GetTickCount ();

unsigned int Texture [1];

//============================================================================

float** initialize2dArr (Vector <int> arrSize);

void free2dArr (float** arr, int sizeX);

void equateArrs (float** arrA, float** arrB, Vector <int> arrSize);

//----------------------------------------------------------------------------

void InitGL ();

void DrawNextTimeLayer ();

void DrawGLScene ();

void ResizeGLScene (int width, int height);

void Keyboard (unsigned char key, int x, int y);

void MakeWave ();

AUX_RGBImageRec* LoadBMP (char* filename);

int LoadGLTextures();

//============================================================================

int main (int argc, char* argv[])
{
    glutInit               (&argc, argv);
    glutInitDisplayMode    (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize     (GetSystemMetrics (SM_CXSCREEN) / 2, GetSystemMetrics (SM_CYSCREEN) / 2);
    glutInitWindowPosition (100, 100);
    glutCreateWindow       (argv[0]);

    InitGL();

    glutDisplayFunc  (DrawGLScene);
    glutReshapeFunc  (ResizeGLScene);
    glutIdleFunc     (DrawGLScene);
    glutKeyboardFunc (Keyboard);

    glutMainLoop ();

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
        assert (arr [x]);

        free (arr [x]);
    }

    assert (arr);

    free (arr);
}

void equateArrs (float** arrA, float** arrB, Vector <int> arrSize)
{
    for (int x = 0; x < arrSize.x; x ++)
    {
        for (int y = 0; y < arrSize.y; y ++)
        {
            assert (x < arrSize.x && x >= 0);
            assert (y < arrSize.y && y >= 0);

            arrA [x][y] = arrB [x][y];
        }
    }
}

//----------------------------------------------------------------------------

void InitGL ()
{
    if (!LoadGLTextures())
    {
       puts ("IMAGE NOT LOADED!");
    }

    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glClearColor (0.7f, 0.7f, 0.7f, 0.0f);
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);

    glShadeModel (GL_SMOOTH);
    glDisable (GL_LINE_SMOOTH);

    //glEnable (GL_CULL_FACE);

    memset (Vertices, 0, sizeof (Vertices));

    CurrentHeight     = initialize2dArr (Vector <int> (SIZE_X, SIZE_Y));
    PreviousHeight    = initialize2dArr (Vector <int> (SIZE_X, SIZE_Y));
    PrePreviousHeight = initialize2dArr (Vector <int> (SIZE_X, SIZE_Y));

    for (int x = 0; x < SIZE_X; x ++)
    {
        for (int y = 0; y < SIZE_Y; y ++)
        {
            assert (x < SIZE_X && x >= 0);
            assert (y < SIZE_Y && y >= 0);

            Vertices [x][y].coords [0] = 1.0f - 2.0f * (float) x / SIZE_X;
            Vertices [x][y].coords [1] = 1.0f - 2.0f * (float) y / SIZE_Y;

            Vertices [x][y].normal [2] = - 4.0f / SIZE_X;
        }
    }

    loadjpgGL ("U:\\Учебные классы\\Water\\Water.jpg");

    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
    glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

    glEnable (GL_TEXTURE_GEN_S);
    glEnable (GL_TEXTURE_GEN_T);

    glEnable (GL_DEPTH_TEST);
    glEnable (GL_TEXTURE_2D);

    glEnable (GL_NORMALIZE);

    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);


}

void DrawNextTimeLayer ()
{
    MakeWave ();

    for (int x = 1; x + 1 < SIZE_X; x ++)
    {
        for (int y = 1; y + 1 < SIZE_Y; y ++)
        {
            assert (x + 1 < SIZE_X && x - 1 >= 0);
            assert (y + 1 < SIZE_Y && y - 1 >= 0);

            Vertices [x][y].coords [2] = CurrentHeight [x][y];

            Vertices [x][y].normal [0] = CurrentHeight [x - 1][y] - CurrentHeight [x + 1][y];
            Vertices [x][y].normal [1] = CurrentHeight [x][y - 1] - CurrentHeight [x][y + 1];
        }
    }

    computeNextHeight (CurrentHeight, PreviousHeight, PrePreviousHeight,
                       Vector <int> (SIZE_X, SIZE_Y), SPACE_STEP, TIME_STEP, ALPHA, VIS);

    for (int x = 0; x + 1 < SIZE_X; x++)
    {
        glBegin (GL_TRIANGLE_STRIP);

        for (int y = 0; y < SIZE_Y; y++)
        {
            assert (x + 1 < SIZE_X && x + 1 >= 0);
            assert (y < SIZE_Y && y >= 0);

            glNormal3fv (Vertices [x][y].normal);
            glVertex3fv (Vertices [x][y].coords);

            glNormal3fv (Vertices [x + 1][y].normal);
            glVertex3fv (Vertices [x + 1][y].coords);

        }

        glEnd();
    }

    equateArrs (PrePreviousHeight, PreviousHeight, Vector <int> (SIZE_X, SIZE_Y));
    equateArrs (PreviousHeight,    CurrentHeight,  Vector <int> (SIZE_X, SIZE_Y));
}

void DrawGLScene ()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity ();

    float pos [4] = {0.0f, 0.0f, -1.0f, 0.0f};

    glTranslatef (0.0f, 0.0f, -3.0f);

    glLightfv (GL_LIGHT0, GL_POSITION, pos);

    glTranslatef (0.0f, 0.0f, ModelZoomZ);

    glRotatef (ModelAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef (ModelAngleY, 0.0f, 1.0f, 0.0f);
    glRotatef (ModelAngleZ, 0.0f, 0.0f, 1.0f);

    //glBindTexture(GL_TEXTURE_2D, Texture [0]);

    glBegin(GL_QUADS);

    //glTexCoord2f (0.0f, 0.0f);
    //glVertex3f (Vertices [0]     [0].coords [0], Vertices [0]     [0].coords [1],  0.0f);
    //glTexCoord2f (1.0f, 0.0f);
    //glVertex3f (Vertices [0]     [0].coords [0], Vertices [0]     [0].coords [1],  1.0f);
    //glTexCoord2f (1.0f, 1.0f);
    //glVertex3f (Vertices [SIZE_X][0].coords [0], Vertices [SIZE_X][0].coords [1],  0.0f);
    //glTexCoord2f (0.0f, 1.0f);
    //glVertex3f (Vertices [SIZE_X][0].coords [0], Vertices [SIZE_X][0].coords [1],  1.0f);

    glVertex3f (-1.0f, 1.0f,  0.0f);
    glVertex3f (-1.0f, 1.0f,  1.0f);
    glVertex3f (1.0f, 1.0f,  1.0f);
    glVertex3f (1.0f, 1.0f,  0.0f);

    glEnd ();

    DrawNextTimeLayer ();

    glutSwapBuffers ();
}

void ResizeGLScene (int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }

    glViewport (0, 0, width, height);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    gluPerspective (45.0f, (GLfloat) width / (GLfloat) height, 0.1f, 100.0f);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

void Keyboard (unsigned char key, int x, int y)
{
    #pragma unused (x, y)

    switch (key)
    {
        case 'q':

        ModelAngleY -= 1.0f;

        break;


        case 'e':

        ModelAngleY += 1.0f;

        break;


        case 'w':

        ModelAngleX -= 1.0f;

        break;


        case 's':

        ModelAngleX += 1.0f;

        break;


        case 'a':

        ModelAngleZ += 1.0f;

        break;


        case 'd':

        ModelAngleZ -= 1.0f;

        break;


        case '=':

        ModelZoomZ += 0.1f;

        break;


        case '-':

        ModelZoomZ -= 0.1f;

        break;


        default:

        break;
    }

    glutPostRedisplay();
}

void MakeWave ()
{
    if (GetTickCount () - NextWave >= 10000)
    {
        Vector <int> randomLocation (rand () % (SIZE_X - 11), rand () % (SIZE_Y - 11));

        Vector <int> randomRadius (rand () % 5 + 1, 0);

        randomRadius.y = randomRadius.x;

        for (int randomX = -randomRadius.x; randomX < randomRadius.x + 1; randomX ++)
        {
            for (int randomY = -randomRadius.y; randomY < randomRadius.y + 1; randomY ++)
            {
                float amplitude = (pow (randomRadius.x, 2) + pow (randomRadius.y, 2))
                                  - pow (randomX, 2) - pow (randomY, 2);

                CurrentHeight [randomLocation.x + randomX + 5][randomLocation.y + randomY + 5] -= amplitude * 3.0f;
            }
        }

        NextWave = GetTickCount ();

        //puts ("KEK!");
    }
}

AUX_RGBImageRec* LoadBMP(char* filename)
{
    FILE* file = NULL;

    if (!filename) return NULL;

    fopen (filename, "r");

    if (file)
    {
        fclose (file);

        return auxDIBImageLoad (filename);
    }

    return NULL;
}

int LoadGLTextures()
{
    int status = 0;

    AUX_RGBImageRec* textureImage [1];

    memset (textureImage, 0, sizeof (void *) * 1);

    if (textureImage [0] = LoadBMP ("Vlad.bmp"))
    {
        status = 1;

        glGenTextures (1, &Texture [0]);

        glBindTexture(GL_TEXTURE_2D, Texture [0]);

        glTexImage2D (GL_TEXTURE_2D, 0, 3, textureImage [0] -> sizeX, textureImage [0] -> sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImage [0] -> data);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }

    if (textureImage [0])                            // If Texture Exists
    {
        if (textureImage [0] -> data)                   // If Texture Image Exists
        {
            free (textureImage [0] -> data);             // Free The Texture Image Memory
        }

        free (textureImage [0]);                      // Free The Image Structure
    }

    return status;
}
//============================================================================
