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
class Main;

class ActionManager
{
public:
  enum Action {
    OpenSystem,
    OpenRecording,
    Tick,
    Quit,

    ActionCount
  };

  QAction * action(Action type) { return m_actions[type]; }

private:
  friend class Main;
  QAction *m_actions[ActionCount];
};

class Main : public QObject
{
  Q_OBJECT

public:
  enum Action {
    OpenSystem,
    OpenRecording,
    Tick,
    Quit,

    ActionCount
  };

  static Main * instance(Marsyas::MarSystem * system)
  {
    static Main *instance = 0;
    if (!instance)
      instance = new Main(system);
    return instance;
  }

public slots:
  void openSystem();
  void openRecording();

private:
  Main(Marsyas::MarSystem * system);
  void createActions();
  void createMenu();

private slots:
  void addRealvecWidget();
  void updateGraphBugs();

  void systemClicked( const QString & path );
  void systemInputClicked( const QString & path );
  void systemOutputClicked( const QString & path );
  void controlClicked( const QString & path );
  void bugClicked( const QString & path );

private:
  MarSystem *systemForPath( const QString & path );

  QAction *& action(ActionManager::Action type)
  {
    return m_action_manager.m_actions[type];
  }

  MarSystem *m_root_system;

  DebugController *m_debugger;

  ActionManager m_action_manager;

  QMainWindow *m_main_window;
  QToolBar *m_toolbar;
  QQuickView *m_graph;
  RealvecWidget *m_realvec_widget;
  ControlsWidget *m_controls_widget;
  DebugWidget *m_debug_widget;
};

#endif // INSPECTOR_MAIN_CONTROLLER_INCLUDED
