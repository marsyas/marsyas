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
   \class MarxGL2Din3DSpaceGraph.cpp
   \brief A 2D graph rotated into 3d space
*/

#include "MarxGL2Din3DSpaceGraph.h"

using namespace std;
using namespace Marsyas;

//! The constructor
/*!
  The constructor
                                                                                        
  \param parent a pointer to the parent widget
  \param long the number of elements in the realvec buffer
*/
MarxGL2Din3DSpaceGraph::MarxGL2Din3DSpaceGraph(QWidget *parent, long b)
  : QGLWidget(parent)
{

  x_axis_stretch = 1.0;
  y_axis_stretch = 1.0;

  y_normalize_factor = 1.0;

  x_translation = 0.0;
  y_translation = 0.0;
  z_translation = 0.0;

  y_rotation = 0.0;

  bgcolor = QColor::fromRgbF(1.0, 1.0, 1.0);
  buffersize = b;
  buffer = new realvec( buffersize );

}


//! setBuffer
/*!
  Update the realvec buffer and redraw the widget.  Returns false if
  the realvec argument size and the buffersize of this object differ.

  \param realvec& the new buffer of data
  \return bool indicates if the buffer was updated
*/
bool 
MarxGL2Din3DSpaceGraph::setBuffer( realvec& rv )
{
  if ( rv.getSize() == buffersize ) { 
     *buffer = rv;

      updateGL();

    return true;
  }
  return false;
}


//! setXAxisStretch
/*!
  Multiplies the normalized x axis by a stretch factor.  This effect
  would be useful for particularly dense changes in the data or to
  give the impression of the graph stretching far off into the
  distance.

  \para float most likely a value 1.0 or greater
*/
void 
MarxGL2Din3DSpaceGraph::setXAxisStretch( float xs )
{
  x_axis_stretch = xs;
  updateGL();
}


void 
MarxGL2Din3DSpaceGraph::setYAxisStretch( float ys )
{
  y_axis_stretch = ys;
  updateGL(); 
}


//! setYNormalizeFactor
/*!
  In order to graph without clipping, the ordinate must be normalized
  a value between -1.0 and 1.0.  Therefore, use this method to set a
  denominator with which to normalize the ordinate.  Most likely the
  parameter should be the greatest expected value or a little larger
  for your data.  The method could also be used to invert the data by
  supplying a negative argument.  The factor cannot be zero.

  \param float the normalizing factor, ie. something larger than the
  greatest expected data value
*/
bool
MarxGL2Din3DSpaceGraph::setYNormalizeFactor( float yn )
{
  if ( yn != 0) {
    y_normalize_factor = yn;
    updateGL();
    return true;
  }
  
  return false;
}


//! setXTranslation
/*!
  Move the model right or left along the viewplane's x-axis.

  \param float translation distance (probably between -1 and 1)
*/
void 
MarxGL2Din3DSpaceGraph::setXTranslation( float xt )
{
  x_translation = xt;
  updateGL();
}


void 
MarxGL2Din3DSpaceGraph::setYTranslation( float yt )
{
  y_translation = yt;
  updateGL();
}


void 
MarxGL2Din3DSpaceGraph::setZTranslation( float zt )
{
  z_translation = zt;
  updateGL();
}


void 
MarxGL2Din3DSpaceGraph::setYRotation( float yr )
{
  y_rotation = yr;
  updateGL();
}


void 
MarxGL2Din3DSpaceGraph::initializeGL()
{

  glEnable( GL_LINE_SMOOTH );

  qglClearColor( bgcolor );
  glLineWidth(1.0);


  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );

  gluPerspective ( 60, 1.2, 0, 20 );
  glTranslatef( 0.0f, 0.0f, -2.f);

}


void 
MarxGL2Din3DSpaceGraph::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // set the transformation to apply to the model
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  glTranslatef( x_translation, y_translation, z_translation );  // translate first
  glRotatef(y_rotation, 0.0, 1.0, 0.0);     // then rotate!

  // now construct the model  
  glBegin( GL_LINES );

  float blue, green, red;
  green = red = blue = 0.0;

  glColor3f( .8, .8, .8 );
  glVertex3f( -1.0, 0.0, -1.0 );
  glVertex3f( (-1+x_axis_stretch*2), 0.0, -1.0 );

//   glVertex3f( (-1*x_axis_stretch), 0.0, 0.0 );
  glVertex3f( -1.0, 0.0, 0.0 );
  glVertex3f( (-1+x_axis_stretch*2), 0.0, 0.0 );


  glVertex3f( -1.0, 0.0, 1.0 );
  glVertex3f( (-1+x_axis_stretch*2), 0.0, 1.0 );




  for (int i=0; i<buffer->getSize()-1; i++ ) {
    float val1 = (float)((*buffer)(i));
    float val2 = (float)((*buffer)(i+1));

    if ( i%32 == 0) {
      glColor3f( .8, .8, .8 );

      glVertex3f( x_axis_stretch * (((float)i)*2/(float)buffersize) - 1.0, 0.0, -1.0 );
      glVertex3f( x_axis_stretch * (((float)i)*2/(float)buffersize) - 1.0, 0.0, 1.0 );

      glColor3f( red, green, blue );
    }

    glColor3f( red, green, blue );

    glVertex3f( x_axis_stretch * (((float)i)*2/(float)buffersize) - 1.0, 
		(val1/y_normalize_factor)*y_axis_stretch, 
		0.0 );
    glVertex3f( x_axis_stretch * (((float)i+1)*2/(float)buffersize) - 1.0, 
		(val2/y_normalize_factor)*y_axis_stretch, 
		0.0 );
  }

  glEnd();


}


void 
MarxGL2Din3DSpaceGraph::resizeGL(int width, int height)
{
  glViewport(-1, -1, width, height);
}


// for testing... needs to be removed!!!
void 
MarxGL2Din3DSpaceGraph::keyPressEvent ( QKeyEvent * e )
{
  if (e->key() == Qt::Key_Y) {
    updateGL();
  }
}
