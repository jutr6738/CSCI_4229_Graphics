#include "CSCIx229.h"

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch, int x, int y)
{
  // Exit on ESC
  if (ch == 27)
  {
    exit(0);
  }
  // Reset view angle
  else if (ch == '0')
  {
    th = ph = 0; //  Reset view angle to 0,0,0
    axes = 1;
    th = ph = 0;
    fov = 55;
    dim = 5;  // reset dim
    mode = 0; // reset mode to orthogonal

    light_move = 1;
    light_distance = 5;
    light_height = 3;
  }
  // Toggle axes
  else if (ch == 'a' || ch == 'A')
  {
    axes = 1 - axes;
  }
  // Switch display mode between orthogonal(0), perspective(1), and first person(2)
  // each time mode switches the view angle is reset
  else if ('m' == ch || 'M' == ch)
  {
    th = ph = 0; //  Reset view angle to 0,0,0
    dim = 5;
    fov = 55; // reset view distance
    if (mode < 2)
      mode++;
    else
      mode = 0;
  }
  else if (ch == '1')
  { // switch to perspective view
    mode = 1;
    th = ph = 0; //  Reset view angle to 0,0,0
    dim = 50;
    fov = 60; // reset view distance
  }
  else if (ch == '2')
  { // set to first person view
    mode = 2;
    th = ph = 0; //  Reset view angle to 0,0,0
    dim = 50;
    fov = 60; // reset view distance
  }
  //  Change field of view angle
  else if (ch == '-' && ch > 1)
  {
    //fov--;
    dim++;
  }
  else if (ch == '+' && ch < 179)
  {
   // fov++;
    dim--;
  }
  // Move forward in the scene
  else if (ch == 'f' || ch == 'F')
  {
    EX += AX * .1;
    EZ += AZ * .1;
  }
  // Move backwards in the scene
  else if (ch == 'b' || ch == 'B')
  {
    EX -= AX * .1;
    EZ -= AZ * .1;
  }
  // Look down x-axis
  else if (ch == 'x' && mode != 2)
  {
    th = -90;
    ph = 0;
  }
  // Look down y-axis
  else if (ch == 'y' && mode != 2)
  {
    th = 0;
    ph = 90;
  }
  // Look down z-axis
  else if (ch == 'z' && mode != 2)
  {
    th = ph = 0;
  }
  // Increase light height
  else if (ch == '[' && light_height < 10)
  {
    light_height++;
  }
  // Decrease light height
  else if (ch == ']' && light_height > 0)
  {
    light_height--;
  }
  // Lighting off/on
  else if ('l' == ch || 'L' == ch)
  {
    light = !light;
  }
  else if ('t' == ch || 'T' == ch) // toggle light movement
  {
    light_move = !light_move;
  }
  // Raise light distance
  else if (ch == 'R')
  {
    light_distance++;
  }
  // Decrease light distance
  else if (ch == 'r')
  {
    if (light_distance > 0)
      light_distance--;
  }
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

  Project(fov, asp, dim);

  if (light_move)
  {
    glutIdleFunc(idle);
  }
  else
  {
    glutIdleFunc(NULL);
  }
  glutPostRedisplay();
}