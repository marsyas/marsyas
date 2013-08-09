
#include "VauxelObject.h"

VauxelObject::VauxelObject()
{
  //to do
}

VauxelObject::~VauxelObject()
{
  //to do
}

void
VauxelObject::draw() const
{
//   glPushMatrix();
//
//   // Move the object back from the screen.
//   glTranslatef(0.0f,0.0f,-0.05);
//
//   // OK, let's start drawing our planer quads.
//   glBegin(GL_QUADS);
//
//   // Bottom Face.  Red, 75% opaque, magnified texture
//
//   glNormal3f( 0.0f, -1.0f, 0.0f); // Needed for lighting
//   glColor4f(0.9,0.2,0.2,.75); // Basic polygon color
//
//   glTexCoord2f(0.800f, 0.800f); glVertex3f(-1.0f, -1.0f, -1.0f);
//   glTexCoord2f(0.200f, 0.800f); glVertex3f( 1.0f, -1.0f, -1.0f);
//   glTexCoord2f(0.200f, 0.200f); glVertex3f( 1.0f, -1.0f,  1.0f);
//   glTexCoord2f(0.800f, 0.200f); glVertex3f(-1.0f, -1.0f,  1.0f);
//
//
//   // Top face; offset.  White, 50% opaque.
//
//   glNormal3f( 0.0f, 1.0f, 0.0f);
//   glColor4f(0.5,0.5,0.5,.5);
//
//   glTexCoord2f(0.005f, 1.995f); glVertex3f(-1.0f,  1.3f, -1.0f);
//   glTexCoord2f(0.005f, 0.005f); glVertex3f(-1.0f,  1.3f,  1.0f);
//   glTexCoord2f(1.995f, 0.005f); glVertex3f( 1.0f,  1.3f,  1.0f);
//   glTexCoord2f(1.995f, 1.995f); glVertex3f( 1.0f,  1.3f, -1.0f);
//
//
//   // Far face.  Green, 50% opaque, non-uniform texture cooridinates.
//
//   glNormal3f( 0.0f, 0.0f,-1.0f);
//   glColor4f(0.2,0.9,0.2,.5);
//
//   glTexCoord2f(0.995f, 0.005f); glVertex3f(-1.0f, -1.0f, -1.3f);
//   glTexCoord2f(2.995f, 2.995f); glVertex3f(-1.0f,  1.0f, -1.3f);
//   glTexCoord2f(0.005f, 0.995f); glVertex3f( 1.0f,  1.0f, -1.3f);
//   glTexCoord2f(0.005f, 0.005f); glVertex3f( 1.0f, -1.0f, -1.3f);
//
//
//   // Right face.  Blue; 25% opaque
//
//   glNormal3f( 1.0f, 0.0f, 0.0f);
//   glColor4f(0.2,0.2,0.9,.25);
//
//   glTexCoord2f(0.995f, 0.005f); glVertex3f( 1.0f, -1.0f, -1.0f);
//   glTexCoord2f(0.995f, 0.995f); glVertex3f( 1.0f,  1.0f, -1.0f);
//   glTexCoord2f(0.005f, 0.995f); glVertex3f( 1.0f,  1.0f,  1.0f);
//   glTexCoord2f(0.005f, 0.005f); glVertex3f( 1.0f, -1.0f,  1.0f);
//
//
//   // Front face; offset.  Multi-colored, 50% opaque.
//
//   glNormal3f( 0.0f, 0.0f, 1.0f);
//
//   glColor4f( 0.9f, 0.2f, 0.2f, 0.5f);
//   glTexCoord2f( 0.005f, 0.005f); glVertex3f(-1.0f, -1.0f,  1.3f);
//   glColor4f( 0.2f, 0.9f, 0.2f, 0.5f);
//   glTexCoord2f( 0.995f, 0.005f); glVertex3f( 1.0f, -1.0f,  1.3f);
//   glColor4f( 0.2f, 0.2f, 0.9f, 0.5f);
//   glTexCoord2f( 0.995f, 0.995f); glVertex3f( 1.0f,  1.0f,  1.3f);
//   glColor4f( 0.1f, 0.1f, 0.1f, 0.5f);
//   glTexCoord2f( 0.005f, 0.995f); glVertex3f(-1.0f,  1.0f,  1.3f);
//
//
//   // Left Face; offset.  Yellow, varying levels of opaque.
//
//   glNormal3f(-1.0f, 0.0f, 0.0f);
//
//   glColor4f(0.9,0.9,0.2,0.0);
//   glTexCoord2f(0.005f, 0.005f); glVertex3f(-1.3f, -1.0f, -1.0f);
//   glColor4f(0.9,0.9,0.2,0.66);
//   glTexCoord2f(0.995f, 0.005f); glVertex3f(-1.3f, -1.0f,  1.0f);
//   glColor4f(0.9,0.9,0.2,1.0);
//   glTexCoord2f(0.995f, 0.995f); glVertex3f(-1.3f,  1.0f,  1.0f);
//   glColor4f(0.9,0.9,0.2,0.33);
//   glTexCoord2f(0.005f, 0.995f); glVertex3f(-1.3f,  1.0f, -1.0f);
//
//
//   // All polygons have been drawn.
//   glEnd();
//
//   // Move back to the origin (for the text, below).
//   glPopMatrix();
}