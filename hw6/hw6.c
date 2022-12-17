/*
Julia Troni
CSCI 4229 Graphics
HW6: Lighting and textures

 */
#include "CSCIx229.h"

// Display axes
int axes = 1;
// Azimuth of view angle
int th = 0;
// Elevation of view angle
int ph = 50;
// Color mode
int mode = 0;
// Dimension of orthogonal box
double dim = 3.0;
// Length of axes
double len = 2;
// Maximum number
int n = 50000;
// Field of view (for perspective)
int fov = 80;
// Aspect ratio
double asp = 1;
// View mode for print
char *views[] = {"Orthogonal", "Perspective", "First Person"};
// X-coordinate of camera position
double EX = 0;
// Y-coordinate of camera position
double EY = 0;
// Z-coordinate of camera position
double EZ = 10;
// X-coordinate of where the camera is looking
double AX = 0;
// Y-coordinate of where the camera is looking
double AY = 0;
// Z-coordinate of where the camera is looking
double AZ = 0;

// Lighting
int light = 1;
// Ambient light
int ambient = 30;
// Diffuse light
int diffuse = 100;
// Emission light
int emission = 0;
// Specular light
int specular = 0;
// Light distance
int light_distance = 5;
// Light angle
int light_angle = 90;
// Light height
int light_height = 3;
// Light move
int light_move = 1;

#define Y_GROUND 0
#define GROUND_WIDTH 3 * dim
#define GROUND_LENGTH 4 * dim

unsigned int leafTexture;
unsigned int woodTexture;
unsigned int brickTexture;
unsigned int roofTexture;
unsigned int grassTexture;

// Utility function to add texture
void Material(float *shininess, float *color, float *emit)
{
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT, GL_SPECULAR, color);
  glMaterialfv(GL_FRONT, GL_EMISSION, emit);
}

/*
 *  Draw a sphere at (x,y,z) radius (r)
 */
void Sun(double x, double y, double z, double radius)
{
  const int d = 5;
  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslatef(x, y, z);
  glScalef(radius, radius, radius);

  glColor3f(1, 1, 1);
  float shininess[] = {0};
  float color[] = {1, 1, 1, 1.0};
  float emit[] = {0.0, 0.0, 0.01 * emission, 1.0};
  Material(shininess, color, emit);

  for (int ph = -90; ph < 90; ph += d)
  {
    glBegin(GL_QUAD_STRIP);
    for (int th = 0; th <= 360; th += d)
    {
      double x_1 = Sin(th) * Cos(ph);
      double y_1 = Cos(th) * Cos(ph);
      double z_1 = Sin(ph);
      glNormal3d(x_1, y_1, z_1);
      glVertex3d(x_1, y_1, z_1);

      double x_2 = Sin(th) * Cos(ph + d);
      double y_2 = Cos(th) * Cos(ph + d);
      double z_2 = Sin(ph + d);
      glNormal3d(x_2, y_2, z_2);
      glVertex3d(x_2, y_2, z_2);
    }
    glEnd();
  }

  //  Undo transformations
  glPopMatrix();
}

