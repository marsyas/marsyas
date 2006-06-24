
#include "MarxGLColorGraph.h"

using namespace std;
using namespace Marsyas;


MarxGLColorGraph::MarxGLColorGraph(QWidget *parent, long b)
  : QGLWidget(parent)
{
  bgcolor = QColor::fromRgbF(0.0, 0.0, 0.0);
  buffersize = b;
  buffer = new realvec( buffersize );

  yzero = 1.0;
  contrast = 2.0;

}


void 
MarxGLColorGraph::setBufferSize( long b )
{
  buffersize = b;
}


void 
MarxGLColorGraph::setContrast( float c )
{
  contrast = c;
  updateGL();
}

void 
MarxGLColorGraph::initializeGL()
{
  qglClearColor( bgcolor );
}


void 
MarxGLColorGraph::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(0.0, 0.0, 0.0);

  glBegin( GL_QUADS );

  for (int i=0; i<buffer->getSize()-1; i++ ) {

    float blue = ((float)((*buffer)(i))+yzero)/contrast*yzero;
    if (blue > 1.0) { blue = 1.0; }

    float green, red;
    green = red = blue;

    glColor3f( red, green, blue );

    glVertex2f( ((float)i*2.0/(float)buffersize) - 1.0, 1.0 );
    glVertex2f( (((float)i+1)*2.0/(float)buffersize) - 1.0, 1.0 );
    glVertex2f( (((float)i+1)*2.0/(float)buffersize) - 1.0, -1.0 );
    glVertex2f( ((float)i*2.0/(float)buffersize) - 1.0, -1.0 );

//     glVertex2f( ((float)i*2.0/(float)buffersize) - 1.0, val1/yzero );
//     glVertex2f( (((float)i+1)*2.0/(float)buffersize) - 1.0, val2/yzero );
  }

  glEnd();

}


void 
MarxGLColorGraph::resizeGL(int width, int height)
{
  glViewport(-1, -1, width+5, height+5);
}




bool 
MarxGLColorGraph::setBuffer( realvec& rv )
{
  if ( rv.getSize() == buffersize ) { 
     *buffer = rv;

      updateGL();

    return true;
  }
  return false;
}

