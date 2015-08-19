//=================================================================================================================
//{         Linker Settings
//=================================================================================================================

// g++ cmdline +=
// X:\gl\ijl15.lib X:\gl\glut32.lib X:\gl\glew32.lib -lopengl32 -lglu32 -lgdi32 -lwinmm

//}
//=================================================================================================================

//=================================================================================================================
//{         Includes
//=================================================================================================================

#include "TXLib.h"              //  Best Graphics Library in the World

#include "gl/glew.h"            //
#include <gl/gl.h>              //  OpenGL
#include "gl/glut.h"            //

#include "gl/ijl.h"             //  Libraries
#include "gl/load_jpg.cpp"      //  to load jpg images

#include "Model.cpp"            //  Calculation of waves propogation

//}
//=================================================================================================================

//=================================================================================================================
//{         Constants
//=================================================================================================================

const int SIZE_X = 127,                //  Quantity of cells on X axis of simulating surface
          SIZE_Y = 127;                //  Quantity of cells on Y axis of simulating surface

const float TIME_STEP  = 0.01f,        //  Time step in difference scheme
            SPACE_STEP = 0.1f,         //  Space step in difference scheme
            NEW_HEIGHT = 0.0002f;      //  On this value fluid surface is deformed

float       VIS        = 0.0005f,      //  Viscosity of fluid
            ALPHA      = 0.20f;        //  Speed of waves propogation

//}
//=================================================================================================================

//=================================================================================================================
//{         Structures
//=================================================================================================================

struct Vertex           //  Point in 3d space with normal
{
    float coords [3];
    float normal [3];
};

struct Vect3d           //  Vector in 3d space
{
    float x, y, z;
};

struct FuncParams       //  Function and it's parameters
{
    int func;

    Vect3d params;
};

struct ViewPoint
{
    float angleX, angleY, angleZ;

    float zoomZ;
};

//}
//=================================================================================================================

//=================================================================================================================
//{         Global Variables
//=================================================================================================================

    //-------------------------------------------------------------------------------------------------------------
    //{         Setup Variables
    //-----------------------------------------------------------------------------------------------------------------

    Vertex Vertices [SIZE_X][SIZE_Y];       //  Simulating surface

    float ** CurrentHeight;                 //
    float ** PreviousHeight;                //  Hight of points on fluid surface on current/previous/preprevious time layer
    float ** PrePreviousHeight;             //

    unsigned int Texture [2];               //  Storage for two textures

    unsigned int Base = 0;                  //  Base Display List for the Font Set

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Model Motion Controlling Variables
    //-----------------------------------------------------------------------------------------------------------------

    ViewPoint viewPointA = {-60.0f, 0.0f, 223.0f, 0.4f};
    ViewPoint viewPointB = {-60.0f, 0.0f, 223.0f, 0.4f};

    bool changeToViewPointB = true;

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Fluid Surface Controlling Variables
    //-----------------------------------------------------------------------------------------------------------------

    float NextWave = 10000;                 //  Countdown till the next random generated wave

    float CurrentWaveAmplitudeMode   = 1.0f,
          CurrentWaveFrequencyMode   = 1.0f;

    float MaxHeight = 0,
          VelocitySum = 0;

    bool Pause    = false,
         ShowInfo = false;

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Currently not used variables
    //-----------------------------------------------------------------------------------------------------------------

    FuncParams* Controls;                   //  Storage for functions that control motion of fluid surface

    int ControlsSize = 0;                   //  Quantity of controlling functions

    int CurrentControl = 0;                 //  Current controlling function

    //}
    //-------------------------------------------------------------------------------------------------------------

//}
//=================================================================================================================

//=================================================================================================================
//{         Declarations of Functions
//=================================================================================================================

    //-------------------------------------------------------------------------------------------------------------
    //{         Functions to Work with Arrays
    //-----------------------------------------------------------------------------------------------------------------

    float** initialize2dArr  (Vector <int> arrSize);

    void free2dArr  (float** arr, int sizeX);

    void equateArrs (float** arrA, float** arrB, Vector <int> arrSize);

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Drawing Functions
    //-----------------------------------------------------------------------------------------------------------------

    void DrawNextTimeLayer ();                              //  Draw waves on next time layer

    void DrawGLScene ();

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Setup Functions
    //-------------------------------------------------------------------------------------------------------------

    void InitGL ();                                         //  Initialize function

    void ResizeGLScene (int width, int height);

    void Keyboard (unsigned char key, int x, int y);

    int MakeWave ();                                        // Generate wave at random location with random amplitude

    void BuildFont ();

    inline void KillFont ();

    void Print (const char* message, ...);

    void ChangeView (ViewPoint* viewPointA, ViewPoint* viewPointB, float numOfSteps);

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Currently not Used Functions
    //-------------------------------------------------------------------------------------------------------------

    //void LoadTextures ();

    //void ReadController ();

    //}
    //-------------------------------------------------------------------------------------------------------------