void Cylinder(double x, double y, double z, double radius, double height)
{
  const int d = 5;
  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslatef(x, y, z);
  glScalef(radius, height, radius);

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 0, 0);
  glTexCoord2f(0.5, 0.5);
  for (int th = 0; th <= 360; th += d)
  {
    // glColor3f(0.4*Cos(th), 0.4 , 0.4*Sin(th));
    glColor3f(1, 1, 1);
    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th) + 0.5, Cos(th) + 0.5);
    glVertex3f(Sin(th), 0, Cos(th));
  }
  glEnd();

  // Latitude bands
  for (int th = 0; th <= 360; th += d)
  {
    glBegin(GL_QUADS);
    // glColor3f(0.4*Cos(th), 0.4 , 0.4*Sin(th));
    glColor3f(1, 1, 1);

    float shininess[] = {0};
    // float color[] = {0.4*Cos(th), 0.4, 0.4*Sin(th), 1.0};
    float color[] = {1, 1, 1, 1.0};
    float emit[] = {0.0, 0.0, 0.01 * emission, 1.0};
    Material(shininess, color, emit);

    float tc = th / (float)360;
    float tcd = (th + d) / (float)360;

    glTexCoord2f(tc, 0);
    glVertex3f(Sin(th), 0, Cos(th));

    glTexCoord2f(tc, 1);
    glVertex3f(Sin(th), 1, Cos(th));

    glTexCoord2f(tcd, 1);
    glVertex3f(Sin(th + d), 1, Cos(th + d));

    glTexCoord2f(tcd, 0);
    glVertex3f(Sin(th + d), 0, Cos(th + d));
    glEnd();
  }

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 1, 0);
  for (int th = 0; th <= 360; th += d)
  {
    // glColor3f(0.4*Cos(th), 0.4 , 0.4*Sin(th));
    glColor3f(1, 1, 1);
    glTexCoord2f(Cos(th) + 0.5, Sin(th) + 0.5);
    glVertex3f(Sin(th), 1, Cos(th));
  }
  glEnd();
  //  Undo transformations
  glPopMatrix();
}

/*
 *  Draw a cone at (x,y,z) radius (r)
 */
void Cone(double x, double y, double z, double radius, double height)
{
  const int d = 5;
  float shininess[] = {0};
  float color[] = {1, 1, 1, 1.0};
  float emit[] = {0.0, 0.0, 0.01 * emission, 1.0};
  float phi = 1 / (sqrt(pow(height, 2) + 1));

  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslatef(x, y, z);
  glScalef(radius, height, radius);

  // // base
  glBegin(GL_TRIANGLE_FAN);
  glTexCoord2f(0.5, 0.5);
  glVertex3f(0, 0, 0);

  for (int th = 0; th <= 360; th += d)
  {
    glColor3f(1, 1, 1);

    Material(shininess, color, emit);

    glNormal3d(Sin(th), 0, Cos(th));
    glTexCoord2f(Sin(th) + 0.5, Cos(th) + 0.5);
    glVertex3f(Sin(th), 0, Cos(th));
  }
  glEnd();

  // wrap around
  glBegin(GL_TRIANGLES);
  for (int th = 0; th < 360; th += d) // stop before 360 since last triangle only needs one point
  {
    glColor3f(1, 1, 1);

    Material(shininess, color, emit);

    // bottom
    glNormal3d(Sin(th), phi, Cos(th)); // start
    glTexCoord2f(Sin(th) + 0.5, Cos(th) + 0.5);
    glVertex3f(Sin(th), 0, Cos(th));

    // top of triangle vertex
    glNormal3d(Sin(th + d / 2), phi, Cos(th + d / 2));
    glTexCoord2f(Sin(th + d / 2) + 0.5, Cos(th + d / 2) + 0.5);
    glVertex3f(0, height, 0);

    // bottom of triangle vertex
    glNormal3d(Sin(th + d), phi, Cos(th + d));
    glTexCoord2f(Sin(th + d) + 0.5, Cos(th + d) + 0.5);
    glVertex3f(Sin(th + d), 0, Cos(th + d));
  }
  glEnd();

  //  Undo transformations
  glPopMatrix();
}

/*
 *  Draw a cube at (x,y,z)
 *  with height (h), length (l), width (w)
 *  at angle (angle) on x (ax), y (ay), or z (az)
 *
 */
