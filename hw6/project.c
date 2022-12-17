/*
 *  Set projection
 */
#include "CSCIx229.h"

void Project(double fov,double asp,double dim)
{
  //  Tell OpenGL we want to manipulate the projection matrix
  glMatrixMode(GL_PROJECTION);
  //  Undo previous transformations
  glLoadIdentity();

  // //  Perspective transformation
  // if (fov)
  //   gluPerspective(fov,asp,dim/4,4*dim);
  // //  Orthogonal transformation
  // else
  //   glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
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
