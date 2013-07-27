#ifndef INSPECTOR_MAIN_CONTROLLER_INCLUDED
#define INSPECTOR_MAIN_CONTROLLER_INCLUDED

#include <MarSystem.h>

#include <QObject>
#include <QToolBar>
#include <QQuickView>

using namespace Marsyas;

class RealvecWidget;
class ControlsWidget;

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
  MarSystem *systemForPath( const QString & path );

  MarSystem *m_root_system;

  QToolBar *m_toolbar;
  QQuickView *m_graph;
  RealvecWidget *m_realvec_widget;
  ControlsWidget *m_controls_widget;
};

#endif // INSPECTOR_MAIN_CONTROLLER_INCLUDED
