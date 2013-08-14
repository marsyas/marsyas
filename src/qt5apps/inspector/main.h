#ifndef INSPECTOR_MAIN_CONTROLLER_INCLUDED
#define INSPECTOR_MAIN_CONTROLLER_INCLUDED

#include <MarSystem.h>

#include <QObject>
#include <QToolBar>
#include <QQuickView>
#include <QMainWindow>
#include <QDockWidget>

using namespace Marsyas;

class RealvecWidget;
class ControlsWidget;
class DebugWidget;
class DebugController;

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
  void addRealvecWidget();
  void systemClicked( const QString & path );
  void systemInputClicked( const QString & path );
  void systemOutputClicked( const QString & path );
  void controlClicked( const QString & path );
  void bugClicked( const QString & path );
  void updateGraphBugs();

private:
  MarSystem *systemForPath( const QString & path );

  MarSystem *m_root_system;

  DebugController *m_debugger;

  QMainWindow *m_main_window;
  QToolBar *m_toolbar;
  QQuickView *m_graph;
  RealvecWidget *m_realvec_widget;
  ControlsWidget *m_controls_widget;
  DebugWidget *m_debug_widget;
};

#endif // INSPECTOR_MAIN_CONTROLLER_INCLUDED
