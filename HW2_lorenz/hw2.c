/*
Julia Troni
CSCI4229 Fall 2022
Homework 2
Lorenz Attractor
*/

/*
 *  Lorenz Attractor
 *  arrows Change view angle (left/right arrows rotates about the y axis up/down arrows rotates about the x axis)
 *  0      Reset view angle to 0,0,0 and resets s, b, r parameters 
 *  ESC    Exit
 * 1 changes the color to red
 * 2 changes the color to green
 * 3 changes the color to blue
 * S increases the s parameter of the lorenz attractor
 * s decreases the s parameter of the lorenz attractor
 * B increases the b parameter of the lorenz attractor
 * b decreases the b parameter of the lorenz attractor
 * R increases the r parameter of the lorenz attractor
 * r decreases the r parameter of the lorenz attractor
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

//  Globals
int th = 0; // Azimuth of view angle
int ph = 0; // Elevation of view angle
char mode = '1'; //begins in red mode
double dim = 2; // Dimension of orthogonal box

/*  Lorenz Parameters  */
double s = 10;
double b = 2.6666;
double r = 28;
double w = 0.02;

int N = 50000;           // Number of points
double coords[50000][3]; // Array of points for Lorenz Attractor
double colors[50000][3]; // Array of colors for Lorenz Attractor

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192 // Maximum length of text string
void Print(const char *format, ...)
{
    char buf[LEN];
    char *ch = buf;
    va_list args;
    //  Turn the parameters into a character string
    va_start(args, format);
    vsnprintf(buf, LEN, format, args);
    va_end(args);
    //  Display the characters one at a time at the current raster position
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(1);
}

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char *where)
{
    int err = glGetError();
    if (err)
        fprintf(stderr, "ERROR: %s [%s]\n", gluErrorString(err), where);
}

/*
 * Simple loop to generate a matrix of points and colors to generate the lorenz attractor
 *  Integrate 50,000 steps (50 time units with dt = 0.001)
 *  Explicit Euler integration
 */
void calcLorenz(void)
{

    /*  Coordinates  */
    double x = 1;
    double y = 1;
    double z = 1;
    /*  Time step  */
    double dt = 0.001;

    for (int i = 0; i < N; i++)
    {
        double dx = s * (y - x);
        double dy = x * (r - z) - y;
        double dz = x * y - b * z;
        x += dt * dx;
        y += dt * dy;
        z += dt * dz;
        coords[i][0] = x * w;
        coords[i][1] = y * w;
        coords[i][2] = z * w;

        double red;
        double green;
        double blue;
        double threshold = (double)N / 3;
        //1 changes the color to red
        if (mode == '1')
        {
            red = 1;
            green = (double)i / N;
            blue = (double)i / N;
        }
        //2 changes the color to green
        else if (mode == '2')
        {
            red = (double)i / N;
            green = 1;
            blue = (double)i / N;
        }
        //3 changes the color to blue
        else if (mode == '3')
        {
            red = (double)i / N;
            green = (double)i / N;
            blue = 1;
        }
        else
        {
            if (i < threshold)
            {
                red = (double)(N - i) / N;
                green = 1;
                blue = 1;
            }
            else if (i < 2 * threshold)
            {
                red = 1;
                green = (double)(N - i - threshold) / N;
                blue = 1;
            }
            else
            {
                red = 1;
                green = 1;
                blue = (double)(N - i - 2 * threshold) / N;
            }
        }
        colors[i][0] = red;
        colors[i][1] = green;
        colors[i][2] = blue;
    }
}

/*
 *  Display the scene
 */
void display()
{
    // generate the coordinates for the lorenz attractor
    calcLorenz();

    //  Clear the image
    glClear(GL_COLOR_BUFFER_BIT);
    //  Reset previous transforms
    glLoadIdentity();

    //  Set view angle
    glRotated(ph, 1, 0, 0); // around x axis
    glRotated(th, 0, 1, 0); // around y axis

    int i;
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < N; i++)
    {
        glColor3f(colors[i][0], colors[i][1], colors[i][2]);
        glVertex3dv(coords[i]);
    }
    glEnd();

    //  Draw axes in white
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(1, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 1, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, 1);
    glEnd();

    //  Label axes
    glRasterPos3d(1, 0, 0);
    Print("X");
    glRasterPos3d(0, 1, 0);
    Print("Y");
    glRasterPos3d(0, 0, 1);
    Print("Z");

    //  Display coordinates
    glWindowPos2i(5, 5);
    Print("View Angle=%d,%d", th, ph);

    glFlush();
    glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
    //  Exit on ESC
    if (ch == 27)
        exit(0);
    //  Reset view angle
    else if (ch == '0')
    {
        th = ph = 0;
        s = 10;
        b = 2.6666;
        r = 28;
    }
    // change color to red
    else if (ch == '1' )
    {
        mode = '1';
        calcLorenz(); // recalculate the lorenz attractor to change the colors
    }
    // change color to green
    else if (ch == '2')
    {
        mode = '2';
        calcLorenz(); // recalculate the lorenz attractor to change the colors
    }
    // change color to blue
    else if (ch == '3')
    {
        mode = '3';
        calcLorenz(); // recalculate the lorenz attractor to change the colors
    }
    // Increase s
    else if (ch == 'S')
    {
        s += 1;
        calcLorenz();
    }
    // Decrease s
    else if (ch == 's')
    {
        s -= 1;
        calcLorenz();
    }
    // Increase b
    else if (ch == 'B')
    {
        b += 1;
        calcLorenz();
    }
    // Decrease b
    else if (ch == 'b')
    {
        b -= 1;
        calcLorenz();
    }
    // Increase r
    else if (ch == 'R')
    {
        r += 1;
        calcLorenz();
    }
    // Decrease r
    else if (ch == 'r')
    {
        r -= 1;
        calcLorenz();
    }

    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
    //  Right arrow key - increase azimuth by 5 degrees
    if (key == GLUT_KEY_RIGHT)
        th += 5;
    //  Left arrow key - decrease azimuth by 5 degrees
    else if (key == GLUT_KEY_LEFT)
        th -= 5;
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
        ph += 5;
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
        ph -= 5;
    //  Keep angles to +/-360 degrees
    th %= 360;
    ph %= 360;
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
    //  Set the viewport to the entire window
    glViewport(0, 0, RES * width, RES * height);
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Orthogonal projection box adjusted for the
    //  aspect ratio of the window
    double asp = (height > 0) ? (double)width / height : 1;
    glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char *argv[])
{
    //  Initialize GLUT and process user parameters
    glutInit(&argc, argv);
    //  Request double buffered, true color window
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    //  Request 500 x 500 pixel window
    glutInitWindowSize(500, 500);
    //  Create the window
    glutCreateWindow("Julia Troni HW2");
#ifdef USEGLEW
    //  Initialize GLEW
    if (glewInit() != GLEW_OK)
        Fatal("Error initializing GLEW\n");
#endif
    //  Tell GLUT to call "display" when the scene should be drawn
    glutDisplayFunc(display);
    //  Tell GLUT to call "reshape" when the window is resized
    glutReshapeFunc(reshape);
    //  Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);
    //  Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);
    //  Pass control to GLUT so it can interact with the user
    glutMainLoop();
    //  Return code
    return 0;
}