void Cube(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az)
{
  //  Save transformation
  glPushMatrix();
  //  Offset, scale
  glTranslatef(x, y, z);
  glRotatef(angle, ax, ay, az);
  glScalef(width, height, depth);
  glColor3f(1, 1, 1);

  float shininess[] = {0};
  float color[] = {1, 1, 1, 1.0};
  float emit[] = {0.0, 0.0, 0.01 * emission, 1.0};
  Material(shininess, color, emit);
  //  Cube
  glBegin(GL_QUADS);
  glNormal3f(0, 0, +1);
  glTexCoord2f(0, 0);
  glVertex3f(-1, 0, +1);
  glTexCoord2f(1, 0);
  glVertex3f(+1, 0, +1);
  glTexCoord2f(1, 1);
  glVertex3f(+1, +1, +1);
  glTexCoord2f(0, 1);
  glVertex3f(-1, +1, +1);
  //  Back
  glNormal3f(0, 0, -1);
  glTexCoord2f(0, 0);
  glVertex3f(+1, 0, -1);
  glTexCoord2f(1, 0);
  glVertex3f(-1, 0, -1);
  glTexCoord2f(1, 1);
  glVertex3f(-1, +1, -1);
  glTexCoord2f(0, 1);
  glVertex3f(+1, +1, -1);
  //  Right
  glNormal3f(+1, 0, 0);
  glTexCoord2f(0, 0);
  glVertex3f(+1, 0, +1);
  glTexCoord2f(1, 0);
  glVertex3f(+1, 0, -1);
  glTexCoord2f(1, 1);
  glVertex3f(+1, +1, -1);
  glTexCoord2f(0, 1);
  glVertex3f(+1, +1, +1);
  //  Left
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-1, 0, -1);
  glTexCoord2f(1, 0);
  glVertex3f(-1, 0, +1);
  glTexCoord2f(1, 1);
  glVertex3f(-1, +1, +1);
  glTexCoord2f(0, 1);
  glVertex3f(-1, +1, -1);
  //  Top
  glNormal3f(0, +1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-1, +1, +1);
  glTexCoord2f(1, 0);
  glVertex3f(+1, +1, +1);
  glTexCoord2f(1, 1);
  glVertex3f(+1, +1, -1);
  glTexCoord2f(0, 1);
  glVertex3f(-1, +1, -1);
  //  Bottom
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-1, 0, -1);
  glTexCoord2f(1, 0);
  glVertex3f(+1, 0, -1);
  glTexCoord2f(1, 1);
  glVertex3f(+1, 0, +1);
  glTexCoord2f(0, 1);
  glVertex3f(-1, 0, +1);
  //  End
  glEnd();
  //  Undo transformations
  glPopMatrix();
}

void Roof(double x, double y, double z, double width, double height, double depth, double angle, double ax, double ay, double az)
{

  //  Save transformation
  glPushMatrix();
  //  Offset, scale
  glTranslatef(x, y, z);
  glRotatef(angle, ax, ay, az);
  glScalef(width, height, depth);

  // glColor3f(1, 0, 0);
  glColor3f(1, 1, 1);

  //  Front
  glBegin(GL_TRIANGLES);
  glNormal3f(-0.6, 0.3, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-1, -1, +1);
  glTexCoord2f(3, 0);
  glVertex3f(-1, -1, -1);
  glTexCoord2f(3, 1.5);
  glVertex3f(0, +1, 0);
  glEnd();
  //  Back
  glBegin(GL_TRIANGLES);
  glNormal3f(0, 0.3, -0.6);
  glTexCoord2f(0, 0);
  glVertex3f(+1, -1, -1);
  glTexCoord2f(3, 0);
  glVertex3f(-1, -1, -1);
  glTexCoord2f(3, 1.5);
  glVertex3f(0, +1, 0);
  glEnd();
  //  Right
  glBegin(GL_TRIANGLES);
  glNormal3f(0, -.3, -0.6);
  glTexCoord2f(0, 0);
  glVertex3f(+1, -1, +1);
  glTexCoord2f(3, 0);
  glVertex3f(-1, -1, +1);
  glTexCoord2f(3, 1.5);
  glVertex3f(0, +1, 0);
  glEnd();
  //  Left
  glBegin(GL_TRIANGLES);
  glNormal3f(0.6, 0.3, 0);
  glTexCoord2f(0, 0);
  glVertex3f(+1, -1, +1);
  glTexCoord2f(3, 0);
  glVertex3f(+1, -1, -1);
  glTexCoord2f(3, 1.5);
  glVertex3f(0, +1, 0);
  glEnd();
  //  Bottom
  glBegin(GL_QUADS);
  glNormal3f(0, -1, 0);
  glTexCoord2f(0, 0);
  glVertex3f(-1, -1, -1);
  glTexCoord2f(0, 0);
  glVertex3f(+1, -1, -1);
  glTexCoord2f(1, 1);
  glVertex3f(+1, -1, +1);
  glTexCoord2f(0, 1);
  glVertex3f(-1, -1, +1);
  glEnd();

  //  Undo transformations
  glPopMatrix();
}