//}
//=================================================================================================================

//=================================================================================================================
//{         Main
//=================================================================================================================

int main (int argc, char* argv[])
{
    glutInit               (&argc, argv);
    glutInitDisplayMode    (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize     (GetSystemMetrics (SM_CXSCREEN) / 2, GetSystemMetrics (SM_CYSCREEN) / 2);
    glutInitWindowPosition (100, 100);
    glutCreateWindow       ("Fluid Simulation");

    InitGL ();

    glutDisplayFunc  (DrawGLScene);
    glutReshapeFunc  (ResizeGLScene);
    glutIdleFunc     (DrawGLScene);
    glutKeyboardFunc (Keyboard);

    glutMainLoop ();

    return 0;
}


//}
//=================================================================================================================

//=================================================================================================================
//{         Implementations of Functions
//=================================================================================================================

    //-------------------------------------------------------------------------------------------------------------
    //{         Functions to Work with Arrays
    //-------------------------------------------------------------------------------------------------------------

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

        //---------------------------------------------------------------------------------------------------------

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

        //---------------------------------------------------------------------------------------------------------

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

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Drawing Functions
    //-------------------------------------------------------------------------------------------------------------

        void DrawNextTimeLayer ()
        {
            if (!Pause) MakeWave ();

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

            if (!Pause) MaxHeight = computeNextHeight (CurrentHeight, PreviousHeight, PrePreviousHeight,
                                                       Vector <int> (SIZE_X, SIZE_Y), SPACE_STEP, TIME_STEP, ALPHA, VIS);

            glEnable (GL_TEXTURE_2D);

            glEnable (GL_NORMALIZE);

            glEnable (GL_LIGHTING);

            glDisable (GL_CULL_FACE);
            glEnable (GL_DEPTH_TEST);

            glEnable (GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

            glEnable (GL_TEXTURE_GEN_S);
            glEnable (GL_TEXTURE_GEN_T);

            glBindTexture(GL_TEXTURE_2D, Texture [1]);

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

            VelocitySum = 0;

            for (int x = 0; x < SIZE_X; x ++)
            {
                for (int y = 0; y < SIZE_Y; y ++)
                {
                    assert (x < SIZE_X && x >= 0);
                    assert (y < SIZE_Y && y >= 0);

                    VelocitySum += fabs (CurrentHeight [x][y] - PreviousHeight [x][y]);
                }
            }

            equateArrs (PrePreviousHeight, PreviousHeight, Vector <int> (SIZE_X, SIZE_Y));
            equateArrs (PreviousHeight,    CurrentHeight,  Vector <int> (SIZE_X, SIZE_Y));
        }

        //---------------------------------------------------------------------------------------------------------

        void DrawGLScene ()
        {
            glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity ();

            glTranslatef (0.0f, 0.5f, -3.0f);

            float pos [4] = {0.0f, -1.0f, -1.0f, 0.0f};
            glLightfv (GL_LIGHT0, GL_POSITION, pos);

            if (!changeToViewPointB) ChangeView (&viewPointA, &viewPointB, 0.001f);

            glTranslatef (0.0f, 0.0f, viewPointA.zoomZ);

            glRotatef (viewPointA.angleX, 1.0f, 0.0f, 0.0f);
            glRotatef (viewPointA.angleY, 0.0f, 1.0f, 0.0f);
            glRotatef (viewPointA.angleZ, 0.0f, 0.0f, 1.0f);

            glEnable (GL_NORMALIZE);

            glEnable (GL_LIGHTING);

            glEnable (GL_TEXTURE_2D);

            glEnable (GL_CULL_FACE);
            glCullFace (GL_FRONT);

            glEnable (GL_DEPTH_TEST);

            glDisable (GL_TEXTURE_GEN_S);
            glDisable (GL_TEXTURE_GEN_T);

            glDisable (GL_BLEND);

            glBindTexture(GL_TEXTURE_2D, Texture [0]);


            glBegin(GL_QUADS);

                // Back Face
                glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.7f);  // Bottom Right Of The Texture and Quad
                glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.7f);  // Top Right Of The Texture and Quad
                glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.7f);  // Top Left Of The Texture and Quad
                glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.7f);  // Bottom Left Of The Texture and Quad

                // Top Face
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.7f);  // Top Left Of The Texture and Quad
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  0.3f);  // Bottom Left Of The Texture and Quad
                glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  0.3f);  // Bottom Right Of The Texture and Quad
                glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.7f);  // Top Right Of The Texture and Quad

                // Bottom Face
                glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.7f);  // Top Right Of The Texture and Quad
                glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.7f);  // Top Left Of The Texture and Quad
                glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  0.3f);  // Bottom Left Of The Texture and Quad
                glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.3f);  // Bottom Right Of The Texture and Quad

                // Right face
                glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.7f);  // Bottom Right Of The Texture and Quad
                glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.7f);  // Top Right Of The Texture and Quad
                glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  0.3f);  // Top Left Of The Texture and Quad
                glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  0.3f);  // Bottom Left Of The Texture and Quad

                // Left Face
                glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.7f);  // Bottom Left Of The Texture and Quad
                glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.3f);  // Bottom Right Of The Texture and Quad
                glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  0.3f);  // Top Right Of The Texture and Quad
                glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.7f);  // Top Left Of The Texture and Quad

            glEnd ();


            glDisable (GL_TEXTURE_2D);

            glDisable (GL_NORMALIZE);

            glDisable (GL_LIGHTING);

            glDisable (GL_TEXTURE_GEN_S);
            glDisable (GL_TEXTURE_GEN_T);

            glDisable (GL_DEPTH_TEST);

            glEnable (GL_CULL_FACE);
            glCullFace (GL_BACK);

            glEnable (GL_BLEND);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            float alpha = 0.5f;


            glBegin (GL_QUADS);

                // Back Face
                glColor4f(0.3f, 0.3f, 1.0f, alpha);
                glVertex3f(-1.0f, -1.0f, -1.7f);
                glVertex3f(-1.0f,  1.0f, -1.7f);
                glVertex3f( 1.0f,  1.0f, -1.7f);
                glVertex3f( 1.0f, -1.0f, -1.7f);

                // Top Face
                glColor4f(0.3f, 0.3f, 1.0f, alpha);
                glVertex3f(-1.0f,  1.0f, -1.7f);
                glVertex3f(-1.0f,  1.0f,  0.0f);
                glVertex3f( 1.0f,  1.0f,  0.0f);
                glVertex3f( 1.0f,  1.0f, -1.7f);

                // Bottom Face
                glColor4f(0.3f, 0.3f, 1.0f, alpha);
                glVertex3f(-1.0f, -1.0f, -1.7f);
                glVertex3f( 1.0f, -1.0f, -1.7f);
                glVertex3f( 1.0f, -1.0f,  0.0f);
                glVertex3f(-1.0f, -1.0f,  0.0f);

                // Right face
                glColor4f(0.3f, 0.3f, 1.0f, alpha);
                glVertex3f( 1.0f, -1.0f, -1.7f);
                glVertex3f( 1.0f,  1.0f, -1.7f);
                glVertex3f( 1.0f,  1.0f,  0.0f);
                glVertex3f( 1.0f, -1.0f,  0.0f);

                // Left Face
                glColor4f(0.3f, 0.3f, 1.0f, alpha);
                glVertex3f(-1.0f, -1.0f, -1.7f);
                glVertex3f(-1.0f, -1.0f,  0.0f);
                glVertex3f(-1.0f,  1.0f,  0.0f);
                glVertex3f(-1.0f,  1.0f, -1.7f);

            glEnd ();


            DrawNextTimeLayer ();


            glDisable (GL_TEXTURE_2D);

            glDisable (GL_LIGHTING);

            glDisable (GL_DEPTH_TEST);

            glLoadIdentity ();

            glTranslatef (0.0f, 0.0f, -3.0f);

            glColor3f (1.0f, 1.0f, 1.0f);
            glRasterPos2f (-2.2f, 1.15f);

            if (ShowInfo) Print ("PRESS [I] TO SHOW INFO");
            else          Print ("PRESS [I] TO HIDE INFO");


            if (ShowInfo)
            {
                glDisable (GL_TEXTURE_2D);

                glDisable (GL_LIGHTING);

                glDisable (GL_DEPTH_TEST);

                glLoadIdentity ();

                glTranslatef (0.0f, 0.0f, -3.0f);

                glColor3f (1.0f, 1.0f, 1.0f);
                glRasterPos2f (-2.2f, 0.95f);

                if (!Pause) Print ("[SIMULATION]");
                else        Print ("[PAUSE]");


                glRasterPos2f (-2.2f, 0.85f);
                Print ("Fluid surface");

                glRasterPos2f (-2.2f, 0.75f);
                Print ("deformation coefficient: %2.1f", CurrentWaveAmplitudeMode);

                glRasterPos2f (-2.2f, 0.65f);
                Print ("One drop in %2.1f seconds", CurrentWaveFrequencyMode * 2.0f);

                glRasterPos2f (-2.2f, 0.55f);
                Print ("Viscosity: %0.5f", VIS);

                glRasterPos2f (-2.2f, 0.45f);
                Print ("Wave propogation speed: %0.3f", ALPHA);


                glRasterPos2f (-2.2f, 0.25f);
                Print ("[p] - wave propogation speed");

                glRasterPos2f (-2.2f, 0.15f);
                Print ("[t] - frequency [v] - viscosity");

                glRasterPos2f (-2.2f, 0.05f);
                Print ("[h] - amplitude");

                glRasterPos2f (-2.2f, -0.15f);
                Print ("Press [key] to decrease value");

                glRasterPos2f (-2.2f, -0.25f);
                Print ("Press [shift] + [key] to increase value");


                glRasterPos2f (-2.2f, -0.45f);
                Print ("Press [q][w][e][a][s][d] to rotate the pool");

                glRasterPos2f (-2.2f, -0.55f);
                Print ("Press [-][=] to zoom in/out the pool");

                glRasterPos2f (-2.2f, -0.75f);
                Print ("Press [spacebar] to stop time");

                glRasterPos2f (-2.2f, -0.85f);
                Print ("Hold [spacebar] to calm fluid surface");

                glRasterPos2f (-2.2f, -0.95f);
                Print ("Press [r] to reset settings");


                glRasterPos2f (0.4f, 1.15f);
                Print ("Maximal height: %4.4f", MaxHeight);

                glRasterPos2f (0.4f, 1.05f);
                Print ("Average velocity: %1.7f", VelocitySum / (SIZE_X * SIZE_Y));
            }


            glutSwapBuffers ();
        }

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Setup Functions
    //-------------------------------------------------------------------------------------------------------------

    void InitGL ()
    {
        glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
        glClearDepth (1.0f);

        glEnable (GL_DEPTH_TEST);
        glDepthFunc (GL_LEQUAL);

        glShadeModel (GL_SMOOTH);
        glEnable (GL_LINE_SMOOTH);

        glColor4f (1.0f, 1.0f, 1.0f, 1.0f);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        memset (Vertices, 0, sizeof (Vertices));

        CurrentHeight     = initialize2dArr (Vector <int> (SIZE_X, SIZE_Y));
        PreviousHeight    = initialize2dArr (Vector <int> (SIZE_X, SIZE_Y));
        PrePreviousHeight = initialize2dArr (Vector <int> (SIZE_X, SIZE_Y));

        BuildFont ();

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


        Texture [0] = loadjpgGL ("X:\\Map\\Tile.jpg");
        Texture [1] = loadjpgGL ("X:\\Map\\Water6.jpg");

        glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

        glEnable (GL_TEXTURE_2D);

        glEnable (GL_NORMALIZE);

        glEnable (GL_LIGHTING);
        glEnable (GL_LIGHT0);
    }

    //-------------------------------------------------------------------------------------------------------------

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

    //-------------------------------------------------------------------------------------------------------------

    void Keyboard (unsigned char key, int x, int y)
    {
        (void) x; (void) y;

        switch (key)
        {
            case 'q':

                viewPointA.angleY -= 1.0f;

            break;


            case 'e':

                viewPointA.angleY += 1.0f;

            break;


            case 'w':

                viewPointA.angleX -= 1.0f;

            break;


            case 's':

                viewPointA.angleX += 1.0f;

            break;


            case 'a':

                viewPointA.angleZ += 1.0f;

            break;


            case 'd':

                viewPointA.angleZ -= 1.0f;

            break;


            case '=':

                viewPointA.zoomZ += 0.1f;

            break;


            case '-':

                viewPointA.zoomZ -= 0.1f;

            break;


            case 'T':

                CurrentWaveFrequencyMode += 0.1f;

            break;


            case 't':

                CurrentWaveFrequencyMode -= 0.1f;

                if (CurrentWaveFrequencyMode < 0) CurrentWaveFrequencyMode = 0.0f;

            break;


            case 'P':

                ALPHA += 0.001f;

            break;


            case 'p':

                ALPHA -= 0.001f;

                if (ALPHA < 0) ALPHA = 0.0f;

            break;


            case 'V':

                VIS += 0.00001f;

            break;


            case 'v':

                VIS -= 0.00001f;

                if (VIS < 0) VIS = 0.0f;

            break;


            case 'H':

                CurrentWaveAmplitudeMode += 0.1f;

            break;


            case 'h':

                CurrentWaveAmplitudeMode -= 0.1f;

                if (CurrentWaveAmplitudeMode < 0) CurrentWaveAmplitudeMode = 0.0f;

            break;


            case ' ':

                if (!Pause) Pause = true;

                else        Pause = false;

            break;


            case 'i':

                if (!ShowInfo) ShowInfo = true;

                else           ShowInfo = false;

            break;


            case 'r':

                viewPointA.angleX  = -60.0f;
                viewPointA.angleY  = 0.0f;
                viewPointA.angleZ  = 223.0f;

                viewPointA.zoomZ  = 0.4f;

                CurrentWaveAmplitudeMode   = 1.0f;
                CurrentWaveFrequencyMode   = 1.0f;

                VIS        = 0.0005f;
                ALPHA      = 0.20f;

                for (int x = 0; x < SIZE_X; x ++)
                {
                    for (int y = 0; y < SIZE_Y; y ++)
                    {
                        assert (x < SIZE_X && x >= 0);
                        assert (y < SIZE_Y && y >= 0);

                        CurrentHeight     [x][y] = 0;
                        PreviousHeight    [x][y] = 0;
                        PrePreviousHeight [x][y] = 0;
                    }
                }


            break;


            case '1':

                if (!changeToViewPointB) changeToViewPointB = true;

                else                     changeToViewPointB = false;

            break;


            default:

            break;
        }

        glutPostRedisplay();
    }

    //-------------------------------------------------------------------------------------------------------------

    int MakeWave ()
    {
        if (GetTickCount () - (DWORD) NextWave >= CurrentWaveFrequencyMode * 2000.0f)
        {
            Vector <int> randomLocation (rand () % (SIZE_X - 21) + 10, rand () % (SIZE_Y - 21) + 10);

            Vector <int> randomRadius (rand () % 5 + 1, 0);

            randomRadius.y = randomRadius.x;

            for (int randomX = -randomRadius.x; randomX < randomRadius.x + 1; randomX ++)
            {
                for (int randomY = -randomRadius.y; randomY < randomRadius.y + 1; randomY ++)
                {
                    float amplitude  = ((float) pow (randomRadius.x, 2) + (float) pow (randomRadius.y, 2))
                                       - (float) pow (randomX, 2) - (float) pow (randomY, 2);

                    assert (randomLocation.x + randomX + 5 < SIZE_X && randomLocation.x + randomX + 5 >= 0);
                    assert (randomLocation.y + randomY + 5 < SIZE_Y && randomLocation.y + randomY + 5 >= 0);

                    PreviousHeight [randomLocation.x + randomX + 5][randomLocation.y + randomY + 5]
                    += amplitude * (float) random (CurrentWaveAmplitudeMode, CurrentWaveAmplitudeMode + 1.0f) * 0.00005f;

                    PrePreviousHeight [randomLocation.x + randomX + 5][randomLocation.y + randomY + 5]
                    += amplitude * (float) random (CurrentWaveAmplitudeMode, CurrentWaveAmplitudeMode + 1.0f) * 0.0001f;
                }
            }

            NextWave = (float) GetTickCount ();

            return 1;
        }

        return 0;
    }

    //-------------------------------------------------------------------------------------------------------------

    void BuildFont ()
    {
        HFONT font, oldfont;

        Base = glGenLists (96);                           //  Storage for 96 characters

        font = CreateFont (-24, 0, 0, 0, FW_BOLD, false, false, false, ANSI_CHARSET, OUT_TT_PRECIS,
                           CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH, "Courier New");

        HWND wnd = FindWindow (NULL, "Fluid Simulation"); assert (wnd);
        HDC  reciever = GetDC (wnd);

        oldfont = (HFONT) SelectObject (reciever, font);       // Selects the font
        wglUseFontBitmaps (reciever, 32, 96, Base);            // Builds 96 characters starting at character 32
        SelectObject (reciever, oldfont);                      // Selects The Font We Want
        DeleteObject (font);
    }

    //-------------------------------------------------------------------------------------------------------------

    inline void KillFont ()
    {
        glDeleteLists (Base, 96);
    }

    //-------------------------------------------------------------------------------------------------------------

    void Print (const char* message, ...)
    {
        if (message == NULL) puts ("Print function: You gave me (NULL)\n");

        char text [256] = "";
        va_list arg;

        va_start (arg, message);
            vsprintf (text, message, arg);
        va_end (arg);

        glPushAttrib (GL_LIST_BIT);                              //  Pushes the display list bits
        glListBase (Base - 32);                                  //  Sets the base character to 32

        glCallLists (strlen (text), GL_UNSIGNED_BYTE, text);     // Draws the display list text
        glPopAttrib ();                                          // Pops the display list bits
    }

    //-------------------------------------------------------------------------------------------------------------

    void ChangeView (ViewPoint* viewPointA, ViewPoint* viewPointB, float numOfSteps)
    {
        viewPointA->angleX = Lerp (viewPointA->angleX, viewPointB->angleX, numOfSteps);
        viewPointA->angleY = Lerp (viewPointA->angleY, viewPointB->angleY, numOfSteps);
        viewPointA->angleZ = Lerp (viewPointA->angleZ, viewPointB->angleZ, numOfSteps);

        viewPointA->zoomZ = Lerp (viewPointA->zoomZ, viewPointB->zoomZ, numOfSteps);
    }

    //}
    //-------------------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------------------
    //{         Currently not Used Functions
    //-------------------------------------------------------------------------------------------------------------

        /*void ReadController ()
        {
            FILE* controller = fopen ("X:\\CONTROL\\Controller.txt", "r");
            assert (controller);

            fseek (controller, 0, SEEK_END);
            int bufferSize = ftell (controller);

            rewind (controller);

            char* buffer = (char*) calloc (bufferSize, sizeof (char));
            assert (buffer);



            Controls = (FuncParams*) calloc (ControlsSize, sizeof (FuncParams));
            assert (Controls);

            for (int x = 0; x < ControlsSize; x ++)
            {
                int currentChar = fgetc (controller);

                if (currentChar != ' ')
                {
                    if (currentChar == 'r') Controls [x].func = 0;

                    Controls [x].angle.x = fgetc (controller);
                    Controls [x].angle.y = fgetc (controller);
                    Controls [x].angle.z = fgetc (controller);

                    Controls [x].zoom.x = fgetc (controller);
                    Controls [x].zoom.x = fgetc (controller);
                    Controls [x].zoom.x = fgetc (controller);
                }

                else x --;
            }

            fclose (controller);
        }*/

        //---------------------------------------------------------------------------------------------------------

        /*void LoadTextures ()

        //}
        //-------------------------------------------------------------------------------------------------------------
        {
            HDC textureA = txLoadImage ("X:\\Map\\Back1.bmp");

            BITMAPINFOHEADER info = { sizeof (info), 1024, 1024, 1, WORD (sizeof (RGBQUAD) * 8), BI_RGB };

            RGBQUAD* bTextureA = new RGBQUAD [1024 * 1024];

            assert (bTextureA);

            Win32::GetDIBits (textureA, (HBITMAP) Win32::GetCurrentObject (textureA, OBJ_BITMAP),
                              0, 1024, bTextureA, (BITMAPINFO*) &info, DIB_RGB_COLORS);

            glGenTextures (1, &Texture [0]);
            glBindTexture (GL_TEXTURE_2D, Texture [0]);

            glTexImage2D (GL_TEXTURE_2D, 0, 3, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, bTextureA);

            glTexFuncParamseteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexFuncParamseteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


            txDeleteDC (textureA);
            delete bTextureA;
        }*/

    //}
    //-------------------------------------------------------------------------------------------------------------

//}
//=================================================================================================================
