/*
Julia Troni
CSCI 4229 Graphics
Homework 3: 3D Modeling
*/

/*
 *  arrows Change view angle (left/right arrows rotates about the y axis up/down arrows rotates about the x axis)
 *  0      Reset view angle to 0,0,0
 *  m changes the mode from orthagonal to perspective projection
 *  ESC closes the window and terminates the program
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
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

#define PI 3.1415927
#define Cos(x) (cos((x)*PI / 180))
#define Sin(x) (sin((x)*PI / 180))

//  Globals
int theta = 0;   // Azimuth of view angle
int phi = 0;     // Elevation of view angle
int mode = 0;    // Projection mode (0 for orthogonal, 1 for perspective)
double z = 0;    // Z variable
double w = 1;    // W variable
double dim = 60; // Dimension of orthogonal box
int fov = 60;    //  Field of view (for perspective)
double asp = 1;  //  Aspect ratio

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
 *  Set projection
 */
static void Project()
{
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Perspective transformation
    if (mode)
        gluPerspective(fov, asp, dim / 4, 4 * dim);
    //  Orthogonal projection
    else
        glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

static void drawTriangle(double x1, double y1, double z1,
                         double x2, double y2, double z2,
                         double x3, double y3, double z3)
{
    glPushMatrix();
    glBegin(GL_POLYGON);
    glVertex3d(x1, y1, z1);
    glVertex3d(x2, y2, z2);
    glVertex3d(x3, y3, z3);
    glEnd();
    glPopMatrix();
}

static void drawSquare(double x1, double z1, double x2, double z2,
                       double x3, double z3, double x4, double z4)
{
    glPushMatrix();
    glBegin(GL_POLYGON);
    glVertex3d(x1, 0, z1);
    glVertex3d(x2, 0, z2);
    glVertex3d(x3, 0, z3);
    glVertex3d(x4, 0, z4);
    glEnd();
    glPopMatrix();
}

/*
 *  Draw a 3D pyrmaid with a square base
 */
static void drawPyramid(double x, double y, double z, double height, double angle,
                        double xscale, double yscale, double zscale)
{
    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(angle, 0, 1, 0);
    glScaled(xscale, yscale, zscale);

    drawTriangle(1, 0, 0, /**/ 0, height, 0, /**/ 0, 0, 1);
    drawTriangle(0, 0, 1, /**/ 0, height, 0, /**/ -1, 0, 0);
    drawTriangle(-1, 0, 0, /**/ 0, height, 0, /**/ 0, 0, -1);
    drawTriangle(0, 0, -1, /**/ 0, height, 0, /**/ 1, 0, 0);
    drawSquare(1, 0, 0, 1, -1, 0, 0, -1);
    glPopMatrix();
}

// Grabbed this from example from class
static void Vertex(double th, double ph)
{
    glVertex3d(Sin(th) * Cos(ph), Sin(ph), Cos(th) * Cos(ph));
}

/*
 *  Draw a sphere
 *     at (x,y,z)
 *    with radius (r)
 */
static void drawSphere(double x, double y, double z, double r)
{
    const int d = 5;
    int th, ph = 0;

    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x, y, z);
    glScaled(r, r, r);

    //  Latitude bands
    for (ph = -90; ph < 90; ph += d)
    {
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d)
        {
            Vertex(th, ph);
            Vertex(th, ph + d);
        }
        glEnd();
    }

    //  Undo transformations
    glPopMatrix();
}

/*
 *
 * Draw a cylinder at (x,y,z) with radius (r) and height (h)
 */
static void drawCylinder(double r, double h, double x, double y, double z,
                         double rotation)
{
    const int SIDE_COUNT = 100;
    glPushMatrix();

    glTranslated(x, y, z);
    if (rotation)
    {
        // Draw cylinder sideways for cactus branches
        glRotated(rotation, 1, 0, 0);
    }

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= SIDE_COUNT; i++)
    {
        float angle = i * ((1.0 / SIDE_COUNT) * (2 * PI));
        glNormal3d(cos(angle), 0, sin(angle));
        glVertex3d(r * cos(angle), h, r * sin(angle));
        glVertex3d(r * cos(angle), 0, r * sin(angle));
    }
    glEnd();

    glPopMatrix();
}

/*
 *  Draw a pine tree constructed with pyramids
 *    at (x,y,z)
 */
static void drawPineTree(double x, double y, double z, double angle)
{
    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(angle, 0, 1, 0);

    glColor3d(.34, .56, 0.39);

    drawPyramid(0, 0, 0, 5, 90, 10, 2, 10);
    drawPyramid(0, 4, 0, 5, 90, 8, 2, 8);
    drawPyramid(0, 10, 0, 5, 0, 5, 2, 5);
    glPopMatrix();
}

/*
 *  Draw a bush tree constructed with spheres and a cylinder
 *    at (x,y,z) and scale xscale, yscale, zscale
 */
