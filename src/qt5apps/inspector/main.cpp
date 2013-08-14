#include "main.h"
#include "debug_controller.h"
#include "graph/marsystem_adaptor.h"
#include "widgets/controls_widget.h"
#include "widgets/realvec_widget.h"
#include "widgets/debug_widget.h"

#include <MarSystemManager.h>

#include <QDebug>

#include <QApplication>
#include <QGuiApplication>

#include <QStringList>
#include <QShortcut>
#include <QAction>
#include <QPalette>

#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickView>

#include <fstream>

using namespace std;
using namespace Marsyas;
using namespace MarsyasQml;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  if (arguments.size() < 2) {
    qWarning("Missing argument: marsyas plugin file.");
    return 1;
  }

  QString plugin_filename = arguments[1];
  ifstream plugin_stream( plugin_filename.toStdString().c_str() );
  MarSystemManager mng;
  MarSystem *system = mng.getMarSystem(plugin_stream);
  if (!system) {
    qCritical("Could not load plugin file!");
    return 1;
  }

  Main::create(system);

  return app.exec();
}


Main::Main(Marsyas::MarSystem * system):
  m_root_system(system)
{
  m_debugger = new DebugController(this);
  m_debugger->setSystem(system);

  // Qml stuff

  MarSystemAdaptor *system_adaptor = new MarSystemAdaptor(system, this);

  QQmlEngine *engine = new QQmlEngine(this);
  engine->rootContext()->setContextProperty("system", QVariant::fromValue<QObject*>(system_adaptor));

  // Main window

  m_main_window = new QMainWindow;
  m_main_window->setWindowTitle("MarSystem Inspector");
  m_main_window->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  m_toolbar = new QToolBar();

  QAction *tick_action = m_toolbar->addAction("Tick");
  connect( tick_action, SIGNAL(triggered()), m_debugger, SLOT(tick()) );

  m_main_window->addToolBar(Qt::TopToolBarArea, m_toolbar);

  ///////////////////

  m_graph = new QQuickView(engine, 0);
  m_graph->setColor( QApplication::palette().color(QPalette::Window) );
  m_graph->setSource(QUrl("qrc:///graph/Graph.qml"));
  m_graph->setResizeMode(QQuickView::SizeRootObjectToView);

  QWidget *graph_widget = QWidget::createWindowContainer(m_graph);
  graph_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  m_controls_widget = new ControlsWidget;
  m_controls_widget->setSystem(system);

  QDockWidget *dock_controls_widget = new QDockWidget;
  dock_controls_widget->setWidget(m_controls_widget);
  dock_controls_widget->setWindowTitle("Control Data");
  m_main_window->addDockWidget(Qt::RightDockWidgetArea, dock_controls_widget);

  addRealvecWidget();

  m_debug_widget = new DebugWidget(m_debugger);
  QDockWidget *dock_debug_widget = new QDockWidget;
  dock_debug_widget->setWidget(m_debug_widget);
  dock_debug_widget->setWindowTitle("Debug");
  m_main_window->addDockWidget(Qt::BottomDockWidgetArea, dock_debug_widget);

  m_main_window->setCentralWidget( graph_widget );

  connect( m_debugger, SIGNAL(ticked()),
           m_controls_widget, SLOT(refresh()) );
  connect( m_debugger, SIGNAL(ticked()),
           m_realvec_widget, SLOT(refresh()) );
  connect( m_debugger, SIGNAL(ticked()),
           this, SLOT(updateGraphBugs()) );
  connect( m_controls_widget, SIGNAL(controlClicked(QString)),
           this, SLOT(controlClicked(QString)) );
  connect( m_debug_widget, SIGNAL(pathClicked(QString)),
           this, SLOT(bugClicked(QString)) );

  QObject *system_item = m_graph->rootObject();
  if (system_item) {
    QObject::connect( system_item, SIGNAL(clicked(QString)),
                      this, SLOT(systemClicked(QString)) );
    QObject::connect( system_item, SIGNAL(inputClicked(QString)),
                      this, SLOT(systemInputClicked(QString)) );
    QObject::connect( system_item, SIGNAL(outputClicked(QString)),
                      this, SLOT(systemOutputClicked(QString)) );
  }
  else {
    qWarning("Could not find top system item!");
  }

  QShortcut *quit_shortcut = new QShortcut(QString("Ctrl+Q"), m_main_window);
  QObject::connect( quit_shortcut, SIGNAL(activated()), qApp, SLOT(quit()) );

  m_main_window->resize(1000, 600);
  m_main_window->showMaximized();
}

