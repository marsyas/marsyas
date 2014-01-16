#ifndef INSPECTOR_MAIN_CONTROLLER_INCLUDED
#define INSPECTOR_MAIN_CONTROLLER_INCLUDED

#include <marsyas/system/MarSystem.h>
#include <marsyas/system/MarSystemManager.h>

#include <QObject>
#include <QToolBar>
#include <QQuickView>
#include <QMainWindow>
#include <QDockWidget>
#include <QLabel>
#include <QSpinBox>
#include <QPointer>

#include <QQmlEngine>

using namespace Marsyas;

class RealvecWidget;
class ControlsWidget;
class StatisticsWidget;
class FilePathLabel;
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

    AddRealvecWidget,

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

signals:
  void systemChanged();
  void fileChanged( const QString & filePath );

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
  FilePathLabel *m_system_label;
  FilePathLabel *m_reference_label;
  QLabel *m_step_label;
  QSpinBox *m_step_control;
  QQuickView *m_graph;
  ControlsWidget *m_controls_widget;
  StatisticsWidget *m_stats_widget;
  QPointer<RealvecWidget> m_current_signal_widget;

  QDockWidget *m_dock_stats_widget;
};

class SignalDockWidget : public QDockWidget
{
  Q_OBJECT

private:
  RealvecWidget *m_signal_widget;

public:
  SignalDockWidget(DebugController* debugger);

  RealvecWidget * widget() { return m_signal_widget; }

  virtual void mousePressEvent(QMouseEvent *);
  virtual void closeEvent(QCloseEvent *);

signals:
  void clicked(RealvecWidget*);
};

class StatusLabel : public QLabel
{
  Q_OBJECT

public:
  StatusLabel( QWidget * parent = 0 );
  StatusLabel( QString label, QString text = QString(), QWidget * parent = 0 );

public slots:
  void setLabel( const QString & label )
  {
    m_label = label;
    updateFullText();
  }

  void setText( const QString & text )
  {
    m_text = text;
    updateFullText();
  }

private:
  void updateFullText()
  {
    QString full_text;
    if (!m_label.isEmpty())
    {
      full_text += m_label;
      full_text += ": ";
    }
    full_text += m_text;
    QLabel::setText(full_text);
  }

  QString m_label;
  QString m_text;
};

class FilePathLabel : public StatusLabel
{
  Q_OBJECT

public:
  FilePathLabel( QWidget * parent = 0 );
  FilePathLabel( QString label, QString filePath = QString(), QWidget * parent = 0 );

public slots:
  void setFileName( const QString & filePath );
};

#endif // INSPECTOR_MAIN_CONTROLLER_INCLUDED