static void drawTree(double x, double y, double z, double angle, double xscale, double yscale, double zscale)
{
    glPushMatrix();

    glTranslated(x, y, z);
    glRotated(angle, 0, 1, 0);
    glScaled(xscale, yscale, zscale);

    glColor3f(0.5, 0.2, 0.1);
    drawCylinder(1, 5, 0, 0, 0, 0);

    glColor3f(0, 1.2, .3);
    drawSphere(0, 5, 0, 2);

    glPopMatrix();
}

/*
 *  Draw a cactus constructed with spheres and a cylinder
 *    at (x,0,z) radius (r) and height (h)
 */
static void drawCactus(double r, double h, double x, double z, double th)
{
    glPushMatrix();

    glTranslated(x, 0, z);
    glRotated(th, 0, 1, 0);

    drawCylinder(r, h, 0, 0, 0, 0);
    drawSphere(0, h, 0, r);
    drawCylinder(r, h / 3, 0, h / 3, 0, 90);
    drawSphere(0, h / 3, 0 + h / 3, r);
    drawCylinder(r, h / 3, 0, h / 3, 0 + h / 3, 0);
    drawSphere(0, h / 3 + h / 3, 0 + h / 3, r);

    drawCylinder(r, h / 3, 0, 3 * h / 5, 0, 270);
    drawSphere(0, 3 * h / 5, 0 - h / 3, r);
    drawCylinder(r, h / 4, 0, 3 * h / 5, 0 - h / 3, 0);
    drawSphere(0, h / 4 + 3 * h / 5, 0 - h / 3, r);

    glPopMatrix();
}

/*
 *  Draw a cactus sun made of a sphere
 *    at (x,0,z) radius (r)
 */
static void drawSun(double x, double y, double z, double r)
{
    glPushMatrix();
    glTranslated(x, y, z);
    glColor3d(0.9, 0.9, 0.4);
    drawSphere(0, 0, 0, r);

    glPopMatrix();
}

/*
 *  draw a car with a yellow body at x,y,z
 */
