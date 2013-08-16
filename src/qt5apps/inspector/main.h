#ifndef INSPECTOR_MAIN_CONTROLLER_INCLUDED
#define INSPECTOR_MAIN_CONTROLLER_INCLUDED

#include <MarSystem.h>
#include <MarSystemManager.h>

#include <QObject>
#include <QToolBar>
#include <QQuickView>
#include <QMainWindow>
#include <QDockWidget>
#include <QLabel>

#include <QQmlEngine>

using namespace Marsyas;

class RealvecWidget;
class ControlsWidget;
class StatisticsWidget;
class DebugController;
class Main;

class ActionManager
{
public:
  enum Action {
    OpenSystem,
    OpenRecording,
    Tick,
    Rewind,
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

  static Main * instance()
  {
    static Main *instance = 0;
    if (!instance)
      instance = new Main;
    return instance;
  }

public slots:
  void openSystem();
  void openSystem(const QString & fileName);
  void openRecording();
  void tick();
  void rewind();

private:
  Main();
  void createActions();
  void createMenu();
  void createToolbar();

private slots:
  void addRealvecWidget();
  void onReferenceChanged(const QString &filename);
  void onTickCountChanged(int count);
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

  QString m_system_filename;
  Marsyas::MarSystemManager m_system_manager;
  MarSystem *m_root_system;

  QQmlEngine *m_qml_engine;

  DebugController *m_debugger;

  ActionManager m_action_manager;

  QMainWindow *m_main_window;
  QLabel *m_reference_label;
  QLabel *m_step_label;
  QQuickView *m_graph;
  RealvecWidget *m_realvec_widget;
  ControlsWidget *m_controls_widget;
  StatisticsWidget *m_stats_widget;
};

#endif // INSPECTOR_MAIN_CONTROLLER_INCLUDED
