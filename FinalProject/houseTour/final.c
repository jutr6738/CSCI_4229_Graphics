/*
 *  Project
 *
 *  3D Village and House Tour
 *
 * Key bindings:
 *
 * ESC     				Exit
 * 0       				Reset view angle
 * n/N					First Person Navigation Mode
 * b/B					Top view of the scene - First Person Navigation Mode
 * j/J					Hall view - First Person Navigation Mode
 * i/I					Bed room view - First Person Navigation Mode
 * k/K					Pent House View - First Person Navigation Mode
 * r/R					Picnic View - First Person Navigation Mode
 * +/-  				Zoom In/Out (Decrease/Increase Field of View)
 * u/U					Toggle axes
 * o/O 					Orthogonal Mode
 * p/P 					Perspective Mode
 * l/L         			Toggle lighting
 * a/A        			Decrease/Increase ambient light
 * d/D        			Decrease/Increase diffuse light
 * s/S        			Decrease/Increase specular light
 * e/E        			Decrease/Increase emitted light
 * h/H        			Decrease/Increase shininess
 * m/M          		Play/Pause light movement
 * f/F					Switch on/off fans
 * g/G 					Open/Close main gate
 * v/V					Increase/Decrease waves in swimming pool
 * w/W					Play seesaw
 * x/X					Move eye position left/right 
 * y/Y					Move eye position up/down 
 * z/Z					Move eye position forward/backward
 * t/T					Decrease/Increase camera's azimuth angle
 * c/C					Decrease/Increase camera's elevation angle
 * <>					Increase/Decrease light Azimuth angle
 * []         			Lower/rise light
 * Up/Down arrows   	Increase/Decrease elevation angle (Move Forward/Backward in First Person Navigation Mode)
 * Left/Right arrows	Increase/Decrease Azimuth angle (Move Left/Right in First Person Navigation Mode)
 * PageUp/PageDown		Climb up/down stairs in First Person Navigation Mode
 * F1/F2				Zoom In/Zoom Out the scene (Decrease/Increase dim)
 * F3/F4				Increase/Decrease azimuth angle
 * F5/F6				Increase/Decrease elevation angle
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int colFlag =0;
int axes=0;       //  Display axes
int mode=2;       //  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov= 80;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=80;   //  Size of world
double rep = 1;  //  Repetition
double rep_x=1;  //  Repetition
double rep_y=1;  //  Repetition
unsigned int texture[30];  //  Texture names
// Azimuth and Elevation Angles of camera for First Person Navigation
int th_camera = 0;
int ph_camera = 0;
// Fixing initial eye position
double Ex = 2;
double Ey = 10;
double Ez = 160;
int object = 0; // Identifies the type of object for appropriate texture
int light=0;      //  Lighting on/off
int alpha=100;    //  Transparency
int move=1;       //  Move light (Play/Pause Light)
int zh        =  90;  // Light azimuth
int distance  =   150;  // Light distance
float ylight  =   30;  // Elevation of light
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
float white[] = {1,1,1,1};
float black[] = {0,0,0,1};
float yellow[] = {1,1,0,1};
int wflag = 1;
int seesawFlag =1;
int fanFlag =1;
double fanAngle=0,fanSpeed=15;
int gateFlag = 1;
struct tm *newtime;
time_t ltime;
float z=5;
#define MAXQUADS 300
int phaseInc=10;
unsigned long int phaseAngle=5;
double stepSize=1;
double vertex[MAXQUADS][MAXQUADS][3];
double normal[MAXQUADS][MAXQUADS][3];


 int objectMap[5][6] = {
	 {20, 25, 0, 50, -150, 50}, 
	 {20, 25, 0, 50, 80, 160},
	 {20, 140, 0, 50, -150, -145},
	 {139,140, 0, 50, -150, 170},
	 {20, 140, 0, 50, 169, 170}	
 };

#include "CSCIx229.h"


/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */

void cube(
	double x,double y,double z,double dx,double dy,double dz,
    double th)
{
   // Define Material for specular 
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   
   if(object == 0) // Do not apply texture
   {
	   glDisable(GL_TEXTURE_2D);
   }
   else if( object == 1) // Road
   {
   		rep_x = 1;
   		rep_y = 20;
   		glBindTexture(GL_TEXTURE_2D,texture[5]);
   }
   else if(object == 2) // House
   {
   		rep_x = 4;
   		rep_y = 1;
   		glBindTexture(GL_TEXTURE_2D,texture[0]);
   }
   else if(object == 3) // Grass
   {
   		rep_x = 1;
   		rep_y = 4;
   		glBindTexture(GL_TEXTURE_2D,texture[6]);
   }
   else if(object == 4) // Footpath
   {
   		rep_x = 1;
   		rep_y = 20;
   		glBindTexture(GL_TEXTURE_2D,texture[7]);
   }
   else if(object == 6) // Table
   {
	   glBindTexture(GL_TEXTURE_2D,texture[13]);
   }
    else if(object == 8) // Couch Rest
   {
   		rep_x = 1;
   		rep_y = 1;
   		glBindTexture(GL_TEXTURE_2D,texture[10]);
   }
   else if(object == 9) // Couch Base
   {
   		rep_x = 1;
   		rep_y = 1;
   		glBindTexture(GL_TEXTURE_2D,texture[9]);
   }
   else if(object == 10) // Pool
   {
   		rep_x = 1;
   		rep_y = 1;
   		glBindTexture(GL_TEXTURE_2D,texture[17]);
		glColor3f(1,1,1);
   }
   else if(object == 11) // Pool Borders
   {
   		rep_x = 1;
   		rep_y = 1;
   		glBindTexture(GL_TEXTURE_2D,texture[19]);
   }
   else if(object == 13) // Bed
   {
   		rep_x = 1;
   		rep_y = 1;
   		glBindTexture(GL_TEXTURE_2D,texture[14]);
   }
   else if(object == 14) // Steps
   {
   		rep_x = 4;
   		rep_y = 1;
   		glBindTexture(GL_TEXTURE_2D,texture[16]);
   }
	   
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1, 1);
   
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(+1,+1,-1);
   
   //  Right
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(+1,+1,+1);
   
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,-1);
   
   //  Top
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,-1);
   
   //  Bottom
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,-1,+1);
   
   glEnd();
   glPopMatrix();
   // Disable textures
   glDisable(GL_TEXTURE_2D);
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
 *     with radius r
 *     
 */
static void sphere(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(r,r,r);
   
   //  White ball
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }
   glPopMatrix();
}

static void Vertex1(double th,double ph)
{
   rep = 1; //20;
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glTexCoord2d(rep*th/360.0,rep*ph/180.0+0.5);
   glVertex3d(x,y,z);

}

static void sphere1(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   
   glPushMatrix();
   glTranslated(x,y,z);
   glScaled(r,r,r);
   
   //  White ball
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   
    // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   if(object == 5) // Leaves
   {
   		glBindTexture(GL_TEXTURE_2D,texture[28]);
   } else if(object == 15) // Leaves
   {
   		glBindTexture(GL_TEXTURE_2D,texture[24]);
   }

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex1(th,ph);
         Vertex1(th,ph+d);
      }
      glEnd();
   }
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}


/*
 *  Draw a mainHouseBase
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
void mainHouseBase(double x,double y,double z,double dx,double dy,double dz,double th)
{  
   rep_x = 4;
   rep_y = 1;
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
    // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[0]);
 
   //  Front
   glBegin(GL_QUADS);
   glColor3f(0.85,0.53,0.1);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1, 1);
   
   
   //  left
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(+1,+1,-1);
   
   //  back
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(+1,+1,+1);
   glEnd();
   
   // Inner walls   
   
   //  Left
   glBindTexture(GL_TEXTURE_2D,texture[27]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   glColor3f(1,1,1);   
   glTexCoord2f(0,0); glVertex3f(+1,-1,-0.99);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-0.99);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-0.99);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-0.99);
   glEnd();
   
   //  main entrace wall left
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   glBegin(GL_QUADS);
   glColor3f(0.85,0.53,0.1);
   glNormal3f(-1, 0, 0);
   
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(-1,-1,-0.2);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-1,+1,-0.2);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,-1);
   
      //  main entrace wall right
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,0.2);
   glTexCoord2f(rep_x,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,0.2);
   
    //  main entrace wall top
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,0.4,-0.2);
   glTexCoord2f(0.75,0); glVertex3f(-1,0.4,0.2);
   glTexCoord2f(0.75,0.5); glVertex3f(-1,+1,0.2);
   glTexCoord2f(0,0.5); glVertex3f(-1,+1,-0.2);
   glEnd();
   
   //  main entrace wall left inner
   glColor3f(0.258824,0.258824,0.435294);
   glBegin(GL_QUADS);
   glVertex3f(-0.99,-1,-1);
   glVertex3f(-0.99,-1,-0.2);
   glVertex3f(-0.99,+1,-0.2);
   glVertex3f(-0.99,+1,-1);
     
   //  main entrace wall right Inner
   glVertex3f(-0.99,-1,0.2);
   glVertex3f(-0.99,-1,+1);
   glVertex3f(-0.99,+1,+1);
   glVertex3f(-0.99,+1,0.2);
   
   //  main entrace wall top inner
   glVertex3f(-0.99,0.4,-0.2);
   glVertex3f(-0.99,0.4,0.2);
   glVertex3f(-0.99,+1,0.2);
   glVertex3f(-0.99,+1,-0.2);  
   
   //  Top
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,0.18);
   glTexCoord2f(rep_x,0); glVertex3f(+1,+1,+0.18);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,-1);
   
   //  Top right 
   glNormal3f( 0, 1, 0);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(-1,+1,1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1,0.3);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,0.3);
   
    //  Top gap front
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,0.3);
   glTexCoord2f(rep_x,0); glVertex3f(-0.2,+1,0.3);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-0.2,+1,0.18);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,0.18);
   
    //  Top gap back
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0,0); glVertex3f(0.2,+1,0.3);
   glTexCoord2f(rep_x,0); glVertex3f(1,+1,0.3);
   glTexCoord2f(rep_x,rep_y); glVertex3f(1,+1,0.18);
   glTexCoord2f(0,rep_y); glVertex3f(0.2,+1,0.18);
     
   //  Bottom
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,-1,+1);
   glEnd();
   
   //  Bottom Inner
   glBindTexture(GL_TEXTURE_2D,texture[26]);
   glBegin(GL_QUADS);
   glColor3f(1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1,-0.99,-1);
   glTexCoord2f(5,0); glVertex3f(+1,-0.99,-1);
   glTexCoord2f(5,5); glVertex3f(+1,-0.99,+1);
   glTexCoord2f(0,5); glVertex3f(-1,-0.99,+1);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   // Back Inner

   glBegin(GL_QUADS);
   glColor3f(0.258824,0.258824,0.435294);
   glTexCoord2f(0,0); glVertex3f(+0.94,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+0.94,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+0.94,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+0.94,+1,+1); 
   
   //  Front Inner
   glNormal3f( 0, 0, 1); 
   glTexCoord2f(0,0); glVertex3f(-1,-1, 0.94);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 0.94);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 0.94);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 0.94);
   glEnd();
   
   glPopMatrix();
}

void pcube(
	double x,double y,double z,double dx,double dy,double dz,
    double th)
{
   // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   rep_x = 3;
   rep_y = 1;
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1, 1);
   
   //  Back
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(+1,+1,-1);
   
   //  Right
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(+1,+1,+1);
   
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep_x,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,-1);
   
   //  Top
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,+1,-1);
   
   //  Bottom
  //Bottom
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-0.99,-1);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-0.99,-1);
   glTexCoord2f(rep_x,rep_y); glVertex3f(+1,-0.99,+0.7);
   glTexCoord2f(0,rep_y); glVertex3f(-1,-0.99,+0.7);
   
   //Bottom steps way
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-0.99,0.7);
   glTexCoord2f(rep_x,0); glVertex3f(-0.2,-0.99,0.7);
   glTexCoord2f(rep_x,rep_y); glVertex3f(-0.2,-0.99,+1);
   glTexCoord2f(0,rep_y); glVertex3f(-1,-0.99,+1);
   
    //Bottom steps way
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(0.6,-0.99,0.7);
   glTexCoord2f(rep_x,0); glVertex3f(+1,-0.99,0.7);
   glTexCoord2f(rep_x,rep_y); glVertex3f(1,-0.99,+1);
   glTexCoord2f(0,rep_y); glVertex3f(0.6,-0.99,+1);
   
   
   glEnd();
   glPopMatrix();
   // Disable textures
   glDisable(GL_TEXTURE_2D);
}


/*
 *  Draw a Cylinder
 *     at (x,y,z)
 *     with radius r and height h
 *     rotated th about the y axis and ph about the x axis
 */