/*
 *  Draw a house at (x,y,z)
 *  with height (h), length (l), width (w)
 *  at angle (angle) on x (ax), y (ay), or z (az)
 *
 */

void House(double x, double y, double z, double width, double height, double length, double angle, double ax, double ay, double az)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, brickTexture);
  Cube(x, y, z, width, height, length, angle, ax, ay, az);

  glBindTexture(GL_TEXTURE_2D, roofTexture);
  Roof(x, y + height * 1.5, z, width + 0.1, height / 2, length + 0.1, angle, ax, ay, az);

  glDisable(GL_TEXTURE_2D);
}

void Tree(double x, double y, double z, double radius, double height)
{

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, leafTexture);
  Cone(x, height, z, radius, height);

  glBindTexture(GL_TEXTURE_2D, woodTexture);
  Cylinder(x, y, z, radius / 5, height);
  glDisable(GL_TEXTURE_2D);
}

// Draw ground plane
void drawGround()
{
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);
  glBindTexture(GL_TEXTURE_2D, grassTexture); // add grass texture
  glNormal3f(0, 1, 0);
  float i, j;
  float delta = 0.4;
  for (i = -2; i <= 2 - delta; i += delta)
  {
    glBegin(GL_QUAD_STRIP);
    for (j = -2; j <= 2 - delta; j += delta)
    {
      glTexCoord2f(0, 0);
      glVertex3f(i * GROUND_WIDTH, Y_GROUND, j * GROUND_LENGTH);
      glTexCoord2f(1, 0);
      glVertex3f((i + delta) * GROUND_WIDTH, Y_GROUND, j * GROUND_LENGTH);
      glTexCoord2f(0, 1);
      glVertex3f(i * GROUND_WIDTH, Y_GROUND, (j + delta) * GROUND_LENGTH);
      glTexCoord2f(1, 1);
      glVertex3f((i + delta) * GROUND_WIDTH, Y_GROUND, (j + delta) * GROUND_LENGTH);
    }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glPopMatrix();
}

