//This code is in the public domain. --author, Curran Kelleher
#include <bits/stdc++.h>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GL/glfw3.h>

// Other Libs
#include <GL/SOIL.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define ESCAPE_KEY 27

float window;
float t = 0.0f;
int pointIsInsideCylinder(float x1, float y1, float z1, float dx,float dy, float dz, float lengthSquared,float radiusSquared ,float xTest,float yTest,float zTest)
{
   float pdx = xTest - x1;
   float pdy = yTest - y1;
   float pdz = zTest - z1;
   float dot = pdx * dx + pdy * dy + pdz * dz;
   int pointIsInside = 0;
   if( dot > 0 && dot < lengthSquared )
       pointIsInside = pdx*pdx + pdy*pdy + pdz*pdz - dot*dot/lengthSquared < radiusSquared;
   return pointIsInside;
}
int pointIsInsideCylinder_convenient( float x1, float y1, float z1, float x2,float y2, float z2, float radius, float xTest,float yTest,float zTest)
{
   //these things need only to be computed once for a cylinder, and reused for each test point
   float dx = x2 - x1;
   float dy = y2 - y1;
   float dz = z2 - z1;
   float lengthSquared = pow(x2-x1,2)+pow(y2-y1,2)+pow(z2-z1,2);
   float radiusSquared=radius*radius;
   return pointIsInsideCylinder(x1,y1,z1,dx,dy,dz,lengthSquared,radiusSquared,xTest,yTest,zTest);
}


void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric)
{
   float vx = x2-x1;
   float vy = y2-y1;
   float vz = z2-z1;

   //handle the degenerate case of z1 == z2 with an approximation
   if(vz == 0)
       vz = .00000001;

   float v = sqrt( vx*vx + vy*vy + vz*vz );
   float ax = 57.2957795*acos( vz/v );
   if ( vz < 0.0 )
       ax = -ax;
   float rx = -vy*vz;
   float ry = vx*vz;
   glPushMatrix();

   //draw the cylinder body
   glTranslatef( x1,y1,z1 );
   glRotatef(70.0f, 1.0, 0.0, 0.0);
   gluQuadricOrientation(quadric,GLU_OUTSIDE);
   gluCylinder(quadric, radius, radius/2, v, subdivisions, 1);

   //draw the first cap
   gluQuadricOrientation(quadric,GLU_INSIDE);
   gluDisk( quadric, 0.0, radius, subdivisions, 1);
   glTranslatef( 0,0,v );

   //draw the second cap
   gluQuadricOrientation(quadric,GLU_OUTSIDE);
   gluDisk( quadric, 0.0, radius/2, subdivisions, 1);
   glPopMatrix();
}
void renderCylinder_convenient(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions)
{
   //the same quadric can be re-used for drawing many cylinders
   GLUquadricObj *quadric=gluNewQuadric();
   gluQuadricNormals(quadric, GLU_SMOOTH);
   renderCylinder(x1,y1,z1,x2,y2,z2,radius,subdivisions,quadric);
   gluDeleteQuadric(quadric);
}
void renderSphere(float x, float y, float z, float radius,int subdivisions,GLUquadricObj *quadric)
{
   glPushMatrix();
   glTranslatef( x,y,z );
   gluSphere(quadric, radius, subdivisions,subdivisions);
   glPopMatrix();
}

void renderSphere_convenient(float x, float y, float z, float radius,int subdivisions)
{
   //the same quadric can be re-used for drawing many spheres
   GLUquadricObj *quadric=gluNewQuadric();
   gluQuadricNormals(quadric, GLU_SMOOTH);
   renderSphere(x,y,z,radius,subdivisions,quadric);
   gluDeleteQuadric(quadric);
}

void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();
   glTranslatef(0,0,-7);

   //define the cylinder
   float x1 = sin(t);
   float y1 = sin(t/2);
   float z1 = cos(t*1.1);
   float x2 = -sin(t*1.3);
   float y2 = 0;
   float z2 = -cos(t);
   float radius = 0.03+(sin(t)/2+0.5)/3;

   //render the cylinder
   renderCylinder_convenient(x1,y1,z1,x2,y2,z2,radius,32);

   // //render spheres in a grid which are inside the cylinder
   // float gridSize = 20;
   // float xTest,yTest,zTest;
   // int x,y,z;
   // for(x = 0; x < gridSize,xTest = (float)x/gridSize*2-1; x++)
   //     for(y = 0; y < gridSize,yTest = (float)y/gridSize*2-1; y++)
   //         for(z = 0; z < gridSize,zTest = (float)z/gridSize*2-1; z++)
   //             if(pointIsInsideCylinder_convenient(x1,y1,z1,x2,y2,z2,radius,xTest,yTest,zTest))
   //                 renderSphere_convenient(xTest,yTest,zTest,0.07,8);

   // t+=0.001;
   glutSwapBuffers();
}
void InitGL(int Width, int Height)        
{
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClearDepth(1.0);
   glDepthFunc(GL_LESS);
   glEnable(GL_DEPTH_TEST);

   glShadeModel(GL_SMOOTH);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
   glMatrixMode(GL_MODELVIEW);

   // glEnable(GL_LIGHTING);
   // GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
   // GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
   // GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
   // glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
   // glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
   // glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
   // glEnable(GL_LIGHT1);
}

void keyPressed(unsigned char key, int x, int y)
{
   if(key == ESCAPE_KEY)
   {
       glutDestroyWindow(window);
       exit(1);
   }
}

int main( int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(640, 480);
   window = glutCreateWindow("");
   glutDisplayFunc(&display);
   glutKeyboardFunc(&keyPressed);
   glutFullScreen();
   glutIdleFunc(&display);
   InitGL(640, 480);
   glutMainLoop();
   return 1;
}