void cylinder (double x, double y, double z, double th, double ph, double r, double h)
{
	int i,k;
	
   // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
 
    // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(r,r,h);

   if(object == 5) // Tree Trunk
	   glBindTexture(GL_TEXTURE_2D,texture[8]);
   else if(object == 6) // Chairs Top
	   glBindTexture(GL_TEXTURE_2D,texture[9]);
	   
   // Top and Bottom
   for (i=1;i>=-1;i-=2)
   {
      glNormal3f(0,0,i);
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.5,0.5);
      glVertex3f(0,0,i);
      for (k=0;k<=360;k+=10)
      {
         glTexCoord2f(0.5*Cos(k)+0.5,0.5*Sin(k)+0.5);
         glVertex3f(i*Cos(k),Sin(k),i);
      }
      glEnd();
   }
   //  Body

   if(object == 6) { //Chairs legs
	   glBindTexture(GL_TEXTURE_2D,texture[10]);
   }
   
   glBegin(GL_QUAD_STRIP);
   for (k=0;k<=360;k+=10)
   {
      glNormal3f(Cos(k),Sin(k),0);
      glTexCoord2f(0,0.5*k); 
	  glVertex3f(Cos(k),Sin(k),+1);
      glTexCoord2f(1,0.5*k); 
	  glVertex3f(Cos(k),Sin(k),-1);
   }
   glEnd();
   glPopMatrix();
   // Disable textures
   glDisable(GL_TEXTURE_2D);
 
}

/*
 *  Draw a Cone
 *     at (x,y,z)
 *     with radius r and height h
 *     rotated th about the y axis and ph about the x axis
 */
void cone(double x, double y, double z, double th, double ph, double r, double h)
{
	int inc =  5;
	
   // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
 
    // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(r,r,h);
	
   if(object == 5) // Tree Leaves
   {
   		glBindTexture(GL_TEXTURE_2D,texture[11]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   }
   if(object == 7)
   {
	   glBindTexture(GL_TEXTURE_2D,texture[12]);
	   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   }
	   
	// Sides

	glBegin(GL_TRIANGLES);
	for(th = 0; th <= 360; th+= 2*inc)
	{
		glNormal3f(Cos(th-inc),Sin(th-inc),1);
		glTexCoord2f(0,0);
	    glVertex3f(Cos(th-inc),Sin(th-inc),0);

		glNormal3f(Cos(th),Sin(th),1);
		glTexCoord2f(0.5,h);
	    glVertex3f(0,0,1);

		glNormal3f(Cos(th+inc),Sin(th+inc),1);
		glTexCoord2f(1,0);
	    glVertex3f(Cos(th+inc),Sin(th+inc),0); 
	}
	glEnd();
	
	// Base
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0,0,-1);
	glTexCoord2f(0.5,0.5);
	glVertex3f(0,0,0);
	for(th = -inc; th <= 360; th+= 2*inc)
	{
		glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
		glVertex3f(Cos(th),Sin(th),0);
	}	
    glEnd();
    glPopMatrix();
	
	// Disable textures
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw a Disk
 *     at (x,y,z)
 *     with radius r
 *     rotated th about the y axis and ph about the x axis
 */
void disk(double x, double y, double z, double th, double ph, double r)
{
	int inc =  5;
	
   // Define Material for specular and emission
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
 
    // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(r,r,r);
   
   glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0,0,-1);
	glTexCoord2f(0.5,0.5);
	glVertex3f(0,0,0);
	for(th = -inc; th <= 360; th+= 2*inc)
	{
		glTexCoord2f(0.5*Cos(th)+0.5,0.5*Sin(th)+0.5);
		glVertex3f(Cos(th),Sin(th),0);
	}	
    glEnd();
	glPopMatrix();
	
	// Disable textures
    glDisable(GL_TEXTURE_2D);
}


/*
 *  Draw fan wing with fixed dimensions
 *     
 */
 
void fanWing()
{
    glPushMatrix();
	glRotated(90,1,0,0);
	glRotated(90,0,1,0);
	glScaled(0,-5,15);
	cylinder(0,-0.12,0,0,-3,0.1,0.01);
	glPopMatrix();
}

/*
 *  Draw the base of fan with fixed dimensions
 *     
 */
void fanBase()
{
	glPushMatrix();

	glTranslated(1,3.2,1);
	glPushMatrix();
	glRotated(90,0,0,0);
	cylinder(0, 0, 0,0, 0,0.01, 0.01);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,0.002,0);
	glRotated(90,1,0,0);
	disk(0,0,0,0,0,0.2);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,-.05,0);
	glRotated(90,0,0,0);
	cylinder(0,0,0,0,0,0.1,0.1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,0.002,0);
	glRotated(90,1,0,0);
	disk(0,0,0.0,0,0,0.2);
	glPopMatrix();
	
	// Attach fan wings
	glPushMatrix();
	glTranslated(0.1,0.0,0);
	fanWing();
	glPopMatrix();
	
	glPushMatrix();
	glRotated(120,0,1,0);
	glTranslated(.1,0,0);
	fanWing();
	glPopMatrix();
	
	glPushMatrix();
	glRotated(240,0,1,0);
	glTranslated(0.1,0.0,0);
	fanWing();
	glPopMatrix();
	
	glPopMatrix();
}

/*
 *  Draw a fan
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 */
void fan(int x, int y, int z, double dx, double dy, double dz)
{	
	glPushMatrix();
	glTranslated(x,y,z);
	glScaled(dx,dy,dz);
	glColor3f(0.458824,0.658824,0.435294);
	// Turn off/on fan
	if(fanFlag == 0)
	{
		glPushMatrix();
		fanBase();
		glPopMatrix();
	}
	else
	{
		fanAngle += fanSpeed;
		glPushMatrix();
		glTranslated(1,0,1);
		glRotated(fanAngle,0,1,0);
		glTranslated(-1,0,-1);
		fanBase();
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(1,3.3,1);
	glRotated(-90,1,0,0);
	cylinder(0, 0, 0, 0, 0, 0.1,0.1);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(1,3.6,1);
	glRotated(90,1,0,0);
	cylinder(0, 0, 0, 0,0, 0.02, 0.3);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1,3.96,1);
	glRotated(90,1,0,0);
	cylinder(0, 0, 0, 0,0, 0.1, 0.1);
	glPopMatrix();
	
	glPopMatrix();
	
	if(fanFlag == 1)
		glutPostRedisplay();
}

/*
 *  Draw a compound wall
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
void compoundWall(double x,double y,double z,double dx,double dy,double dz,double th)
{
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
    // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   
   glBindTexture(GL_TEXTURE_2D,texture[23]);
   glBegin(GL_QUADS);
   // Left
   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(-1,-1,1.5);
   glTexCoord2f(2,0); glVertex3f(+1,-1,1.5);
   glTexCoord2f(2,1); glVertex3f(+1,+1,1.5);
   glTexCoord2f(0,1); glVertex3f(-1,+1,1.5);
   
   // Rightw
   glNormal3f(0,0,1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(2,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(2,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   
   // Back
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1.5);
   glTexCoord2f(4,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(4,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1.5);
   
   // Front - 1
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(4,0); glVertex3f(-1,-1,+0.5);
   glTexCoord2f(4,1); glVertex3f(-1,+1,+0.5);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   
   // Front - 2
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1.5);
   glTexCoord2f(2,0); glVertex3f(-1,-1,+0.8);
   glTexCoord2f(2,1); glVertex3f(-1,+1,+0.8);
   glTexCoord2f(0,1); glVertex3f(-1,+1,+1.5);
   glEnd();
   
    //Gate
   glBindTexture(GL_TEXTURE_2D,texture[18]);
   glColor3f(0.53,0.16,0.02);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);

   if(gateFlag == 0){
	    glNormal3f(0, 0, 1);
	    glTexCoord2f(0,0); glVertex3f(-0.4,-1,+0.8);
		glTexCoord2f(1,0); glVertex3f(-1,-1,+0.8);
		glTexCoord2f(1,1); glVertex3f(-1,+1,+0.8);
		glTexCoord2f(0,1); glVertex3f(-0.4,+1,+0.8);
	} else {
		glNormal3f(-1, 0, 0);
	    glTexCoord2f(0,0); glVertex3f(-1,-1,+0.5);
		glTexCoord2f(1,0); glVertex3f(-1,-1,+0.8);
		glTexCoord2f(1,1); glVertex3f(-1,+1,+0.8);
		glTexCoord2f(0,1); glVertex3f(-1,+1,+0.5);
   }	   
   glEnd();
   glPopMatrix();
   // Disable textures
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw a fence
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis and ph about the x axis
 */
void fence(
	double x,double y,double z,double dx,double dy,double dz,
    double th, double ph)
{	
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);

   glColor3f(0.5,0.91,0.93);
   compoundWall(0,1.2,0,1,2,1,0);
   
   glPopMatrix();
}