void Town()
{

  for (int i = -1; i < 2; i++)
  {
    if (i == 0)
      continue;
    double dist = 0.5 * i;
    House(0, 0, 0, 0.3, 0.5, 0.4, 0, 0, 0, 0);
    House(-dist * 3, 0, -dist * 4, 0.2, 1, 0.3, 45, 0, 1, 0);
    House(dist * 4, 0, 0, 0.5, 0.4, 0.6, 30, 0, 1, 0);
    House(dist * 1.5, 0, -dist * 4, 0.2, 1, 0.3, 20, 0, 1, 0);

    Tree(dist * 4, 0, dist * 2, 0.2, 0.3);
    Tree(dist, 0, -dist * 0.4, 0.2, 0.6);
    Tree(dist, 0, dist * 2, 0.2, 0.5);
    Tree(dist * 4, 0, -dist * 2, 0.15, 0.3);
    Tree(-dist * 1.5, 0, dist * 4, 0.15, 0.4);
    Tree(dist * 1.5, 0, dist * 4, 0.3, 0.9);
  }
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
  // Erase the window and the depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Enable Z-buffering in OpenGL
  glEnable(GL_DEPTH_TEST);
  // Undo previous transformations
  glLoadIdentity();

  // switch between the different views
  //  Orthogonal - set world orientation
  if (mode == 0)
  {
    glRotatef(ph, 1, 0, 0);
    glRotatef(th, 0, 1, 0);
  }
  else if (mode == 1)
  { // Perspective view- set eye position
    double Ex = -2 * dim * Sin(th) * Cos(ph);
    double Ey = 2 * dim * Sin(ph);
    double Ez = 2 * dim * Cos(th) * Cos(ph);
    gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
  }
  // First person view
  else if (mode == 2)
  {
    // Recalculate where the camera is looking
    AX = 2 * dim * Sin(th); // cameralook at x =AX
    AY = 2 * dim * Sin(ph);
    AZ = -2 * dim * Cos(th) * Cos(ph);
    // Orient the scene so it imitates first person movement
    gluLookAt(EX, EY, EZ, EX + AX, EY + AY, EZ + AZ, 0, 1, 0);
  }

  if (light)
  {
    //  Translate intensity to color vectors
    float Ambient[] = {0.01 * ambient, 0.01 * ambient, 0.01 * ambient, 1.0};
    float Diffuse[] = {0.01 * diffuse, 0.01 * diffuse, 0.01 * diffuse, 1.0};
    float Specular[] = {0.01 * specular, 0.01 * specular, 0.01 * specular, 1.0};
    //  Light position
    float Position[] = {light_distance * Cos(light_angle), light_height, light_distance * Sin(light_angle), 1.0};

    //  Draw light position as ball (still no lighting here)
    glColor3f(1, 1, 1);
    Sun(Position[0], Position[1], Position[2], 0.3);
    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
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
  {
    glDisable(GL_LIGHTING);
  }

  glPushMatrix();
  glColor3f(0, 0, 1);
  Town();
  drawGround();
  glPopMatrix();

  // Draw axes (white)
  glDisable(GL_LIGHTING);
  glColor3f(1, 1, 1);
  if (axes)
  {
    glBegin(GL_LINES);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(len, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, len, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, len);
    glEnd();
    // Label axes
    glRasterPos3d(len, 0.0, 0.0);
    Print("X");
    glRasterPos3d(0.0, len, 0.0);
    Print("Y");
    glRasterPos3d(0.0, 0.0, len);
    Print("Z");
  }

  // Display parameters
  glWindowPos2i(5, 5);
  Print("Angle=%d,%d  Axes=%s  Dim=%.1f  Projection=%s  LightHeight=%d  LightMove=%s", th, ph, axes ? "On" : "Off", dim, views[mode], light_height, light_move ? "Move" : "Stop");

  // Render the scene and make it visible
  glFlush();
  glutSwapBuffers();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc, char *argv[])
{
  // Initialize GLUT and process user parameters
  glutInit(&argc, argv);
  // Request double buffered, true color window with Z buffering
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  // Request 800 x 800 pixel window
  glutInitWindowSize(800, 800);
  // Create the window
  glutCreateWindow("HW6: Julia Troni");
  // Tell GLUT to call "display" when the scene should be drawn

#ifdef USEGLEW
  //  Initialize GLEW
  if (glewInit() != GLEW_OK)
    Fatal("Error initializing GLEW\n");
#endif
  glutDisplayFunc(display);
  // Tell GLUT to call "reshape" when the window is resized
  glutReshapeFunc(reshape);
  // Tell GLUT to call "special" when an arrow key is pressed
  glutSpecialFunc(special);
  // Tell GLUT to call "key" when a key is pressed
  glutKeyboardFunc(key);
  // Tell GLUT to call "idle" when nothing else is going on
  glutIdleFunc(idle);
  // Pass control to GLUT so it can interact with the user

  leafTexture = LoadTexBMP("textures/cactus.bmp");
  woodTexture = LoadTexBMP("textures/wood.bmp");
  brickTexture = LoadTexBMP("textures/brick.bmp");
  roofTexture = LoadTexBMP("textures/roof-2.bmp");
  grassTexture = LoadTexBMP("textures/leaf.bmp");

  glutMainLoop();
  // Return code

  return 0;
}