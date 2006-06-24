// graph based on hue

#ifndef MARXGLCOLORGRAPH_H
#define MARXGLCOLORGRAPH_H

#include <QGLWidget>
#include <QColor>

#include "realvec.h"

class MarxGLColorGraph : public QGLWidget
{
  
public:

  MarxGLColorGraph( QWidget *parent, long b=0 );

  bool setBuffer( Marsyas::realvec& );
  void setBufferSize( long );

  void setContrast( float );

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);

private:
  QColor bgcolor;

  Marsyas::realvec* buffer;
  long buffersize;

  float yzero;
  float contrast;

};

#endif