/*
 *  Draw a compound wall
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
void interiorWall(
	double x,double y,double z,double dx,double dy,double dz, double th)
{
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   
    // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glColor3f(1,1,1);

   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   
   glEnd();
   glPopMatrix();
   // Disable textures
   glDisable(GL_TEXTURE_2D);
}

void tableg(float llen,float lthk)
{
	glPushMatrix();
	glRotated(-90,1,0,0);
	cylinder(0,0,llen,0,0,1,lthk);
	glPopMatrix();
}
void table(float tabwid,float tablen,float tabthk,float llen,float lthk)
{
	glPushMatrix();
	glPushMatrix();
	glTranslated(0,llen,0);
	glScaled(tabwid,tabthk,tablen);
	cube(0,0.5,0,0.5,0.5,0.5,0);
	glPopMatrix();
	float dist1=.95*tablen/2-lthk/2;
	float dist2=.95*tabwid/2-lthk/2;
	// front right leg
	glPushMatrix();
	glTranslated(dist2,0,dist1);
	tableg(llen,lthk);
	glPopMatrix();
	//back right leg
	glPushMatrix();
	glTranslated(dist2,0,-dist1);
	tableg(llen,lthk);
	glPopMatrix();
	//back left leg
	glPushMatrix();
	glTranslated(-dist2,0,-dist1);
	tableg(llen,lthk);
	glPopMatrix();
	//front left leg
	glPushMatrix();
	glTranslated(-dist2,0,dist1);
	tableg(llen,lthk);
	glPopMatrix();

	glPopMatrix();
}

void dtable(float tabwid,float tablen,float tabthk,float llen,float lthk)
{
	glPushMatrix();
	glPushMatrix();
	glTranslated(0,llen,0);
	glScaled(tabwid,tabthk,tablen);
	cube(0,0.5,0,0.5,0.5,0.5,0);
	glPopMatrix();
	float dist1=.95*tablen/2-lthk/2;
	float dist2=.95*tabwid/2-lthk/2;
	// front right leg
	glPushMatrix();
	glTranslated(dist2,0,dist1);
	tableg(0,-2);
	glPopMatrix();
	//back right leg
	glPushMatrix();
	glTranslated(dist2,0,-dist1);
    tableg(0,-2);
	glPopMatrix();
	//back left leg
	glPushMatrix();
	glTranslated(-dist2,0,-dist1);
	tableg(0,-2);
	glPopMatrix();
	//front left leg
	glPushMatrix();
	glTranslated(-dist2,0,dist1);
	tableg(0,-2);
	glPopMatrix();

	glPopMatrix();
}

/*
 *  Draw a bed
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 */
void bed(double x, double y, double z, double dx,double dy,double dz)
{
	glPushMatrix();
	glTranslated(x,y,z);
	glScaled(dx,dy,dz);
	// Matress
	object = 12;
	glColor3f(1,1,1);
	cube(0,0.3,0,0.5,0.5,0.5,0);
	glPopMatrix();
		
	// Head
	object = 13;
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslated(8,6,0);
	glRotated(90,0,0,1);
	glScaled(10,-2,15);
	cube(0,0.3,0,0.5,0.5,0.5,0);
	glPopMatrix();	
}

/*
 *  Draw a couch with fixed dimensions
 *     
 */
