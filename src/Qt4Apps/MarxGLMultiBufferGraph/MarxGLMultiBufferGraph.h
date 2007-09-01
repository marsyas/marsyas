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


#ifndef MARXGLMULTIBUFFERGRAPH_H
#define MARXGLMULTIBUFFERGRAPH_H

#include <QGLWidget>
#include <QColor>

#include "realvec.h"

class MarxGLMultiBufferGraph : public QGLWidget
{

public:
  MarxGLMultiBufferGraph( QWidget* parent=0, long n=0, int d=0 );

  bool setBuffer( Marsyas::realvec& );

  // the int corresponds to the enum of X, Y or Z
  void modelRotate( int, float );
  void modelTranslate( int, float );
  void projectionRotate( int, float );
  void projectionTranslate( int, float );

  float getXProjectionTranslation();
  float getYProjectionTranslation();
  float getZProjectionTranslation();
  float getXProjectionRotation();
  float getYProjectionRotation();
  float getZProjectionRotation();


  enum { X, Y, Z };

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);

private:
  int depth;
  long buffersize;

  Marsyas::realvec buffer;          // the current top buffer

  QColor bgcolor;

  float model_rotate_x;
  float model_rotate_y;
  float model_rotate_z;
  float model_translate_x;
  float model_translate_y;
  float model_translate_z;

  float projection_rotate_x;
  float projection_rotate_y;
  float projection_rotate_z;
  float projection_translate_x;
  float projection_translate_y;
  float projection_translate_z;

};

#endif