static void drawCar(double x, double y, double z)
{
    glPushMatrix();

    glTranslated(x, y, z);

    glScaled(0.25, 0.25, 0.25);

    // Draw the body
    glBegin(GL_POLYGON);
    glVertex3f(-40.0f, 0.0f, 15.0f);
    glVertex3f(40.0f, 0.0f, 15.0f);
    glVertex3f(40.0f, -15.0f, 15.0f);
    glVertex3f(-40.0f, -15.0f, 15.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(40.0f, 0.0f, -15.0f);
    glVertex3f(40.0f, 0.0f, 15.0f);
    glVertex3f(40.0f, -15.0f, 15.0f);
    glVertex3f(40.0f, -15.0f, -15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-40.0f, 0.0f, -15.0f);
    glVertex3f(-40.0f, 0.0f, 15.0f);
    glVertex3f(-40.0f, -15.0f, 15.0f);
    glVertex3f(-40.0f, -15.0f, -15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-40.0f, 0.0f, -15.0f);
    glVertex3f(40.0f, 0.0f, -15.0f);
    glColor3f(1, 1.0, 1.0);
    glVertex3f(40.0f, -15.0f, -15.0f);
    glVertex3f(-40.0f, -15.0f, -15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-40.0f, 0.0f, 15.0f);
    glVertex3f(-40.0f, 0.0f, -15.0f);
    glVertex3f(40.0f, 0.0f, -15.0f);
    glVertex3f(40.0f, 0.0f, 15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-40.0f, -15.0f, 15.0f);
    glVertex3f(-40.0f, -15.0f, -15.0f);
    glVertex3f(40.0f, -15.0f, -15.0f);
    glVertex3f(40.0f, -15.0f, 15.0f);
    glEnd();

    glBegin(GL_POLYGON);
    glVertex3f(-20.0f, 0.0f, 15.0f);
    glVertex3f(-10.0f, 10.0f, 15.0f);
    glVertex3f(20.0f, 10.0f, 15.0f);
    glVertex3f(25.0f, 0.0f, 15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-20.0f, 0.0f, -15.0f);
    glVertex3f(-10.0f, 10.0f, -15.0f);
    glVertex3f(20.0f, 10.0f, -15.0f);
    glVertex3f(25.0f, 0.0f, -15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-10.0f, 10.0f, 15.0f);
    glVertex3f(-10.0f, 10.0f, -15.0f);
    glVertex3f(20.0f, 10.0f, -15.0f);
    glVertex3f(20.0f, 10.0f, 15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-10.0f, 10.0f, 15.0f);
    glVertex3f(-20.0f, 0.0f, 15.0f);
    glVertex3f(-20.0f, 0.0f, -15.0f);
    glVertex3f(-10.0f, 10.0f, -15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(20.0f, 10.0f, 15.0f);
    glVertex3f(20.0f, 10.0f, -15.0f);
    glVertex3f(25.0f, 0.0f, -15.0f);
    glVertex3f(25.0f, 0.0f, 15.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex3f(-30.0f, -15.0f, 15.0f);
    glVertex3f(-30.0f, -15.0f, -15.0f);
    glVertex3f(30.0f, -15.0f, -15.0f);
    glVertex3f(30.0f, -15.0f, 15.0f);
    glEnd();

    // draw the wheels
    glBegin(GL_3D);
    glColor3f(1, 1, 1);
    glTranslated(-20.0f, -15.0f, 15.0f);
    glutSolidTorus(2, 5, 5, 100);

    glTranslated(0.0f, 0.0f, -30.0f);
    glutSolidTorus(2, 5, 5, 100);
    glTranslated(45.0f, 0.0f, 0.0f);
    glutSolidTorus(2, 5, 5, 100);
    glTranslated(0.0f, 0.0f, 30.0f);
    glutSolidTorus(2, 5, 5, 100);
    glEnd();

    glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
    //  Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //  Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    // Reset previous transforms
    glLoadIdentity();
    // switch between the different views
    if (mode)
    {
        double Ex = -2 * dim * Sin(theta) * Cos(phi);
        double Ey = +2 * dim * Sin(phi);
        double Ez = +2 * dim * Cos(theta) * Cos(phi);
        gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(phi), 0);
    }
    //  Orthogonal - set world orientation
    else
    {
        glRotatef(phi, 1, 0, 0);
        glRotatef(theta, 0, 1, 0);
    }

    // Draw the scene

    // draw the bush trees
    drawTree(15, 0, 0, 90, 2, 6, 2);
    drawTree(-10, 5, -20, 45, 2, 6, 2);
    drawTree(8, 0, 15, 90, 4, 8, 2);

    // draw the pine tree
    drawPineTree(0, 0, 0, 90);

    // draw the sun
    glColor3d(0.9, 0.9, 0.4);
    drawSun(-20, 40, 35, 5);

    // draw the car
    glColor3d(0.9, 0.9, 0.4);
    drawCar(-30, 0, 0);

    // draw the cactuses
    double cactusHeight = 20;
    double cactusRadius = 2;

    glColor3d(0, 0.5, 0);
    drawCactus(cactusRadius / 1.3, cactusHeight / 1.3, -20, 35, 75);
    drawCactus(cactusRadius / 2, cactusHeight / 2, 40, -30, 120);
    drawCactus(cactusRadius / 1.5, cactusHeight / 1.5, 20, -35, 200);
    drawCactus(cactusRadius / 2, cactusHeight / 2, -40, -40, 120);
    drawCactus(cactusRadius / 2, cactusHeight / 2, 30, 0, 20);

    //  Draw axes in white
    glColor3f(1, 1, 1);

    double AXIS_LENGTH = dim / 1.2;
    glBegin(GL_LINES);
    glVertex3d(0, 0, 0);
    glVertex3d(AXIS_LENGTH, 0, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, AXIS_LENGTH, 0);
    glVertex3d(0, 0, 0);
    glVertex3d(0, 0, AXIS_LENGTH);
    glEnd();
    //  Label axes
    glRasterPos3d(AXIS_LENGTH, 0, 0);
    Print("X");
    glRasterPos3d(0, AXIS_LENGTH, 0);
    Print("Y");
    glRasterPos3d(0, 0, AXIS_LENGTH);
    Print("Z");
    glFlush();
    glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
    //  Exit on ESC
    if (27 == ch)
        exit(0);
    else if ('0' == ch)
    {
        //  Reset view angle and default paramter values for the attractor
        theta = phi = 0;
    }
    else if ('m' == ch || 'M' == ch)
    {
        mode = !mode;
    }
    Project();
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
    if (key == GLUT_KEY_RIGHT)
        theta += 5;
    else if (key == GLUT_KEY_LEFT)
        theta -= 5;
    else if (key == GLUT_KEY_UP)
        phi += 5;
    else if (key == GLUT_KEY_DOWN)
        phi -= 5;
    //  Keep angles to +/-360 degrees
    theta %= 360;
    phi %= 360;
    Project();
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width, int height)
{
    //  Ratio of the width to the height of the window
    asp = (height > 0) ? (double)width / height : 1;
    //  Set the viewport to the entire window
    glViewport(0, 0, width, height);
    //  Tell OpenGL we want to manipulate the projection matrix
    Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char *argv[])
{
    //  Initialize GLUT and process user parameters
    glutInit(&argc, argv);
    //  Request double buffered, true color window with Z buffering at 500x500
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    //  Create the window
    glutCreateWindow("HW3 Julia Troni");
    //  Tell GLUT to call "display" when the scene should be drawn
    glutDisplayFunc(display);
    //  Tell GLUT to call "reshape" when the window is resized
    glutReshapeFunc(reshape);
    //  Tell GLUT to call "special" when an arrow key is pressed
    glutSpecialFunc(special);
    //  Tell GLUT to call "key" when a key is pressed
    glutKeyboardFunc(key);
    //  Tell GLUT to call "key" when a key is pressed
    glutMainLoop();
    return 0;
}