void couch(){
	
	// 3 seater
	object = 8;
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslated(112, 0,-15);
	glRotated(90,0,0,1);
	glScaled(20,-2,-40);
	cube(0.2,0.2,0.4,0.4,0.4,0.4,0);
	glPopMatrix();
	
	object = 9;
	glPushMatrix();
    glColor3f(0.53,0.16,0.02);
	glTranslated(110,0.7,-65);
	glRotated(90,0,0,1);
	glScaled(2,4,9);
	cube(2,1,3.7,1.5,1.4,1.6,0);
	glPopMatrix();
	
	object = 8;
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslated(111.3,-4,-20);
	glRotated(90,0,0,1);
	glScaled(4,3,1);
	cube(2,1,3.7,2,1.2,1.6,0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(111.3,-4,-50);
	glRotated(90,0,0,1);
	glScaled(4,3,1);
	cube(2,1,3.7,2,1.2,1.6,0);
	glPopMatrix();
	
    // 1 seater
	glPushMatrix();
	glTranslated(90,-4,-12);
	glRotated(90,0,0,1);
	glScaled(4,5,1);
	cube(2,1,3.7,1.8,1.6,1.6,0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(80,-10,8);
	glRotated(90,0,0,1);
	glScaled(7,2,-6);
	cube(2,1,3.7,1,1,1,0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(96,-10,8);
	glRotated(90,0,0,1);
	glScaled(7,2,-6);
	cube(2,1,3.7,1,1,1,0);
	glPopMatrix();
	
	object = 9;
	glPushMatrix();
    glColor3f(0.53,0.16,0.02);
	glTranslated(90, 2,-32);
	glRotated(90,0,0,1);
	glScaled(2,4,4);
	cube(2,1,3.7,1.5,1.5,1.8,0);
	glPopMatrix();
	
	// 1 seater
	object = 8;
	glPushMatrix();
	glColor3f(1,1,1);
	glTranslated(90,-4,-60);
	glRotated(90,0,0,1);
	glScaled(4,5,1);
	cube(2,1,3.7,1.8,1.4,1.6,0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(80,-10,-28);
	glRotated(90,0,0,1);
	glScaled(7,2,-6);
	cube(2,1,3.7,1,1,1,0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(96,-10,-28);
	glRotated(90,0,0,1);
	glScaled(7,2,-6);
	cube(2,1,3.7,1,1,1,0);
	glPopMatrix();
	
	object = 9;
	glPushMatrix();
	glColor3f(0.53,0.16,0.02);
	glTranslated(90, 2,-63);
	glRotated(90,0,0,1);
	glScaled(2,4,4);
	cube(2,1,3.7,1.5,1.5,1.6,0);
	glPopMatrix();
}

/*
 *  Draw a bed lamp with fixed dimensions
 *     
 */
void bedLamp(){

	glPushMatrix();
	glColor3f(0.6, 0.3, 0.37);
	glTranslated(5.5,4.1,-10);
	glRotated(90,0,0,1);
	glScaled(2,1,1);
	cube(1,0.5,0,1,1,1,0);
	glPopMatrix();
	
	glPushMatrix();
	glColor3f(0.8,0.2,0.4);
	glLineWidth(5);	
	glBegin(GL_LINES);
	glNormal3f(0, 0, 1);
    glVertex3f(5,5,-10);
    glVertex3f(5,10,-10);	
    glEnd();  
	glPopMatrix();	
	
}

/*
 *  Draw a bed
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 */
void cot(double x, double y, double z, double dx, double dy, double dz)
{
	glPushMatrix();
	glTranslated(x, y, z); 
	glScaled(dx, dy, dz);
	glColor3f(0,0,0);
	table(15,15,1,3.5,3);	
	glColor3f(1, 1, 1);
	bed(0,5,0,15,2,15);
	bedLamp();	
	glPopMatrix();	
	
}

/*
 *  Draw a wall clock
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 */
void wallClock(double x, double y, double z, double dx, double dy, double dz)
{
	int hour_ticks , sec_ticks;
	glPushMatrix();
	glTranslated(x, y, z); 
	glScaled(dx, dy, dz);
    glColor3f(0.2,0.4,0.8);

	// Draw clock
	glPushMatrix();
	glTranslatef(0,0,1.0);
	disk(0,0,-1,0,0,8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,0,1.95);
	disk(0,0,-1,0,0,8);
	glPopMatrix();

	// Draw hour hand
	glPushMatrix();
	glColor3f(1.0, 0.5, 0.5);
	glTranslatef( 0, 0, 1.5);
	glRotatef( -(360/12) * (newtime->tm_hour+newtime->tm_min/60.0), 0.0, 0.0, 1.0);
	glRotatef( -90, 1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,0,3);
	glEnd();		  
	glPopMatrix();

	// Draw minute hand
	glPushMatrix();
	glLineWidth(2);
	glColor3f(1.0, 0.5, 1.0);
	glTranslatef( 0, 0, 1.25);
	glRotatef( -(360/60) * newtime->tm_min, 0.0, 0.0, 1.0);
	glRotatef(-90, 1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,0,4);
	glEnd();		  
	glPopMatrix();

	// Draw second hand
	
	glPushMatrix();
	glLineWidth(2);
	glTranslatef( 0, 0, 1);
	glRotatef(-(360/60) * newtime->tm_sec, 0.0, 0.0, 1.0);
	glRotatef( -90, 1.0, 0.0, 0.0);	
	glColor3f(1,0,0);
	glBegin(GL_LINES);
	glVertex3f(0,0,0);
	glVertex3f(0,0,5);
	glEnd();		  
	glPopMatrix();
	
	for(hour_ticks = 0; hour_ticks < 12; hour_ticks++)
	  {
		glPushMatrix();
        glTranslatef(0.0, 0.0, 1);
		glRotatef( (360/12) * hour_ticks, 0.0, 0.0, 1.0);
		glTranslatef( 6.0, 0.0, 0.0);
		cube(0,0,2,0,0,0,0);
		glPopMatrix();
	}

	for(sec_ticks = 0; sec_ticks < 60; sec_ticks++)
	 {
   	glPushMatrix();
	glTranslatef(0.0, 0.0, 1.1);
	glRotatef( (360/60) * sec_ticks, 0.0, 0.0, 1.0);
	glTranslatef(6.0, 0.0, 0.0);
	cube(0.2,0.2,0.2,0.2,0.2,0.2,0);
	glPopMatrix();
	}

  glPopMatrix();
}

void chair(float cblen,float cbwid,float cbthk,float cllen,float clwid)
{
	glPushMatrix();
	glTranslated(0,cllen,0);
	
	//chair base
	glPushMatrix();
	glScaled(cblen,cbthk,cbwid);
	cube(1,0.4,1,0.3,0.1,1,0);
	cube(1,1,1.9,0.4,0.5,0.1,0);
	glPopMatrix();
	
	
	glPopMatrix();
}

void diningtable()
{

 	glPushMatrix();
	glTranslated(90,2,-80);
	glScaled(2,2,1);
	glColor3f(0.57,0.66,0.48);
	dtable(15,15,0.8,1.5,2);	
	glColor3f(1,1,1);
	
	cylinder(8,-1,0.2,0,90,0.3,1.8);
	cylinder(8,-1,-2.5,0,90,0.3,1.8);
	cylinder(6,-1,0.2,0,90,0.3,1.8);
	cylinder(6,-1,-2.5,0,90,0.3,1.8);
	
	//back left chair
	glPushMatrix();
	cube(1,1,7,2,0.3,2,0);
	cube(1,2.7,9,2.2,2,0.1,0);
	glRotated(90,0,0,0);
		
	
	glPopMatrix();
	
	cylinder(0,-1,8,0,90,0.3,1.8);
	cylinder(0,-1,4,0,90,0.3,1.8);
	cylinder(2,-1,8,0,90,0.3,1.8);
	cylinder(2,-1,4,0,90,0.3,1.8);
	//front right chair
	glPushMatrix();
	glRotated(90,0,1,0);
	cube(1,1,7,2,0.3,2,0);
	cube(1,2.7,9,2.2,2,0.1,0);
	glPopMatrix();
	
	glPushMatrix();
	cylinder(-6,-1,0.2,0,90,0.3,1.8);
	cylinder(-6,-1,-2.5,0,90,0.3,1.8);
	cylinder(-8,-1,0.2,0,90,0.3,1.8);
	cylinder(-8,-1,-2.5,0,90,0.3,1.8);
	glRotated(180,0,1,0);
	cube(1,1,7,2,0.3,2,0);
	cube(1,2.7,9,2.2,2,0.1,0);
	glPopMatrix();
	
	glPushMatrix();
	cylinder(-2,-1,-8,0,90,0.3,1.8);
	cylinder(-2,-1,-4,0,90,0.3,1.8);
	cylinder(0,-1,-8,0,90,0.3,1.8);
	cylinder(0,-1,-4,0,90,0.3,1.8);
	glRotated(270,0,1,0);
	cube(1,1,7,2,0.3,2,0);
	cube(1,2.7,9,2.2,2,0.1,0);
	glPopMatrix();
	/*
	glPushMatrix();
	glTranslated(2,1,-1);	
	glRotated(180,0,1,0);
	cube(-1,-0.4,1.8,1,0.2,0.6,0);
	cube(-1,1,2.4,1.1,1.5,0.1,0);
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(2,1,-1);	
	glRotated(270,0,1,0);
	cube(-1,-0.4,1.8,1,0.2,0.6,0);
	cube(-1,1,2.4,1.1,1.5,0.1,0);
	glPopMatrix();*/
	/*
	//back right chair
	glPushMatrix();
	glTranslated(-5,-1,0);	
	glRotated(270,0,1,0);
	chair(5.15,5.15,3.02,0.3,0.005);
	glPopMatrix();
	
	//back chair
	glPushMatrix();
	glTranslated(-5,-1,0);	
	glRotated(-180,0,1,0);
	chair(5.15,5.15,3.02,0.3,0.005);
	glPopMatrix();*/
	
	
	
	//front chair
	/*glPushMatrix();
	glTranslated(0,0,0.27);
	glRotated(90,0,1,0);
	chair(0.15,0.15,0.02,0.3,0.005);
	glPopMatrix();*/

	glPopMatrix();

}

void steps(void)
{
    int i;
	object = 14;
	glPushMatrix();
	glRotated(-90,0,1,0);
	glTranslated(-70,-1,-60);	
	int h =0;
	for(i=5;i<31;i++)
	{	glPushMatrix();
	if(i == 30){
		glTranslated(63,h*1.6,-i*1.2);
		glScaled(11,2,15);
	} else {
		glColor3f(1,1,1);
		glTranslated(63,h*1.6,-i*1);
		glScaled(11,2,3);
	}	
		cube(0.3, 0, 0, 0.3, 0.1, 0.3, 0);
		glPopMatrix();
		++h;
	}
	glPopMatrix();

	
}

/*
 *  Draw a Tree
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis and ph about the x axis
 */
 
void tree1( double x, double y, double z, double dx, double dy, double dz, double th, double ph, int flag)
{
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(th,0,1,0);
	glRotated(ph,1,0,0);
	glScaled(dx,dy,dz);
		
	// Brown Trunk
	glColor3f(0.53,0.16,0.02);
	cylinder(0,0,0,0,90,1.75,20);
		
	// Leaves	
	if(flag == 1){
		glColor3f(1,1,1);
		cone(0,-5,0,0,-90,8,40);
	} else {
		glColor3f(0,0.5,0);
		cone(0,-5,0,0,-90,8,40);
	}
	
	glPopMatrix();
}

void tree2( double x, double y, double z, double dx, double dy, double dz, double th, double ph, double r)
{
	object = 5;
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(th,0,1,0);
	glRotated(ph,1,0,0);
	glScaled(dx,dy,dz);
	
	// Trunk
	glColor3f(0.53,0.16,0.02);
	cylinder(0,0,0,0,90,1.75,20);
	
	//Branches
    glColor3f(0.184314 , 0.309804 , 0.184314);
    sphere1(-5,15,1,r);
    sphere1(5,15,1,r);
    sphere1(0,25,1,r); 
			
	glPopMatrix();
}

void tree3( double x, double y, double z, double dx, double dy, double dz, double th, double ph, double r)
{
	object = 15;
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(th,0,1,0);
	glRotated(ph,1,0,0);
	glScaled(dx,dy,dz);
	
	// Trunk
	glColor3f(0.53,0.16,0.02);
	cylinder(0,0,0,0,90,1.75,20);
	
	//Branches
	glColor3f(1,1,1);
    sphere1(-8,15,1,r);
    sphere1(8,15,1,r);
    sphere1(-5,25,1,r-1);
    sphere1(5,25,1,r-1);
    sphere1(0,35,1,r-2);
	sphere1(0,25,-5,r-2);
	sphere1(0,25,5,r-2);
			
	glPopMatrix();
}
	
/*
 *  Draw a Chair
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis and ph about the x axis
 */
 
void chair1(
	double x, double y, double z, double dx, double dy, double dz,
	double th, double ph)
{
	glPushMatrix();
	glTranslated(x,y,z);
	glRotated(th,0,1,0);
	glRotated(ph,1,0,0);
	glScaled(dx,dy,dz);
	
	glColor3f(1,1,1);
	cylinder(0,-1,0,0,90,1,15);
	glPopMatrix();
}
	
/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius r
 */
void ball(double x,double y,double z,double r)
{
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   glutSolidSphere(1.0,16,16);
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a house
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis and ph about the x axis
 */
 
static void house(double x,double y,double z,double dx,double dy,double dz,
                 double th,  double ph)
{
	// Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);
   
   //  Body
   cube(0,0,0,1,1,1,0);
  
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   
   // Ceiling
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glNormal3f(0,1,0);
   glTexCoord2f(0,0); glVertex3f(-1.2,+1,+1.2);
   glTexCoord2f(rep,0); glVertex3f(+1.2,+1,+1.2);
   glTexCoord2f(rep,rep); glVertex3f(+1.2,+1,-1.2);
   glTexCoord2f(0,rep); glVertex3f(-1.2,+1,-1.2);
   glEnd(); 
   
   // Roof
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glColor3f(0.53,0.16,0.02);
   
   glNormal3f(0,-1,1);
   glTexCoord2f(0,0); glVertex3f(-1.2,+1,+1.2);
   glTexCoord2f(1,0); glVertex3f(+1.2,+1,+1.2);
   glTexCoord2f(1,1); glVertex3f(+1.2,+2,0);
   glTexCoord2f(0,1); glVertex3f(-1.2,+2,0);
	
   glNormal3f(0,-1,-1);
   glTexCoord2f(0,0); glVertex3f(-1.2,+2,0);
   glTexCoord2f(1,0); glVertex3f(+1.2,+2,0);
   glTexCoord2f(1,1); glVertex3f(+1.2,+1,-1.2);
   glTexCoord2f(0,1); glVertex3f(-1.2,+1,-1.2);
  
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1.2,+2,0);
   glTexCoord2f(1,0); glVertex3f(-1.2,+1,-1.2);
   glTexCoord2f(0,1); glVertex3f(-1.2,+1,+1.2);
   glTexCoord2f(0,0); glVertex3f(-1.2,+2,0);
	
   glNormal3f(1,0,0);
   glTexCoord2f(0,0); glVertex3f(1.2,+2,0);
   glTexCoord2f(1,0); glVertex3f(1.2,+1,-1.2);
   glTexCoord2f(0,1); glVertex3f(1.2,+1,+1.2);
   glTexCoord2f(0,0); glVertex3f(1.2,+2,0);    
   glEnd(); 
   
   // Front Door
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glBegin(GL_QUADS);
   glColor3f(1,0,1);
     
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1.05,-0.95,-0.25);
   glTexCoord2f(1,0); glVertex3f(-1.05,0,-0.25);
   glTexCoord2f(1,1); glVertex3f(-1.05,0,0.25);
   glTexCoord2f(0,1); glVertex3f(-1.05,-0.95,0.25);
   glEnd();
   
   // Front Window
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   glColor3f(1,1,0);
   
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1.05,0.2,0.8);
   glTexCoord2f(1,0); glVertex3f(-1.05,0.6,0.8);
   glTexCoord2f(1,1); glVertex3f(-1.05,0.6,0.4);
   glTexCoord2f(0,1); glVertex3f(-1.05,0.2,0.4);
   
   glTexCoord2f(0,0); glVertex3f(-1.05,0.2,-0.8);
   glTexCoord2f(1,0); glVertex3f(-1.05,0.6,-0.8);
   glTexCoord2f(1,1); glVertex3f(-1.05,0.6,-0.4);
   glTexCoord2f(0,1); glVertex3f(-1.05,0.2,-0.4);

   glEnd();
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);   
}

/*
 *  Draw a duplex house
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis and ph about the x axis
 */
static void duplex(
	double x,double y,double z,double dx,double dy,double dz,
	double th,double ph)
{
	// Define Material for specular
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);
   
   // Body
   cube(0,0,0,1,1,1,0);
   //glColor3f(0.5,0.91,0.93);
   glColor3f(1,1,1);
   cube(0,2,0,1,1,1,0);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

   // Ceiling
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glNormal3f(0,1,0);
   
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   
   glTexCoord2f(0,0); glVertex3f(-1.2,+3,+1.2);
   glTexCoord2f(1,0); glVertex3f(+1.2,+3,+1.2);
   glTexCoord2f(1,1); glVertex3f(+1.2,+3,-1.2);
   glTexCoord2f(0,1); glVertex3f(-1.2,+3,-1.2);
   glEnd();
   
   // Roof
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glColor3f(0.53,0.16,0.02);
   
   glNormal3f(0,-1,1);
   glTexCoord2f(0,0); glVertex3f(-1.2,+3,+1.2);
   glTexCoord2f(1,0); glVertex3f(+1.2,+3,+1.2);
   glTexCoord2f(1,1); glVertex3f(+1.2,+4,0);
   glTexCoord2f(0,1); glVertex3f(-1.2,+4,0);
  
   glNormal3f(0,-1,-1);
   glTexCoord2f(0,0); glVertex3f(-1.2,+4,0);
   glTexCoord2f(1,0); glVertex3f(+1.2,+4,0);
   glTexCoord2f(1,1); glVertex3f(+1.2,+3,-1.2);
   glTexCoord2f(0,1); glVertex3f(-1.2,+3,-1.2);
	
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1.2,+4,0);
   glTexCoord2f(1,0); glVertex3f(-1.2,+3,-1.2);
   glTexCoord2f(1,1); glVertex3f(-1.2,+3,+1.2);
   glTexCoord2f(0,1); glVertex3f(-1.2,+4,0);
	
   glNormal3f(1,0,0);
   glTexCoord2f(0,0); glVertex3f(1.2,+4,0);
   glTexCoord2f(1,0); glVertex3f(1.2,+3,-1.2);
   glTexCoord2f(1,1); glVertex3f(1.2,+3,+1.2);
   glTexCoord2f(0,1); glVertex3f(1.2,+4,0);
   glEnd();
   
   // Doors - up and down
   glPushMatrix();
   glBindTexture(GL_TEXTURE_2D,texture[1]);

   glBegin(GL_QUADS);
   glColor3f(1,0,1);  

   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1.05,-0.95,-0.25);
   glTexCoord2f(1,0); glVertex3f(-1.05,0,-0.25);
   glTexCoord2f(1,1); glVertex3f(-1.05,0,0.25);
   glTexCoord2f(0,1); glVertex3f(-1.05,-0.95,0.25);
   
   glTexCoord2f(0,0); glVertex3f(-1.05,1.05,-0.25);
   glTexCoord2f(1,0); glVertex3f(-1.05,2,-0.25);
   glTexCoord2f(1,1); glVertex3f(-1.05,2,0.25);
   glTexCoord2f(0,1); glVertex3f(-1.05,1.05,0.25);
   glEnd();
   glPopMatrix();
   
   // Windows - up and down
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   glColor3f(1,1,0);
   
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1.05,0.2,0.8);
   glTexCoord2f(1,0); glVertex3f(-1.05,0.6,0.8);
   glTexCoord2f(1,1); glVertex3f(-1.05,0.6,0.4);
   glTexCoord2f(0,1); glVertex3f(-1.05,0.2,0.4);
   
   glTexCoord2f(0,0); glVertex3f(-1.05,0.2,-0.8);
   glTexCoord2f(1,0); glVertex3f(-1.05,0.6,-0.8);
   glTexCoord2f(1,1); glVertex3f(-1.05,0.6,-0.4);
   glTexCoord2f(0,1); glVertex3f(-1.05,0.2,-0.4);
   
   glTexCoord2f(0,0); glVertex3f(-1.05,2.2,0.4);
   glTexCoord2f(1,0); glVertex3f(-1.05,2.6,0.4);
   glTexCoord2f(1,1); glVertex3f(-1.05,2.6,0.8);
   glTexCoord2f(0,1); glVertex3f(-1.05,2.2,0.8);
   
   glTexCoord2f(0,0); glVertex3f(-1.05,2.2,-0.4);
   glTexCoord2f(1,0); glVertex3f(-1.05,2.6,-0.4);
   glTexCoord2f(1,1); glVertex3f(-1.05,2.6,-0.8);
   glTexCoord2f(0,1); glVertex3f(-1.05,2.2,-0.8);
   
   glEnd();
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}