void Main::addRealvecWidget()
{
  RealvecWidget * realvec_widget = new RealvecWidget(m_debugger);

  QDockWidget * dock_widget = new QDockWidget;
  dock_widget->setWidget(realvec_widget);
  dock_widget->setWindowTitle("Realvec Data");
  m_main_window->addDockWidget(Qt::RightDockWidgetArea, dock_widget);

  m_realvec_widget = realvec_widget;
}

void Main::systemClicked( const QString & path )
{
  //qDebug() << "Main: System clicked:" << path;

  MarSystem *system = systemForPath(path);
  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }

  m_controls_widget->setSystem(system);
  m_realvec_widget->clear();
}

void Main::systemInputClicked( const QString & path )
{
  (void) path;
}

void Main::systemOutputClicked( const QString & path )
{
  MarSystem *system = systemForPath(path);
  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }
  m_controls_widget->setSystem(system);
  m_realvec_widget->displayDebugValue(QString::fromStdString(system->getAbsPath()));
}

void Main::controlClicked( const QString & path )
{
  MarSystem *system = m_controls_widget->system();
  m_realvec_widget->displayControl(system, path);
}

void Main::updateGraphBugs()
{
  QObject *root_item = m_graph->rootObject();
  if (!root_item)
    return;

  QStringList bug_paths;
  const debugger::report * bugs = m_debugger->bugReport();
  if (bugs)
  {
    for (const auto & bug_mapping : *bugs)
    {
      bug_paths << QString::fromStdString(bug_mapping.first);
    }
  }

  root_item->setProperty("bugs", QVariant::fromValue(bug_paths));
}

void Main::bugClicked( const QString & path )
{
  QObject *root_item = m_graph->rootObject();
  if (!root_item)
    return;

  QQmlProperty property(root_item, "systemViews");
  QVariant prop_var = property.read();
  QVariantMap prop_map = prop_var.value<QVariantMap>();

  QVariant item_var = prop_map[path];
  QObject *item = item_var.value<QObject*>();
  if (!item)
  {
    qCritical() << "no item for path";
    return;
  }

  bool ok;

  ok = item->metaObject()->invokeMethod(item, "setExpanded", Q_ARG(QVariant, true));
  if (!ok) {
    qCritical() << "setExpanded failed!";
    return;
  }

  ok = root_item->metaObject()->invokeMethod
       ( root_item, "navigateToItem",
         Q_ARG(QVariant, QVariant::fromValue(item)) );
  if (!ok) {
    qCritical() << "navigateToItem failed!";
    return;
  }
}

MarSystem *Main::systemForPath( const QString & path )
{
  QString relative_path;

  int separator_index;
  separator_index = path.indexOf('/', 1);
  if (separator_index != -1)
    separator_index = path.indexOf('/', separator_index + 1);
  if (separator_index != -1)
    relative_path = path.mid(separator_index+1);

  if (relative_path.size() && relative_path[relative_path.size()-1] == '/')
    relative_path.chop(1);

  //qDebug() << "relative path:" << relative_path;

  MarSystem *system;
  if (relative_path.isEmpty())
    system = m_root_system;
  else
    system = m_root_system->getChildMarSystem( relative_path.toStdString() );

  return system;
}
