/*
Julia Troni
CSCI 4229 Graphics
Homework 4: Projections
*/

/*
 *  arrows Change view angle (left/right arrows rotates about the y axis up/down arrows rotates about the x axis)
 *  0      Reset view angle to 0,0,0 and mode to orthagonal projection (mode 0)
 *  1      Switch to perspective projection (mode 1)
 *  2       Switch to first person view (mode 0)
 *  m changes the mode from orthagonal (0) to perspective (1) to first person (3) projection
 * * In all modes (0,1,2):
 *     +/-   Change field of view for perspective
 *     x/X looks down the x axis
 *     y/Y looks down the y axis
 *     z/Z looks down the z axis
 *     Left/right arrows rotate about the y axis
 *     Up/down arrows rotate about the x axis
 *
 * In first person (2) mode only:
 *      f moves forward
 *      b moves backward
 * ESC closes the window and terminates the program
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES

//  OpenGL with prototypes for glext
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
int mode = 0;    // Projection mode (0 for orthogonal, 1 for perspective, 2 first person)
double dim = 50; // Dimension of orthogonal box
int fov = 60;    //  Field of view (for perspective)
double asp = 1;  //  Aspect ratio

//  First person view
double EX = 10; // X- coordinate of camera position in first person mode
double EY = 10; // Y- coordinate of camera position in first person mode
double EZ = 95; // Z-coordinate of camera position in first person mode
double AX = 0;  // X-coordinate of where the camera is looking in first person mode
double AY = 0;  // Y-coordinate of where the camera is looking in first person mode
double AZ = 0;  // Z-coordinate of where the camera is looking in first person mode

int move = 1; //  Ball movement
// Light values
int one = 1;       // Unit value
int distance = 70; // Light distance
int inc = 10;      // Ball increment
int smooth = 1;    // Smooth/Flat shading
int local = 0;     // Local Viewer Model
int emission = 0;  // Emission intensity (%)
int ambient = 30;  // Ambient intensity (%)
int diffuse = 100; // Diffuse intensity (%)
int specular = 0;  // Specular intensity (%)
int shininess = 0; // Shininess (power of two)
float shinyvec[1]; // Shininess (value)
int zh = 90;       // Light azimuth
float ylight = 0;  // Elevation of light
int light = 1;     // Always have light

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
    if (mode == 0)
    { // orthogonal
        glOrtho(-asp * dim, +asp * dim, -dim, +dim, -dim, +dim);
    }
    else if (mode == 1)
    { // perspective
        gluPerspective(fov, asp, dim / 4, 4 * dim);
    }
    else if (mode == 2)
    {
        gluPerspective(fov, asp, dim / 3, 3 * dim); // first person
    }
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
   glBegin(GL_TRIANGLES);
   double nx = (y2 - y1)*(z3 - z1) - (z2 - z1)*(y3 - y1);
   double ny = (z2 - z1)*(x3 - x1) - (x2 - x1)*(z3 - z1);
   double nz = (x2 - x1)*(y3 - y1) - (y2 - y1)*(x3 - x1); 
   glNormal3f(nx, ny, nz);
   glVertex3d(x1, y1, z1);
   glVertex3d(x2, y2, z2);
   glVertex3d(x3, y3, z3);
   glEnd();
   glPopMatrix(); 
}

// Draws simple 2D square
static void drawSquare(double x1, double z1, double x2, double z2,
                       double x3, double z3, double x4, double z4)
{
    glPushMatrix();
    glBegin(GL_POLYGON);
    glNormal3f(0, 1, 0);
    glVertex3d(x1, 0, z1);
    glNormal3f(0, 1, 0);
    glVertex3d(x2, 0, z2);
    glNormal3f(0, 1, 0);
    glVertex3d(x3, 0, z3);
    glNormal3f(0, 1, 0);
    glVertex3d(x4, 0, z4);
    glNormal3f(0, 1, 0);
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

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
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

// ball for lighting
static void ball(double x, double y, double z, double r)
{
    int th, ph;
    float yellow[] = {1.0, 1.0, 0.0, 1.0};
    float Emission[] = {0.0, 0.0, 0.01 * emission, 1.0};
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x, y, z);
    glScaled(r, r, r);
    //  White ball
    glColor3f(1, 1, 1);
    glMaterialfv(GL_FRONT, GL_SHININESS, shinyvec);
    glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
    glMaterialfv(GL_FRONT, GL_EMISSION, Emission);
    //  Bands of latitude
    for (ph = -90; ph < 90; ph += inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += 2 * inc)
        {
            Vertex(th, ph);
            Vertex(th, ph + inc);
        }
        glEnd();
    }
    //  Undo transofrmations
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
    //  Orthogonal - set world orientation
    if (mode == 0)
    {
        glRotatef(phi, 1, 0, 0);
        glRotatef(theta, 0, 1, 0);
    }
    else if (mode == 1)
    { // Perspective view- set eye position
        double Ex = -2 * dim * Sin(theta) * Cos(phi);
        double Ey = 2 * dim * Sin(phi);
        double Ez = 2 * dim * Cos(theta) * Cos(phi);
        gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(phi), 0);
    }
    // First person view
    else if (mode == 2)
    {
        // Recalculate where the camera is looking
        AX = 2 * dim * Sin(theta); // cameralook at x =AX
        AY = 2 * dim * Sin(phi);
        AZ = -2 * dim * Cos(theta) * Cos(phi);
        // Orient the scene so it imitates first person movement
        gluLookAt(EX, EY, EZ, EX + AX, EY + AY, EZ + AZ, 0, 1, 0);
    }

    // Lighting
    //   Light switch
    if (light)
    {
        //  Translate intensity to color vectors
        float Ambient[] = {0.01 * ambient, 0.01 * ambient, 0.01 * ambient, 1.0};
        float Diffuse[] = {0.01 * diffuse, 0.01 * diffuse, 0.01 * diffuse, 1.0};
        float Specular[] = {0.01 * specular, 0.01 * specular, 0.01 * specular, 1.0};
        //  Light position
        float Position[] = {distance * Cos(zh), ylight, distance * Sin(zh), 1.0};
        //  Draw light position as ball (still no lighting here)
        glColor3f(1, 1, 1);
        ball(Position[0], Position[1], Position[2], 1.0);
        //  OpenGL should normalize normal vectors
        glEnable(GL_NORMALIZE);
        //  Enable lighting
        glEnable(GL_LIGHTING);
        //  Location of viewer for specular calculations
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
        //  glColor sets ambient and diffuse color materials
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);
        //  Enable light 0
        glEnable(GL_LIGHT0);
        //  Set ambient, diffuse, specular components and position of light 0
        glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
        glLightfv(GL_LIGHT0, GL_POSITION, Position);
    }
    else
        glDisable(GL_LIGHTING);

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
    //  Label axes
    glRasterPos3d(AXIS_LENGTH, 0, 0);
    Print("X");
    glRasterPos3d(0, AXIS_LENGTH, 0);
    Print("Y");
    glRasterPos3d(0, 0, AXIS_LENGTH);
    Print("Z");
    //  Display parameters
    //  Display parameters
    glWindowPos2i(5, 5);
    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
          theta, phi, dim, fov, mode ? "Perpective" : "Orthogonal", light ? "On" : "Off");
    if (light)
    {
        glWindowPos2i(5, 45);
        Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f", smooth ? "Smooth" : "Flat", local ? "On" : "Off", distance, ylight);
        glWindowPos2i(5, 25);
        Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f", ambient, diffuse, specular, emission, shinyvec[0]);
    }
    glFlush();
    glutSwapBuffers();
}

void idle()
{
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    zh = fmod(90 * t, 360.0);
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
    //  Exit on ESC
    if (27 == ch)
    {
        exit(0);
    }
    else if ('0' == ch)
    {
        theta = phi = 0; //  Reset view angle to 0,0,0
        dim = 20;
        fov = 60; // reset field of view and dim
        mode = 0; // reset mode to orthogonal
    }
    // Switch display mode between orthogonal(0), perspective(1), and first person(2)
    // each time mode switches the view angle is reset
    else if ('m' == ch)
    {
        mode = (mode + 1) % 3;
        theta = phi = 0;
    }
    else if ('m' == ch || 'M' == ch)
    {
        theta = phi = 0; //  Reset view angle to 0,0,0
        dim = 20;
        fov = 60; // reset view distance
        if (mode < 2)
            mode++;
        else
            mode = 0;
    }
    else if (ch == '1')
    { // switch to perspective view
        mode = 1;
        theta = phi = 0; //  Reset view angle to 0,0,0
        dim = 50;
        fov = 60; // reset view distance
    }
    else if (ch == '2')
    { // set to first person view
        mode = 2;
        theta = phi = 0; //  Reset view angle to 0,0,0
        dim = 50;
        fov = 60; // reset view distance
    }
    //  Change field of view angle
    else if (ch == '-' && ch > 1)
    {
        fov--;
    }
    else if (ch == '+' && ch < 179)
    {
        fov++;
    }
    // Move forward in the scene in first person only
    else if (ch == 'f' || ch == 'F')
    {
        EX += AX * .1;
        EZ += AZ * .1;
    }
    // Move backwards in the scene in first person only
    else if (ch == 'b' || ch == 'B')
    {
        EX -= AX * .1;
        EZ -= AZ * .1;
    }
    // Look down x-axis in orthagonal and perspective
    else if (ch == 'x' && mode != 2)
    {
        theta = -90;
        phi = 0;
    }
    // Look down y-axis in orthagonal and perspective
    else if (ch == 'y' && mode != 2)
    {
        theta = 0;
        phi = 90;
    }
    // Look down z-axis
    else if (ch == 'z')
    {
        theta = phi = 0;
    }

    // Lighting
    else if ('l' == ch || 'L' == ch)
    {
        light = !light;
    }
    else if ('t' == ch || 'T' == ch) // toggle light movement
    {
        move = !move;
    }
    else if ('<' == ch)
    {
        zh += 1;
    }
    else if ('>' == ch)
    {
        zh -= 1;
    }
    //  Light elevation
    else if (ch == '[')
        ylight -= 0.1;
    else if (ch == ']')
        ylight += 0.1;
    //  Ambient level
    else if (ch == 'a' && ambient > 0)
        ambient -= 5;
    else if (ch == 'A' && ambient < 100)
        ambient += 5;
    //  Diffuse level
    else if (ch == 'd' && diffuse > 0)
        diffuse -= 5;
    else if (ch == 'D' && diffuse < 100)
        diffuse += 5;
    //  Specular level
    else if (ch == 's' && specular > 0)
        specular -= 5;
    else if (ch == 'S' && specular < 100)
        specular += 5;
    //  Emission level
    else if (ch == 'e' && emission > 0)
        emission -= 5;
    else if (ch == 'E' && emission < 100)
        emission += 5;
    //  Shininess level
    else if (ch == 'n' && shininess > -1)
        shininess -= 1;
    else if (ch == 'N' && shininess < 7)
        shininess += 1;
    //  Translate shininess power to value (-1 => 0)
    shinyvec[0] = shininess < 0 ? 0 : pow(2.0, shininess);

    //  Reproject
    Project();

    glutIdleFunc(move ? idle : NULL);

    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key, int x, int y)
{
    //  Right arrow key - increase angle by 5 degrees
    if (key == GLUT_KEY_RIGHT)
        theta += 5;
    //  Left arrow key - decrease angle by 5 degrees
    else if (key == GLUT_KEY_LEFT)
        theta -= 5;
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
        phi += 5;
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
        phi -= 5;
    //  Keep angles to +/-360 degrees
    theta %= 360;
    phi %= 360;
    //  Update projection
    Project();
    //  Tell GLUT it is necessary to redisplay the scene
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
    glutInitWindowSize(550, 550);
    //  Create the window
    glutCreateWindow("HW5 Julia Troni");
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
    // Tell GLUT to call "idle" when nothing else is going on
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}