static void mainHouse(
	double x,double y,double z,double dx,double dy,double dz,
	double th,double ph)
{
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);
   
   // Body
   mainHouseBase(0,1.2,0,1,2,1,0);
   
    // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

    // Ceiling
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glNormal3f(0,1,0);
   
   glTexCoord2f(0,0); glVertex3f(-1,+3.15,+0.18);
   glTexCoord2f(2,0); glVertex3f(+1,+3.15,+0.18);
   glTexCoord2f(2,2); glVertex3f(+1,+3.15,-1);
   glTexCoord2f(0,2); glVertex3f(-1,+3.15,-1);

   glTexCoord2f(0,0); glVertex3f(-1,+3.15,+1);
   glTexCoord2f(2,0); glVertex3f(+1,+3.15,+1);
   glTexCoord2f(2,2); glVertex3f(+1,+3.15,0.3);
   glTexCoord2f(0,2); glVertex3f(-1,+3.15,0.3);

   glTexCoord2f(0,0); glVertex3f(-1,+3.15,0.3);
   glTexCoord2f(2,0); glVertex3f(-0.2,+3.15,0.3);
   glTexCoord2f(2,2); glVertex3f(-0.2,+3.15,0.18);
   glTexCoord2f(0,2); glVertex3f(-1,+3.15,0.18);

   glTexCoord2f(0,0); glVertex3f(0.2,+3.15,+0.3);
   glTexCoord2f(2,0); glVertex3f(1,+3.15,+0.3);
   glTexCoord2f(2,2); glVertex3f(1,+3.15,0.18);
   glTexCoord2f(0,2); glVertex3f(0.2,+3.15,0.18);
   glEnd();
   
   // Roof
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1.2,+3.21,+0.16);
   glTexCoord2f(2,0); glVertex3f(+1.2,+3.21,+0.16);
   glTexCoord2f(2,2); glVertex3f(+1.2,+3.21,-1.2);
   glTexCoord2f(0,2); glVertex3f(-1.2,+3.21,-1.2);
   glEnd();
   
    // Roof top right 
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1.2,+3.21,+1.2);
   glTexCoord2f(2,0); glVertex3f(+1.2,+3.21,+1.2);
   glTexCoord2f(2,2); glVertex3f(+1.2,+3.21,0.3);
   glTexCoord2f(0,2); glVertex3f(-1.2,+3.21,0.3);
   glEnd();
   
    // Roof top front 
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(-1.2,+3.21,+0.3);
   glTexCoord2f(2,0); glVertex3f(-0.2,+3.21,+0.3);
   glTexCoord2f(2,2); glVertex3f(-0.2,+3.21,0.16);
   glTexCoord2f(0,2); glVertex3f(-1.2,+3.21,0.16);
   glEnd();
   
    // Roof top back 
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glTexCoord2f(0,0); glVertex3f(0.4,+3.21,+0.3);
   glTexCoord2f(2,0); glVertex3f(1.2,+3.21,+0.3);
   glTexCoord2f(2,2); glVertex3f(1.2,+3.21,0.16);
   glTexCoord2f(0,2); glVertex3f(0.4,+3.21,0.16);
   glEnd();     
     
   // Windows - up and down
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   glColor3f(1,1,0);     
   glNormal3f(-1,0,0);
   glTexCoord2f(0,1); glVertex3f(-1.05,1.2,0.8);
   glTexCoord2f(1,1); glVertex3f(-1.05,2.4,0.8);
   glTexCoord2f(1,0); glVertex3f(-1.05,2.4,0.4);
   glTexCoord2f(0,0); glVertex3f(-1.05,1.2,0.4);
   
   glTexCoord2f(0,0); glVertex3f(-1.05,1.2,-0.8);
   glTexCoord2f(1,0); glVertex3f(-1.05,2.4,-0.8);
   glTexCoord2f(1,1); glVertex3f(-1.05,2.4,-0.4);
   glTexCoord2f(0,1); glVertex3f(-1.05,1.2,-0.4);
   glEnd();
   
   glTranslated(0,1.2,0);
   glRotated(0,0,1,0);
   glScaled(1,2,1);  
   
   // Open door
    if(gateFlag == 0)
	{
       // main house Door left
	   glBindTexture(GL_TEXTURE_2D,texture[1]);
	   glBegin(GL_QUADS);
	   glColor3f(1,0,1);
	   glNormal3f(0,0,-1);
	   glTexCoord2f(0,0); glVertex3f(-1,-1,-0.2);
	   glTexCoord2f(1,0); glVertex3f(-1,0.4,-0.2);
	   glTexCoord2f(1,1); glVertex3f(-0.7,0.4,-0.2);
	   glTexCoord2f(0,1); glVertex3f(-0.7,-1,-0.2);
	   glEnd();
	   
	   // main house Door right
	   glBindTexture(GL_TEXTURE_2D,texture[1]);
	   glBegin(GL_QUADS);
	   glColor3f(1,0,1);
	   glNormal3f(0,0,1);
	   glTexCoord2f(0,0); glVertex3f(-0.7,-1,0.2);
	   glTexCoord2f(1,0); glVertex3f(-0.7,0.4,0.2);
	   glTexCoord2f(1,1); glVertex3f(-1,0.4,0.2);
	   glTexCoord2f(0,1); glVertex3f(-1,-1,0.2);
	   glEnd();
	} 
	else 
	{
		// Close door
	   // main house Door left
	   glBindTexture(GL_TEXTURE_2D,texture[1]);
	   glBegin(GL_QUADS);
	   glColor3f(1,0,1);
	   glNormal3f(-1,0,0);
	   glTexCoord2f(0,0); glVertex3f(-1,-1,-0.2);
	   glTexCoord2f(1,0); glVertex3f(-1,0.4,-0.2);
	   glTexCoord2f(1,1); glVertex3f(-1,0.4,0);
	   glTexCoord2f(0,1); glVertex3f(-1,-1,0);
	   glEnd();
	   
	   // main house Door right
	   glBindTexture(GL_TEXTURE_2D,texture[1]);
	   glBegin(GL_QUADS);
	   glColor3f(1,0,1);
	   glNormal3f(-1,0,0);
	   glTexCoord2f(0,0); glVertex3f(-1,-1,0);
	   glTexCoord2f(1,0); glVertex3f(-1,0.4,0);
	   glTexCoord2f(1,1); glVertex3f(-1,0.4,0.2);
	   glTexCoord2f(0,1); glVertex3f(-1,-1,0.2);
	   glEnd();
	}   
   
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

static void pentHouse(
	double x,double y,double z,double dx,double dy,double dz,
	double th,double ph)
{
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);
   
   // Body	
   pcube(0,0,0,1,2,1,0);
   
   
    // Define Material for specular and emission
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

   // Ceiling
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   glNormal3f(0,1,0);
   glTexCoord2f(0,0); glVertex3f(-1,+1.95,+1);
   glTexCoord2f(2,0); glVertex3f(+1,+1.95,+1);
   glTexCoord2f(2,2); glVertex3f(+1,+1.95,-1);
   glTexCoord2f(0,2); glVertex3f(-1,+1.95,-1);
   glEnd();
   
   // Roof
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   glBegin(GL_QUADS);
   glNormal3f(0,1,0);
   glColor3f(1,1,1);
   
   glTexCoord2f(0,0); glVertex3f(-1.2,+2,+1.2);
   glTexCoord2f(2,0); glVertex3f(+1.2,+2,+1.2);
   glTexCoord2f(2,2); glVertex3f(+1.2,+2,-1.2);
   glTexCoord2f(0,2); glVertex3f(-1.2,+2,-1.2);
   
   glTexCoord2f(0,0); glVertex3f(-1.2,+2.01,+1.2);
   glTexCoord2f(2,0); glVertex3f(+1.2,+2.01,+1.2);
   glTexCoord2f(2,2); glVertex3f(+1.2,+2.01,-1.2);
   glTexCoord2f(0,2); glVertex3f(-1.2,+2.01,-1.2);
   
   glNormal3f(0,-1,1);
   glTexCoord2f(0,0); glVertex3f(-1.2,+2,+1.2);
   glTexCoord2f(2,0); glVertex3f(+1.2,+2,+1.2);
   glTexCoord2f(2,2); glVertex3f(+1.2,+3,0);
   glTexCoord2f(0,2); glVertex3f(-1.2,+3,0);
  
   glNormal3f(0,-1,-1);
   glTexCoord2f(0,0); glVertex3f(-1.2,+3,0);
   glTexCoord2f(2,0); glVertex3f(+1.2,+3,0);
   glTexCoord2f(2,2); glVertex3f(+1.2,+2,-1.2);
   glTexCoord2f(0,2); glVertex3f(-1.2,+2,-1.2);

   glColor3f(0.53,0.16,0.02);
   glNormal3f(-1,0,0);
   glTexCoord2f(0,0); glVertex3f(-1.2,+3,0);
   glTexCoord2f(1,0); glVertex3f(-1.2,+2,-1.2);
   glTexCoord2f(1,1); glVertex3f(-1.2,+2,+1.2);
   glTexCoord2f(0,0); glVertex3f(-1.2,+3,0);
	
   glNormal3f(1,0,0);
   glTexCoord2f(0,0); glVertex3f(1.2,+3,0);
   glTexCoord2f(1,0); glVertex3f(1.2,+2,-1.2);
   glTexCoord2f(1,1); glVertex3f(1.2,+2,+1.2);
   glTexCoord2f(0,0); glVertex3f(1.2,+3,0);
   glEnd(); 
    
   glPushMatrix();
   glTranslated(0,0,0);
   glRotated(0,0,1,0);
   glScaled(1,2,1);
   
   // Inner walls
   
   //  Back
   glBindTexture(GL_TEXTURE_2D,texture[25]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   glColor3f(1,1,1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-0.99);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-0.99);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-0.99);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-0.99);
   glEnd();
   
      // Front
   glBegin(GL_QUADS);
   glColor3f(0.258824,0.258824,0.435294);
   glNormal3f( 0, 0, 1);   
   glVertex3f(-1,-1, 0.99);
   glVertex3f(+1,-1, 0.99);
   glVertex3f(+1,+1, 0.99);
   glVertex3f(-1,+1, 0.99);
   
   //  Right
   glNormal3f( 1, 0, 0);
   glVertex3f(+0.99,-1,+1);
   glVertex3f(+0.99,-1,-1);
   glVertex3f(+0.99,+1,-1);
   glVertex3f(+0.99,+1,+1);
   
   //  Left
   glNormal3f(-1, 0, 0);
   glVertex3f(-0.99,-1,-1);
   glVertex3f(-0.99,-1,+1);
   glVertex3f(-0.99,+1,+1);
   glVertex3f(-0.99,+1,-1); 
   glEnd(); 	
    
   //Bottom
   glBindTexture(GL_TEXTURE_2D,texture[26]);
   glBegin(GL_QUADS);
   glColor3f(1,0,0);
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-0.96,-1);
   glTexCoord2f(4,0); glVertex3f(+1,-0.96,-1);
   glTexCoord2f(4,4); glVertex3f(+1,-0.96,+0.7);
   glTexCoord2f(0,4); glVertex3f(-1,-0.96,+0.7);
   
   //Bottom steps way
   glNormal3f( 0, -1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-0.96,0.7);
   glTexCoord2f(1,0); glVertex3f(-0.1,-0.96,0.7);
   glTexCoord2f(1,1); glVertex3f(-0.1,-0.96,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-0.96,+1);
   
   //Bottom steps way
   glNormal3f( 0, -1, 0);	
   glTexCoord2f(0,0); glVertex3f(0.5,-0.96,0.6);
   glTexCoord2f(1,0); glVertex3f(+1,-0.96,0.6);
   glTexCoord2f(1,1); glVertex3f(1,-0.96,+1);
   glTexCoord2f(0,1); glVertex3f(0.5,-0.96,+1);
   
     //Bottom steps way
   glNormal3f( 0, -1, 0);	
   glTexCoord2f(0,0); glVertex3f(-1,-0.96,0.88);
   glTexCoord2f(1,0); glVertex3f(1,-0.96,0.88);
   glTexCoord2f(1,1); glVertex3f(1,-0.96,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-0.96,+1);
   
   glEnd();
   glPopMatrix();
     
   // Windows
   glBindTexture(GL_TEXTURE_2D,texture[15]);
   glBegin(GL_QUADS);
   glColor3f(1,1,1); 
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1.05,1.2,-0.4);
   glTexCoord2f(1,0); glVertex3f(-1.05,-1.2,-0.4);
   glTexCoord2f(1,1); glVertex3f(-1.05,-1.2,0.4);
   glTexCoord2f(0,1); glVertex3f(-1.05,1.2,0.4); 
    
   glEnd();
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw a table
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void table1(
	double x,double y,double z,double dx,double dy,double dz,double th)
{
	  glPushMatrix();
	  glTranslated(x,y,z);
	  glRotated(th,0,1,0);
	  glScaled(dx,dy,dz);
		
	  // Top
      cube(0,0.4,0,2,0.01,1,0);

	  // Legs
	  glColor3f(0.53,0.16,0.02);
	  cylinder(-1.4,-0.5,-0.8,0,90,0.1,0.85);
	  cylinder(1.4,-0.5,0.8,0,90,0.1,0.85);
	  cylinder(-1.4,-0.5,0.8,0,90,0.1,0.85);
	  cylinder(1.4,-0.5,-0.8,0,90,0.1,0.85);

      glPopMatrix();
}


