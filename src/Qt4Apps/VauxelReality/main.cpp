
// export QMAKESPEC=/usr/local/Qt4.3/mkspecs/macx-g++/

#include <QApplication>

#include "VauxelRealityMainWindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  if ( !QGLFormat::hasOpenGL() )
  {
    qWarning( "This system has no OpenGL support. Exiting." );
    return -1;
  }

  VauxelRealityMainWindow vrmw;
  vrmw.show();

  return app.exec();
}

