// graph based on hue

#ifndef MARXGLCOLORGRAPH_H
#define MARXGLCOLORGRAPH_H

#include <QGLWidget>
#include <QColor>

#include "realvec.h"

using namespace std;

class MarxGLColorGraph : public QGLWidget
{
  
public:

  MarxGLColorGraph( QWidget *parent, long b=0 );

  bool setBuffer( realvec& );
  void setBufferSize( long );

  void setContrast( float );

protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);

private:
  QColor bgcolor;

  realvec* buffer;
  long buffersize;

  float yzero;
  float contrast;

};

#endif