/*
 *  Draw a swimming pool
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void pool(
	double x,double y,double z,double dx,double dy,double dz,double th)
{
	  glPushMatrix();
	  glTranslated(x,y,z);
	  glRotated(th,0,1,0);
	  glScaled(dx,dy,dz);
		
	  // Pool
      cube(0,0,0,1,1,1,0);
	  // Borders
	  object = 11;
	  glColor3f(1,1,1);
	  cube(0,-5,0,1.2,1.1,1.2,0);
	  
      glPopMatrix();
}

void genVerticesAndNormals()
{ 
	int i,j;
	double x,y,z,t,n;
	t =- MAXQUADS*stepSize/2.0;
	for (i=0;i<MAXQUADS;i++)
	{
		y = t+(double)i*stepSize;
		for (j=0;j<MAXQUADS;j++)
		{
			x =t+(double)j*stepSize;
			z =sin((x*x+y*y-phaseAngle)/100.0);
			n =cos((x*x+y*y-phaseAngle)/100.0);
			vertex[i][j][0] = x;
			vertex[i][j][1] = y;
			vertex[i][j][2] = z;
			normal[i][j][0] = x*n;
			normal[i][j][1] = y*n;
			normal[i][j][2] = 1;
		}
	}
}

void drawQuadsForWaves()
{ 
	int i,j;
	for (i=0; i<MAXQUADS-1; i++){
		for (j=0; j<MAXQUADS-1; j++)
		{ 
			glBegin(GL_QUADS);
			glNormal3dv(normal[i][j]);
			glVertex3dv(vertex[i][j]);
			
			glNormal3dv(normal[i][j+1]);
			glVertex3dv(vertex[i][j+1]);
			
			glNormal3dv(normal[i+1][j+1]);
			glVertex3dv(vertex[i+1][j+1]);
			
			glNormal3dv(normal[i+1][j]);
			glVertex3dv(vertex[i+1][j]);
			glEnd();
		}	
	}
}
 
/*
 *  Draw a waves in pool
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     
 */
void drawWaves(double x, double y, double z, double dx, double dy, double dz)
{
	float diffuse[] = {0.28,0.46,1.0,1.0};
	float specular[] = {1.0,1.0,1.0,1.0};
	float shininess[] = {20.0};   
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffuse);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glTranslated(x,y,z);
	glScaled(dx,dy,dz);
	glRotatef(90, 0.01, 0.0, 0.0);	 
	genVerticesAndNormals();
	drawQuadsForWaves();
	glDisable(GL_LIGHTING);
	glPopMatrix();
}

/*
 *  Draw a tennis court
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void tennisCourt(
	double x,double y,double z,double dx,double dy,double dz,double th)
{
	 // Define Material for specular and emission
	  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
	  glPushMatrix();
	  glTranslated(x,y,z);
	  glRotated(th,0,1,0);
	  glScaled(dx,dy,dz);
		
      cube(0,0,0,1,1,1,0);
	  
	  glLineWidth(3);
	  glColor3f(1,1,1);
	  glBegin(GL_LINES);
	  glNormal3f(0,1,0);
	  glVertex3f(1.01,1.001,1.01);
	  glVertex3f(-1.01,1.001,1.01);
	  
	  glVertex3f(-1.01,1.001,-1.01);
	  glVertex3f(1.01,1.001,-1.01);
	  
	  glVertex3f(1.01,1.001,1.01);
	  glVertex3f(1.01,1.001,-1.01);
	  
	  glVertex3f(-1.01,1.001,-1.01);
	  glVertex3f(-1.01,1.001,1.01);
	  	  
	  glVertex3f(1.01,1.001,0.75);
	  glVertex3f(-1.01,1.001,0.75);
	  
	  glVertex3f(1.01,1.001,-0.75);
	  glVertex3f(-1.01,1.001,-0.75);

	  glVertex3f(0.5,1.001,0.75);
	  glVertex3f(0.5,1.001,-0.75);

	  glVertex3f(-0.5,1.001,0.75);
	  glVertex3f(-0.5,1.001,-0.75);

	  glVertex3f(0.5,1.001,0);
	  glVertex3f(-0.5,1.001,0);
	  
	  glColor3f(0,0,0);
	  glNormal3f(-1,0,0);
	  glVertex3f(0,1.001,1.1);
	  glVertex3f(0,100,1.1);
	  glVertex3f(0,1.001,-1.1);
	  glVertex3f(0,100,-1.1);
	  glEnd();
	  
	  // Enable textures
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D,texture[20]);
	  glColor4f(1,1,1,0.5);
	  //  Enable blending
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE);
      // Tennis Net
      glBegin(GL_QUADS);
	  glTexCoord2f(0,0); glVertex3f(0,20,-1.1);
	  glTexCoord2f(15,0); glVertex3f(0,20,1.1);
      glTexCoord2f(15,2); glVertex3f(0,200,1.1);
	  glTexCoord2f(0,2); glVertex3f(0,200,-1.1); 
      
      glEnd();  
      glPopMatrix();
	  glDisable(GL_TEXTURE_2D);
	  glDisable(GL_BLEND);
}

/*
 *  Draw a torus
 *     at (x,y,z)
 */
void torus(double x,double y,double z)
{	
	  glPushMatrix();
      glTranslated(x,y,z);
      glRotated(90,1,0,0);
      glutSolidTorus(1,4,50,50);	
	  glPopMatrix();
}

/*
 *  Draw a windmill
 *     at (x,y,z)
 */
 
void windmill( double x, double y, double z, double dx, double dy, double dz, double th, double ph)
{
	 // Define Material for specular and emission
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
	  glPushMatrix();
	  glTranslated(x,y,z);
	  glRotated(th,0,1,0);
	  glRotated(ph,1,0,0);
	  glScaled(dx,dy,dz);  

	  glColor3f(1,0.5,0);
	  cylinder(0,20,0,0,90,1,20);
	  glColor3f(0.53,0.16,0.02);
	  cube(0,40,1,0.5,0.5,0.5,0);
	  
	  glColor3f(1,1,1);
	  if(wflag == 1)
	  {
		  glBegin(GL_TRIANGLES);
		  glNormal3f(0,0,1);
		  glVertex3f(0,40,1);
		  glVertex3f(18,43,1);
		  glVertex3f(18,37,1);
		  
		  glVertex3f(0,40,1);
		  glVertex3f(-2,20,1);
		  glVertex3f(3,23,1);
		 
		  glVertex3f(0,40,1);
		  glVertex3f(-15,47,1);
		  glVertex3f(-15,53,1);
		  glEnd();
	  } else if(wflag == 0){
		  
		  glBegin(GL_TRIANGLES);
		  glNormal3f(0,0,1);
		  glVertex3f(0,40,1);
		  glVertex3f(12,26,1);
		  glVertex3f(15,30,1);
		  
		  glVertex3f(0,40,1);
		  glVertex3f(4,58,1);
		  glVertex3f(8,54,1);
		  
		  glVertex3f(0,40,1);
		  glVertex3f(-16,32,1);
		  glVertex3f(-13,28,1);
		  glEnd();
	  }
	  glPopMatrix();
}

