
// export QMAKESPEC=/usr/local/Qt4.3/mkspecs/macx-g++/

#include <QApplication>
#include "PeakVisMainWindow.h"

int 
main(int argc, char *argv[])
{
  QApplication app(argc, argv);
   
   if ( !QGLFormat::hasOpenGL() )
   {
      qWarning( "This system has no OpenGL support. Exiting." );
      return -1;
   }   
   
  PeakVisMainWindow pvmw;
   
#if QT_VERSION < 0x040000
   app.setMainWidget(&pvmw);
#else
   //pvmw.setWindowTitle("PeakVis");
#endif   
      
  //pvmw.resize(1024,768);
  pvmw.show();
  return app.exec();
}

