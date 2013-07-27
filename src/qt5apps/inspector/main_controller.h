#ifndef INSPECTOR_MAIN_CONTROLLER_INCLUDED
#define INSPECTOR_MAIN_CONTROLLER_INCLUDED

#include <MarSystem.h>

#include <QObject>

class Main : public QObject
{
  Q_OBJECT

public:

  static Main * create(Marsyas::MarSystem * system) {
    static Main *instance = 0;
    if (instance)
      return 0;
    instance = new Main(system);
    return instance;
  }

private:
  Main(Marsyas::MarSystem * system);

private slots:
  void tickSystem();
  void systemClicked( const QString & path );
  void systemInputClicked( const QString & path );
  void systemOutputClicked( const QString & path );

private:
  Marsyas::MarSystem *systemForPath( const QString & path );
};

#endif // INSPECTOR_MAIN_CONTROLLER_INCLUDED