/*
 *  Draw a swing
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void swing(
	double x,double y,double z,double dx,double dy,double dz,double th)
{
	 // Define Material for specular and emission
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
	  glPushMatrix();
	  glTranslated(x,y,z);
	  glRotated(th,0,1,0);
	  glScaled(dx,dy,dz);

	  glColor3f(0.53,0.16,0.02);
	  cylinder(15,10,-5,0,105,0.5,20);
	  cylinder(15,10,5,0,75,0.5,20);
	  
	  glColor3f(0.53,0.16,0.02);
	  cylinder(-15,10,-5,0,105,0.5,20);
	  cylinder(-15,10,5,0,75,0.5,20);
	  
	  glColor3f(1,1,0);
	  glRotated(-90,0,1,0);
	  cylinder(0,30,0,0,0,0.5,20);
	  
	  glLineWidth(2);
	  glColor3f(1,1,1);
	  glBegin(GL_LINES);
	  glNormal3f(1,0,0);
	  glVertex3f(0,0,10);
	  glVertex3f(0,30,10);	  
	  glVertex3f(0,0,4);
	  glVertex3f(0,30,4);	  
	  glVertex3f(0,0,-4);
	  glVertex3f(0,30,-4);
	  glVertex3f(0,0,-10);
	  glVertex3f(0,30,-10);
	  glEnd();
	  
	  glColor3f(1,1,0);
	  glBegin(GL_QUADS);
	  glNormal3f(0,1,0);
	  glVertex3f(-2,0,10);
	  glVertex3f(-2,0,4);
	  glVertex3f(2,0,4);
	  glVertex3f(2,0,10);
	  
	  glVertex3f(-2,0,-4);
	  glVertex3f(-2,0,-10);
	  glVertex3f(2,0,-10);
	  glVertex3f(2,0,-4);
	  glEnd();

      glPopMatrix();
}

/*
 *  Draw a seesaw
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void seesaw(
	double x,double y,double z,double dx,double dy,double dz,double th)
{
	 // Define Material for specular and emission
      glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
      glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   
	  glPushMatrix();
	  glTranslated(x,y,z);
	  glRotated(th,0,1,0);
	  glScaled(dx,dy,dz);
	 
	  glBegin(GL_QUADS);
	  // Base
	  glColor3f(0,1,0);
	  glNormal3f(0,-1,0);
	  glVertex3f(3,-7,3);
	  glVertex3f(-3,-7,3);
	  glVertex3f(-3,-7,-3);
	  glVertex3f(3,-7,-3);
	  
	  // Sides
	  glColor3f(1,0,0);
	  glNormal3f(0,-1,1);
	  glVertex3f(3,-7,3);
	  glVertex3f(-3,-7,3);
	  glVertex3f(-3,0,0);
	  glVertex3f(3,0,0);
	  
	  glNormal3f(0,1,1);
	  glVertex3f(3,-7,-3);
	  glVertex3f(-3,-7,-3);
	  glVertex3f(-3,0,0);	  
	  glVertex3f(3,0,0);
	  
	  // Faces
	  glColor3f(0,1,0);
	  glNormal3f(1,0,0);
	  glVertex3f(3,-7,3);
	  glVertex3f(3,0,0);
	  glVertex3f(3,-7,-3);	  
	  glVertex3f(3,-7,3);
	  
	  glNormal3f(-1,0,0);
	  glVertex3f(-3,-7,3);
	  glVertex3f(-3,0,0);
	  glVertex3f(-3,-7,-3);	  
	  glVertex3f(-3,-7,3);
	  glEnd();
	  
	  glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

	  // Base
	  if(seesawFlag == 1)
	  {
		  glBindTexture(GL_TEXTURE_2D,texture[21]);
		  glBegin(GL_QUADS);
		  glColor3f(1,1,1); // Down
		  glNormal3f(0,1,1);
		  glTexCoord2f(0,0); glVertex3f(3,-7,30);
		  glTexCoord2f(1,0); glVertex3f(3,0,0);
		  glTexCoord2f(1,1); glVertex3f(-3,0,0);
		  glTexCoord2f(0,1); glVertex3f(-3,-7,30);
		  glEnd();
		  
		  glBindTexture(GL_TEXTURE_2D,texture[22]);
		  glBegin(GL_QUADS);
		  glColor3f(1,1,0); // Up
		  glNormal3f(0,1,1);
		  glTexCoord2f(0,0); glVertex3f(3,0,0);
		  glTexCoord2f(1,0); glVertex3f(3,7,-30);
		  glTexCoord2f(1,1); glVertex3f(-3,7,-30);
		  glTexCoord2f(0,1); glVertex3f(-3,0,0);
		  glEnd();
		  
		  glLineWidth(2);
		  glColor3f(1,0,0);
		  glBegin(GL_LINES);
		  glVertex3f(3.01,-7,30.01);
		  glVertex3f(3.01,0,0);
		  glVertex3f(-3.01,-7,30.01);
		  glVertex3f(-3.01,0,0);
		  glColor3f(1,1,1);
		  glVertex3f(3.01,0,0);
		  glVertex3f(3.01,7,-30.01);
		  glVertex3f(-3.01,0,0);
		  glVertex3f(-3.01,7,-30.01);
		  glEnd();
	  }
	  else 
	  {
		  glBindTexture(GL_TEXTURE_2D,texture[21]);
		  glBegin(GL_QUADS);
		  glColor3f(1,1,1); // Up
		  glNormal3f(0,1,-1);
		  glTexCoord2f(0,0); glVertex3f(3,0,0);
		  glTexCoord2f(1,0); glVertex3f(3,7,30);
		  glTexCoord2f(1,1); glVertex3f(-3,7,30);
		  glTexCoord2f(0,1); glVertex3f(-3,0,0);
		  glEnd(); 
		  
		  glBindTexture(GL_TEXTURE_2D,texture[22]);
		  glBegin(GL_QUADS);
		  glColor3f(1,1,0);  // Down
		  glNormal3f(0,1,-1);
		  glTexCoord2f(0,0); glVertex3f(-3,-7,-30);
		  glTexCoord2f(1,0); glVertex3f(3,-7,-30);
		  glTexCoord2f(1,1); glVertex3f(3,0,0);
		  glTexCoord2f(0,1); glVertex3f(-3,0,0);
		  glEnd(); 
		  
		  glLineWidth(2);
		  glColor3f(1,0,0);
		  glBegin(GL_LINES);
		  glColor3f(1,0,0);
		  glVertex3f(3.01,0,0);
		  glVertex3f(3.01,7,30.01);
		  glVertex3f(-3.01,7,30.01);
		  glVertex3f(-3.01,0,0);		  
		  glColor3f(1,1,1);
		  glVertex3f(-3.01,0,0);
		  glVertex3f(-3.01,-7,-30.01);
		  glVertex3f(3.01,-7,-30.01);
		  glVertex3f(3.01,0,0);
		  glEnd();
	  }	  		
      glPopMatrix();
	  // Disable textures
	  glDisable(GL_TEXTURE_2D);
}

/*
 *  Check collision detection at position (x,y,z)
 *  with an object existing in the limits given
 *
 */
 int detectCollision(double ex, double ey, double ez)
 {
	int i,j;
	for(i=0;i<5;i++){
		for(j=0;j<6;j++){
			 double xmin = objectMap[i][0];
			 double xmax = objectMap[i][1];
			 double ymin = objectMap[i][2];
			 double ymax = objectMap[i][3];
			 double zmin = objectMap[i][4];
			 double zmax = objectMap[i][5];
			 
			 // Collision occured
			 if(ex>=xmin && ex<=xmax && ey>=ymin && ey<=ymax && ez>=zmin && ez<=zmax){
				 
				 if(ex>=xmin && ex<=xmax && th_camera>= 0 && th_camera <=90){					
						 	if(ex == 39){
								Ex+=1;
							}else{
								Ex-=(ex-xmin);	
							}
				 } else if(ex>=xmin && ex<=xmax && th_camera>= 180 && th_camera <=270){
					 Ex+=(ex-xmin);
				 } else if(ez>=zmin && ez<=zmax && th_camera>= 90 && th_camera <=180){
					 Ez-=(ez-zmin);
				 } else if(ez>=zmin && ez<=zmax && th_camera>= 270 && th_camera <=360){
					 Ez+=(ez-zmin);
				 }
				 return 1;
			 } else{
				colFlag = 0;
			 }
		}
	}
	return colFlag;
 }
 

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   // Get time
   time(&ltime);
   // Convert to local time
   newtime = localtime(&ltime);
   //  Length of axes
   const double len=80;
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   
   //  First Person Navigation - set coordinates
   if (mode == 1)
   {
      double deltaX = +2*dim*Sin(th_camera)*Cos(ph_camera);
	  double deltaY = -2*dim*Sin(ph_camera);
      double deltaZ = -2*dim*Cos(th_camera)*Cos(ph_camera);
	  
	  //int cFlag = detectCollision(Ex,Ey,Ez);
		  
	  gluLookAt(Ex,Ey,Ez , Ex+deltaX, Ey+deltaY, Ez+deltaZ , 0,Cos(ph_camera),0);
	//   if(!light)
	//   {
	// 	glWindowPos2i(5,25);
	// 	Print("CamAng = %d,%d CD=%d Eye=%f, %f, %f",th_camera,ph_camera,cFlag,Ex,Ey,Ez);
	//   }
   }
   //  Perspective - set eye position
   if (mode == 2)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim*Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez,0,0,0,0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   
   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light direction
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2] , 5);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
      glDisable(GL_LIGHTING);
	  
   // Draw Road
   object = 1;
   glColor3f(1, 1, 1);
   cube(0,-5,0,10,1,200,0);
   
   // Draw Footpath
   object = 4;
   glColor3f(1, 1, 1);
   cube(-20,-5,0,10,1,200,0);
   cube(20,-5,0,10,1,200,0);
   
   // Draw Grass
   object = 3;
   glColor3f(0,0.5,0);
   cube(-80,-5,0,50,1,200,0);
   cube(80,-5,0,50,1,200,0);
   
   //  Draw Houses
   object = 2;
   glColor3f(0,0,1);
   duplex(-75,5,-50,30,10,30,180, 0);
   glColor3f(0.85,0.53,0.1);
   mainHouse(85,5,-20,35,10,70,0,0);
   glColor3f(0.5,0.5,0);
   pentHouse(83,58,-35 ,20,10,40,0,0);
   glColor3f(0.3,0.7,0.4);
   house(-75,5,-135 ,30,10,20,180,0);
   fence(80,1,-20,48,5,120,0,0);
   

   // Draw Trees
    object = 5;	
	tree1(-20,15,120,1,1,1,0,0,0);
	tree1(20,15,-120,1,1,1,0,0,0);
	tree1(20,15,20,1,1,1,0,0,1);	
	tree2(-20,15,-140,1,1,1,0,0,7);
	tree3(-20,15,20,1,1,1,0,0,10);
		
	// Play Area
	// Draw Table and Chairs
	object = 6;
	glColor3f(1, 1, 1);			
	table1(-65,5,100,7,7,7,0);
	chair1(-58,-1,111,3,0.3,3,6,0);
	chair1(-72,-1,110,3,0.3,3,6,0);
	chair1(-58,-1,88,3,0.3,3,6,0);
	chair1(-72,-1,88,3,0.3,3,6,0);
	
   // Draw Tennis Court
    object = 0;
    glColor3f(0.137255,0.137255,0.556863);
    tennisCourt(-75,-3,30,30,0.05,20,0);
	
  // Draw Swing
    glColor3f(1,1,1);
	swing(-100,5,90,1,1,1,90);
	
   // Draw Seesaw
    glColor3f(1,1,1);
	seesaw(-80,5,135,1,1,1,90);
	
	// Fancy Lights Decoration
	object = 7;
	glColor3f(0.5, 0.35, 0.05);	
	cone(110,-5,140,0,-90,2,30);
	glColor3f(1,0.5,0);
	sphere(110,29,140,4); 
	
	glColor3f(0.5, 0.35, 0.05);	
	cone(50,-5,140,0,-90,2,30);
	glColor3f(1,0.5,0);
	sphere(50,29,140,4); 
	
	glColor3f(0.5, 0.35, 0.05);	
	cone(120,-5,-120,0,-90,2,30);
	glColor3f(0.5,1.0,0.5);
	sphere(120,29,-120,4); 
	
	glColor3f(0.5, 0.35, 0.05);	
	cone(50,-5,-120,0,-90,2,30);
	glColor3f(0.5,1.0,0.5);
	sphere(50,29,-120,4); 
	
	// Draw Swimming Pool
   object = 10;
   glColor3f(1,1,1);
   pool(80,-1,110,25,0.05,20,0);
   glColor3f(0.53,0.12,0.47);
   sphere(70,0,100,1);
   glColor3f(1,0.5,0);
   sphere(95,-0.5,100,1);
   glColor3f(1,1,0);
   sphere(80,-0.75,105,1);
   glColor3f(1,0,0);
   torus(80,0,120);
   glColor3f(1,1,1);
   torus(95,-1,100);
   // Draw Waves in Swimming Pool;
   drawWaves(80,-1,110,0.18,0.1,0.14);
   
   // Draw Windmill
   windmill(-110,0,-170,1,1,1,1,0);
   windmill(110,0,-170,1,1,1,1,0);
	
   // Draw House Utilities
	interiorWall(92,15, -70 ,23,20,70 , 0);
	
	// Draw Wall Clocks
	glColor3f(1, 1, 1);
	wallClock(80,25,0,0.5,0.5,0.5);
	wallClock(80,25,-89,0.5,0.5,0.5);
	
	// Draw Fans	
	fan(70,1.7, -30, 10, 8, -10); // Bedroom
	fan(70,1.7, 40, 10, 8, -10); // Hall
	fan(70,43, -5, 10, 8, -10); // Upstairs
	
	// Draw Dining table and chairs
	glRotated(0,0,0,1);
	diningtable();
	
	// Draw beds
	cot(90,-6,25,2,2.5,2); // Main House
	cot(86,38,-40,1.5,1.5,1); // Pent House
	cot(-80,-4,-50,2,1,2); // Duplex

	// Draw staircase
	steps();
	
	// Draw couch
	glColor3f(1,1,1);
	couch();
	
	//Draw bed lamp 
    glColor3f(1,1,0.1);
	sphere(100,20,5,2); // Main house 
	sphere(93.5,55,-50,2); // Pent house
	
    //Draw Street lights
    glColor3f(0,0,1);
	glLineWidth(5);	
	glBegin(GL_LINES);
	glNormal3f(0, 0, 1);
    glVertex3f(-50,-5,80);
    glVertex3f(-50,30,80);
    glEnd();
    glColor3f(1,1,1);
    sphere(-50,30,80,5);
	
	glColor3f(0,0,1);
	glLineWidth(5);	
	glBegin(GL_LINES);
	glNormal3f(1, 0, 0);
    glVertex3f(0,-5,30);
    glVertex3f(0,30,30);
    glEnd();
    glColor3f(1,1,1);
    sphere(0,30,30,5); 
	  
	glColor3f(0,0,1);
	glLineWidth(5);	
	glBegin(GL_LINES);
	glNormal3f(1, 0, 0);
    glVertex3f(0,-5,-30);
    glVertex3f(0,30,-30);
    glEnd();
    glColor3f(1,1,1);
    sphere(0,30,-30,5); 
	  
	glColor3f(0,0,1);
	glLineWidth(5);	
	glBegin(GL_LINES);
	glNormal3f(1, 0, 0);
    glVertex3f(0,-5,-100);
    glVertex3f(0,30,-100);
    glEnd();
    glColor3f(1,1,1);
    sphere(0,30,-100,5);
	  
	// Draw axes - no lighting for axes
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   // glWindowPos2i(5,5);
   // Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",th,ph,dim,fov,mode == 1 ?"FPN": (mode == 2? "Perpective":"Orthogonal"),light?"On":"Off");
   // if (light)
   // {
   //    glWindowPos2i(5,25);
   //    Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shinyvec[0]);
   // }
   
   //  Render the scene and make it visible  
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}


