/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/**
   \class MarxGLMultiBufferGraph.cpp
   \brief Graphing multiple buffers along the z-axis
                                                                                        
Provides graphing for Marsyas realvecs where older buffers are moved
further back along the Z axis as new buffers are introduced.
*/

#include "MarxGLMultiBufferGraph.h"

using namespace std;
using namespace Marsyas;


MarxGLMultiBufferGraph::MarxGLMultiBufferGraph( QWidget* parent, long n, int d )
  : QGLWidget(parent) 
{
  buffersize = n*d;
  depth = d;
  buffer.allocate( buffersize );

  model_rotate_x = 0.0f;
  model_rotate_y = 0.0f;
  model_rotate_z = 0.0f;
  model_translate_x = 0.0f;
  model_translate_y = 0.0f;
  model_translate_z = 0.0f;

  projection_rotate_x = 30.f;
  projection_rotate_y = 0.0f;
  projection_rotate_z = 0.0f;
  projection_translate_x = 0.0f;
  projection_translate_y = -0.5f;
  projection_translate_z = -2.f; 

  bgcolor = QColor::fromRgbF(1.0, 1.0, 1.0);
}


float
MarxGLMultiBufferGraph::getXProjectionTranslation()
{
  return projection_translate_x;
}


float
MarxGLMultiBufferGraph::getYProjectionTranslation()
{
  return projection_translate_y;
}


float
MarxGLMultiBufferGraph::getZProjectionTranslation()
{
  return projection_translate_z;
}


float
MarxGLMultiBufferGraph::getXProjectionRotation()
{
  return projection_rotate_x;
}


float
MarxGLMultiBufferGraph::getYProjectionRotation()
{
  return projection_rotate_y;
}


float
MarxGLMultiBufferGraph::getZProjectionRotation()
{
  return projection_rotate_z;
}

void 
MarxGLMultiBufferGraph::modelRotate( int axis, float v )
{

  switch (axis) {
  case X:
    model_rotate_x = v;
    break;
  case Y:
    model_rotate_y = v;
    break;
  case Z:
    model_rotate_z = v;
    break;
  }
  updateGL();

}


void 
MarxGLMultiBufferGraph::modelTranslate( int axis, float v )
{

  switch (axis) {
  case X:
    model_translate_x = v;
    break;
  case Y:
    model_translate_y = v;
    break;
  case Z:
    model_translate_z = v;
    break;
  }
  updateGL();

}


void 
MarxGLMultiBufferGraph::projectionRotate( int axis, float v )
{

  switch (axis) {
  case X:
    projection_rotate_x = v;
    break;
  case Y:
    projection_rotate_y = v;
    break;
  case Z:
    projection_rotate_z = v;
    break;
  }
  updateGL();

}


void 
MarxGLMultiBufferGraph::projectionTranslate( int axis, float v )
{

  switch (axis) {
  case X:
    projection_translate_x = v;
    break;
  case Y:
    projection_translate_y = v;
    break;
  case Z:
    projection_translate_z = v;
    break;
  }
  updateGL();

}


bool
MarxGLMultiBufferGraph::setBuffer( realvec& rv )
{
    if ( rv.getSize() == buffersize ) { 
     buffer = rv;
     updateGL();
      
    return true;
  }
  return false;
}


void 
MarxGLMultiBufferGraph::initializeGL()
{
  glEnable( GL_LINE_SMOOTH );

  qglClearColor( bgcolor );
}


/* 
   currently this is a messy two-pass deal, necessitated by the
   drawing of a wireframe.  opengl is good aboud hidden surface
   removal, but hidden line removal is trickier business apparently.
   also the transparency on the polygon plane at y=0 was't appearing
   properly, ie. with some transparency and the correct depth
   overlapping.

   clean-up and improvement projects for a rainy day.
*/
void 
MarxGLMultiBufferGraph::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set the projection matrix
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluPerspective ( 60, 1, 0, depth*2 );
  glTranslatef( projection_translate_x, 
		projection_translate_y, 
		projection_translate_z);
  glRotatef( projection_rotate_x, 1.f, 0.f, 0.f );
  glRotatef( projection_rotate_y, 0.f, 1.f, 0.f );
  glRotatef( projection_rotate_z, 0.f, 0.f, 1.f );

  // set the transformation to apply to the model
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  glTranslatef( model_translate_x, 
		model_translate_y, 
		model_translate_z);
  glRotatef( model_rotate_x, 1.f, 0.f, 0.f );
  glRotatef( model_rotate_y, 0.f, 1.f, 0.f );
  glRotatef( model_rotate_z, 0.f, 0.f, 1.f );


  // add the buffer curves
  glLineWidth(1.0);

  float h = 0.0;
  for (int g=0; g<depth || g<buffer.getCols(); g++) { // add the bottom half of the data first

    // now construct the model  
    glBegin( GL_LINES );
    glColor3f( 0.f, 0.f, 0.f );
    
    for (int i=0; i<buffer.getRows(); i++) {
      float val1 = (float)buffer(i, g);
      float val2 = (float)buffer(i+1, g);

      if (val1 < 0) {
	glVertex3f( (((float)i)*2.f/(float)buffer.getRows()) - 1.0, 
		    val1,
		    -1. * h );
	glVertex3f( (((float)i+1)*2/(float)buffer.getRows()) - 1.0, 
		    val2,
		    -1. * h);
      }

    }

    glEnd();
    h++;  // set the next depth
  }

  // add some axis
  glBegin( GL_LINES );
  glLineWidth(0.25);
  glColor3f( 0.7f, 0.7f, 0.7f );
  for (int i=0; i<depth; i++) {
    glVertex3d( -1.0, 0.0, -1*i);
    glVertex3d( +1.0, 0.0, -1*i);
  }

  for (float i=-1.0; i<=1; i+=.5) {
    glVertex3d( i, 0.0, 0.f);
    glVertex3d( i, 0.0, -1*(depth-1));  
  }
  glEnd( );


  h = 0.0;
  for (int g=0; g<depth || g<buffer.getCols(); g++) { // now add the top half of the data

    // now construct the model  
    glBegin( GL_LINES );
    glColor3f( 0.f, 0.f, 0.f );
    
    for (int i=0; i<buffer.getRows(); i++) {
      float val1 = (float)buffer(i, g);
      float val2 = (float)buffer(i+1, g);

      if (val1 > 0) {
	glVertex3f( (((float)i)*2.f/(float)buffer.getRows()) - 1.0, 
		    val1,
		    -1. * h );
	glVertex3f( (((float)i+1)*2/(float)buffer.getRows()) - 1.0, 
		    val2,
		    -1. * h);
      }
    }

    glEnd();
    h++;  // set the next depth
  }

  // add a simple axis
//   glEnable( GL_DEPTH_TEST );
//   glBegin( GL_QUADS );
//   glEnable(GL_BLEND);
//   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//   glColor4f( 0.9f, 0.9f, 0.9f, .5f );
//   glColor3f( bgcolor.redF(), bgcolor.greenF(), bgcolor.blueF() );
//   glVertex3d( -1.0, 0.0, 0.f);
//   glVertex3d( 1.0, 0.0, 0.f);
//   glVertex3d( 1.0, 0.0, -1*depth);
//   glVertex3d( -1.0, 0.0, -1*depth);
//   glEnd( );
//   glDisable( GL_BLEND );
//   glDisable( GL_DEPTH_TEST );




}


void
MarxGLMultiBufferGraph::resizeGL(int width, int height)
{
  glViewport(-1, -1, width, height);
}
