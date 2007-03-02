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


#ifndef MARXGL2DIN3DSPACEGRAPH_H 
#define MARXGL2DIN3DSPACEGRAPH_H

#include <QGLWidget>
#include <QColor>
#include <QKeyEvent> // for testing only!!!

#include "realvec.h"

class MarxGL2Din3DSpaceGraph : public QGLWidget
{
  
public:

  MarxGL2Din3DSpaceGraph( QWidget *parent, long b=0 );

  bool setBuffer( Marsyas::realvec& );
  void setXAxisStretch( float );
  void setYAxisStretch( float );
  bool setYNormalizeFactor( float );
  void setXTranslation( float );
  void setYTranslation( float );
  void setZTranslation( float );
  void setYRotation( float );  // rotation in degrees around y axis

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);

  void keyPressEvent ( QKeyEvent * e );  // for testing

private:
  QColor bgcolor;

  Marsyas::realvec* buffer;
  long buffersize;

  float x_axis_stretch;
  float y_axis_stretch; 

  float y_normalize_factor;
  float x_translation;
  float y_translation;
  float z_translation;
  float y_rotation;

};

#endif