/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   int ts = t*10;
   zh = fmod(90*t,360.0);
   wflag = ts%2;
   phaseAngle += phaseInc;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow
   if (key == GLUT_KEY_RIGHT)
   {
	   if(mode == 1) // FPN
			th_camera += 1; // Increase camera angle by 5
		else
			th += 5;
   }
   //  Left arrow key
   else if (key == GLUT_KEY_LEFT)
   {
	   if(mode == 1) // FPN
			th_camera -= 1; // Decrease camera angle by 5
 		else
			th -= 5;
   }
   //  Up arrow
   else if (key == GLUT_KEY_UP)
   {
	   if(mode == 1) // FPN
	   {
		    Ex -= -Sin(th_camera)*Cos(ph_camera); // Update Coordinates
			Ez -= Cos(th_camera)*Cos(ph_camera);
	   }
	   else
		   ph -= 5;
   }
   //  Down arrow
   else if (key == GLUT_KEY_DOWN)
   {
	   if(mode == 1) // FPN
	   {
		   Ex += -Sin(th_camera)*Cos(ph_camera); // Update Coordinates
		   Ez += Cos(th_camera)*Cos(ph_camera);
	   }
	   else
		   ph += 5;
   }
   //  PageUp - climb up
   else if (key == GLUT_KEY_PAGE_UP){
	  Ex += Cos(ph_camera); 
	  Ey += Cos(ph_camera); 
   }
   //  PageDown - climb down
   else if (key == GLUT_KEY_PAGE_DOWN){
	  Ex -= Cos(ph_camera); 
	  Ey -= Cos(ph_camera); 	  
   }
   //  F1 - decrease dim
   else if (key == GLUT_KEY_F1 && dim>1){
      dim -= 1;
   }
   //  F2 - increase dim
   else if (key == GLUT_KEY_F2){
      dim += 1;
   }
   //  F3 - increase azimuth angle
   else if (key == GLUT_KEY_F3){
      th += 5;
   }
   //  F4 - decrease azimuth angle
   else if (key == GLUT_KEY_F4){
	  th -= 5;	
   }
   //  F5 - increase elevation angle
   else if (key == GLUT_KEY_F5){
      ph += 5;
   }
   //  F6 - decrease elevation angle
   else if (key == GLUT_KEY_F6){
	  ph -= 5;	
   }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   th_camera %= 360;
   ph_camera %= 360;
   //  Update projection
   if(mode == 0)
		Project(0,asp,dim);
   else
	   Project(fov,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
   {
	   th = ph = 0;
	   if(mode == 1)
	   {
			th_camera = ph_camera = 0;
			Ex = 0;
			Ey = 10;
			Ez = 200;
	   }
   } 
   //  Toggle axes
   else if (ch == 'u' || ch == 'U')
      axes = 1 - axes;
   else if (ch == 'x')
      Ex -=0.5;
   else if (ch == 'X')
	  Ex +=0.5;
   else if (ch == 'y')
	  Ey -= 0.5;
   else if (ch == 'Y')
	  Ey += 0.5;
   else if (ch == 'z')
      Ez -=0.5;
   else if (ch == 'Z')
	  Ez +=0.5;
   else if (ch == 't')
      th_camera -= 1;
   else if (ch == 'T')
	  th_camera  += 1;
   else if (ch == 'c')
      ph_camera -= 1;
   else if (ch == 'C')
	  ph_camera  += 1;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
  // First Person Navigation Mode
   else if (ch == 'n' || ch == 'N')
	  mode = mode == 1 ? 2 : 1;
   //  Perspective mode
   else if (ch == 'p' || ch == 'P')
      mode = 2;
  // Orthogonal Mode
   else if (ch == 'o' || ch == 'O')
	   mode = 0;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov++;
   else if (ch == '+' && ch<179)
      fov--;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.5;
   else if (ch==']')
      ylight += 0.5;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='h' && shininess>-1)
      shininess -= 1;
   else if (ch=='H' && shininess<7)
      shininess += 1;  
   else if (ch == 'g' || ch =='G')
	   gateFlag = 1- gateFlag;
   else if (ch == 'f' || ch =='F')
	   fanFlag = 1- fanFlag;
   else if (ch == 'w' || ch =='W')
	   seesawFlag = 1- seesawFlag;
   else if (ch == 'i' || ch =='I') // Bed room View
   {
	   th_camera = 60;
	   ph_camera = 0;
	   Ex = 49.45;
	   Ey = 10;
	   Ez = 40.59;
	   th = -2;
	   ph = 23;
	   dim = 80;
	   gateFlag = 1;
	   
   }
   else if (ch == 'j' || ch =='J') //  Hall View
   {
	   th_camera = -14;
	   ph_camera = 0;
	   Ex = 1.522;
	   Ey = 29.5;
	   Ez = -51.08;
	   th = 76;
	   ph = 2;
	   dim = 80;
	   gateFlag = 1;
   }
   else if (ch == 'k' || ch =='K') // Pent House View
   {
	   th_camera = 68;
	   ph_camera = 0;
	   Ex = 62.51;
	   Ey = 52.5;
	   Ez = 11.14;
	   th = -6;
	   ph = 6;
	   dim = 80;
   }
   else if (ch == 'r' || ch =='R') // Picnic View
   {
		th_camera = -51;
		ph_camera = 0;
		Ex = -27.12;
	    Ey = 9.5;
	    Ez = 138.83;
	    th = 0;
	    ph = 0;
		dim = 80;
   }
   else if (ch == 'b' || ch =='B') // Top View
   {
		th_camera = 1;
		ph_camera = 2;
		Ex = -9.42;
	    Ey = 10;
	    Ez = 226.5;
	    th = 90;
	    ph = 60;
		dim = 80;
   }
   else if (ch =='v'){
	   	z++;
		phaseInc++;
   }
   else if (ch =='V'){
	   	z--;
		phaseInc--;
   }
      
  //  Translate shininess power to value (-1 => 0)
   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   if(mode == 0)
		Project(0,asp,dim);
   else
	   Project(fov,asp,dim);
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   if(mode == 0)
		Project(0,asp,dim);
   else
	   Project(fov,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("House Tour");

   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Load textures
   texture[0] = LoadTexBMP("walls.bmp");
   texture[1] = LoadTexBMP("door.bmp");
   texture[2] = LoadTexBMP("window.bmp");
   texture[3] = LoadTexBMP("roof.bmp");
   texture[4] = LoadTexBMP("ceiling.bmp");
   texture[5] = LoadTexBMP("road.bmp");
   texture[6] = LoadTexBMP("grass.bmp");
   texture[7] = LoadTexBMP("footpath.bmp");
   texture[8] = LoadTexBMP("trunk.bmp");
   texture[9] = LoadTexBMP("chair.bmp");
   texture[10] = LoadTexBMP("chair_legs.bmp");
   texture[11] = LoadTexBMP("tree1.bmp");
   texture[12] = LoadTexBMP("lights.bmp");
   texture[13] = LoadTexBMP("table.bmp");
   texture[14] = LoadTexBMP("bed.bmp");
   texture[15] = LoadTexBMP("window1.bmp");
   texture[16] = LoadTexBMP("steps.bmp");
   texture[17] = LoadTexBMP("water.bmp");
   texture[18] = LoadTexBMP("gate.bmp");
   texture[19] = LoadTexBMP("pooltiles.bmp");
   texture[20] = LoadTexBMP("tennisnet.bmp");
   texture[21] = LoadTexBMP("seesaw1.bmp");
   texture[22] = LoadTexBMP("seesaw2.bmp");
   texture[23] = LoadTexBMP("fence.bmp");
   texture[24] = LoadTexBMP("tree3.bmp");
   texture[25] = LoadTexBMP("wall1.bmp");
   texture[26] = LoadTexBMP("floor.bmp");
   texture[27] = LoadTexBMP("wall2.bmp");
   texture[28] = LoadTexBMP("tree2.bmp");
   //  Pass control to GLUT so it can interact with the user 
   glutMainLoop();
   return 0;
}
