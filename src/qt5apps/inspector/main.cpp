#include "main.h"
#include "debug_controller.h"
#include "graph/marsystem_adaptor.h"
#include "widgets/controls_widget.h"
#include "widgets/realvec_widget.h"
#include "widgets/stats_widget.h"

#include <marsyas/system/MarSystemManager.h>
#include <marsyas/json_io.h>

#include <QDebug>

#include <QApplication>
#include <QGuiApplication>

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QAction>

#include <QStringList>
#include <QPalette>
#include <QKeySequence>

#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>

#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickView>

#include <fstream>
#include <stdexcept>

using namespace std;
using namespace Marsyas;
using namespace MarsyasQml;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();

  QString filename;
  if (arguments.size() > 1)
    filename = arguments[1];

  Main *main = Main::instance();

  if (!filename.isEmpty())
    main->openSystem(filename);

  int result = app.exec();

  delete main;

  return result;
}

Main::Main():
  m_root_system(0)
{
  m_qml_engine = new QQmlEngine(this);

  m_debugger = new DebugController(this);

  // Main window

  m_main_window = new QMainWindow;
  m_main_window->setWindowTitle("MarSystem Inspector");
  m_main_window->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  createActions();
  createMenu();
  createToolbar();

  ///////////////////

  QPalette status_label_palette;
  status_label_palette.setColor(QPalette::Window, Qt::black);
  status_label_palette.setColor(QPalette::WindowText, Qt::white);

  m_reference_label = new QLabel(tr("Reference: -"));
  m_reference_label->setPalette(status_label_palette);
  m_reference_label->setAutoFillBackground(true);

  m_step_label = new QLabel(tr("Step: -"));
  m_step_label->setPalette(status_label_palette);
  m_step_label->setAutoFillBackground(true);

  m_graph = new QQuickView(m_qml_engine, 0);
  m_graph->setColor( QApplication::palette().color(QPalette::Window) );
  m_graph->setResizeMode(QQuickView::SizeRootObjectToView);

  // Central widget

  QWidget *graph_widget = QWidget::createWindowContainer(m_graph);
  graph_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  QHBoxLayout *status_layout = new QHBoxLayout;
  status_layout->setSpacing(2);
  status_layout->addWidget(m_step_label, 1);
  status_layout->addWidget(m_reference_label, 4);

  QVBoxLayout *central_layout = new QVBoxLayout;
  central_layout->setContentsMargins(0,0,0,0);
  central_layout->setSpacing(2);
  central_layout->addLayout(status_layout);
  central_layout->addWidget(graph_widget);

  QWidget *central_widget = new QWidget;
  central_widget->setLayout(central_layout);

  m_main_window->setCentralWidget( central_widget );

  // Dock widgets

  m_controls_widget = new ControlsWidget;

  QDockWidget *dock_controls_widget = new QDockWidget;
  dock_controls_widget->setWidget(m_controls_widget);
  dock_controls_widget->setWindowTitle("Control Data");
  m_main_window->addDockWidget(Qt::RightDockWidgetArea, dock_controls_widget);

  addRealvecWidget();

  m_stats_widget = new StatisticsWidget(&m_action_manager, m_debugger);
  QDockWidget *dock_debug_widget = new QDockWidget;
  dock_debug_widget->setWidget(m_stats_widget);
  dock_debug_widget->setWindowTitle("Statistics");
  m_main_window->addDockWidget(Qt::BottomDockWidgetArea, dock_debug_widget);

  connect( m_debugger, SIGNAL(ticked()),
           m_controls_widget, SLOT(refresh()) );
  connect( m_debugger, SIGNAL(ticked()),
           m_realvec_widget, SLOT(refresh()) );
  connect( m_debugger, SIGNAL(ticked()),
           this, SLOT(updateGraphBugs()) );
  connect( m_debugger, SIGNAL(recordingChanged(QString)),
           this, SLOT(onReferenceChanged(QString)) );
  connect( m_debugger, SIGNAL(tickCountChanged(int)),
           this, SLOT(onTickCountChanged(int)) );
  connect( m_controls_widget, SIGNAL(controlClicked(QString)),
           this, SLOT(controlClicked(QString)) );
  connect( m_stats_widget, SIGNAL(pathClicked(QString)),
           this, SLOT(bugClicked(QString)) );

  m_main_window->resize(1000, 600);
  m_main_window->showMaximized();
}

void Main::createActions()
{
  QAction *a;

  a = action(ActionManager::OpenSystem) = new QAction(tr("Open System..."), this);
  a->setShortcut(QKeySequence::Open);
  connect(a, SIGNAL(triggered()), this, SLOT(openSystem()));

  a = action(ActionManager::OpenRecording) = new QAction(tr("Open Reference..."), this);
  connect(a, SIGNAL(triggered()), this, SLOT(openRecording()));

  a = action(ActionManager::Tick) = new QAction(tr("Tick"), this);
  connect(a, SIGNAL(triggered()), m_debugger, SLOT(tick()));

  a = action(ActionManager::Rewind) = new QAction(tr("Rewind"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(rewind()));

  a = action(ActionManager::Quit) = new QAction(tr("Quit"), this);
  a->setShortcut(QKeySequence::Quit);
  connect(a, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Main::createMenu()
{
  QMenuBar *menuBar;
  QMenu *menu;

  // On Mac, create a parent-less menu bar to be shared by all windows:
#ifdef Q_OS_MAC
  menuBar = new QMenuBar(0);
#else
  menuBar = m_main_window->menuBar();
#endif

  menu = menuBar->addMenu(tr("&File"));
  menu->addAction(action(ActionManager::OpenSystem));
  menu->addAction(action(ActionManager::OpenRecording));
  menu->addSeparator();
  menu->addAction(action(ActionManager::Quit));
  menuBar->addMenu(menu);

  menu = menuBar->addMenu(tr("&Debug"));
  menu->addAction(action(ActionManager::Tick));
  menu->addAction(action(ActionManager::Rewind));
  menuBar->addMenu(menu);
}

void Main::createToolbar()
{
  QToolBar *toolbar = m_main_window->addToolBar("Actions");
  toolbar->addAction(action(ActionManager::OpenSystem));
  toolbar->addAction(action(ActionManager::OpenRecording));
  toolbar->addSeparator();
  toolbar->addAction(action(ActionManager::Tick));
  toolbar->addAction(action(ActionManager::Rewind));
}

void Main::openSystem()
{
  QString filename =
    QFileDialog::getOpenFileName(m_main_window,
                                 "Open MarSystem");
  if (filename.isEmpty())
    return;

  openSystem(filename);
}

void Main::openSystem(const QString & filename)
{
  if (filename.isEmpty())
    return;

  QString json_ending(".json");
  QString mpl_ending(".mpl");

  MarSystem *system = 0;

  if (filename.endsWith(mpl_ending))
  {
    ifstream plugin_stream( filename.toStdString().c_str() );
    system = m_system_manager.getMarSystem(plugin_stream);
  }
  else if (filename.endsWith(json_ending))
  {
    try {
      system = system_from_json_file(filename.toStdString());
    }
    catch (const std::runtime_error &e)
    {
      qCritical() << "Failed to parse JSON file:" << e.what();
    }
  }

  if (!system) {
    qCritical("Could not open MarSystem file!");
    return;
  }

  // Handle old state

  QObject *old_system_adaptor =
      m_graph->rootContext()->contextProperty("system").value<QObject*>();
  MarSystem *old_system = m_root_system;

  m_graph->setSource(QUrl());

  // Apply new state

  m_root_system = system;

  MarSystemAdaptor *system_adaptor = new MarSystemAdaptor(system, this);
  m_graph->rootContext()->setContextProperty("system", QVariant::fromValue<QObject*>(system_adaptor));
  m_graph->setSource(QUrl("qrc:///graph/Graph.qml"));

  QObject *root_item = m_graph->rootObject();
  if (root_item) {
    QObject::connect( root_item, SIGNAL(clicked(QString)),
                      this, SLOT(systemClicked(QString)) );
    QObject::connect( root_item, SIGNAL(inputClicked(QString)),
                      this, SLOT(systemInputClicked(QString)) );
    QObject::connect( root_item, SIGNAL(outputClicked(QString)),
                      this, SLOT(systemOutputClicked(QString)) );
  }
  else {
    qWarning("Could not find top system item!");
  }

  m_debugger->setSystem(system);
  m_stats_widget->setSystem(system);
  m_controls_widget->setSystem(system);
  m_realvec_widget->clear();
  m_system_filename = filename;

  delete old_system_adaptor;
  delete old_system;
}

void Main::openRecording()
{
  QString filename =
    QFileDialog::getOpenFileName(m_main_window,
                                 tr("Choose Reference Recording"));
  if (filename.isEmpty())
    return;

  m_debugger->setRecording(filename);
}

void Main::tick()
{
  m_debugger->tick();
}

void Main::rewind()
{
  m_debugger->rewind();
  openSystem( m_system_filename );
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

void Main::onReferenceChanged(const QString &filename)
{
  if (!filename.isEmpty())
    m_reference_label->setText(tr("Reference: %1").arg(filename));
  else
    m_reference_label->setText(tr("Reference: -"));
}

void Main::onTickCountChanged(int count)
{
  if (count)
    m_step_label->setText(tr("Step: %1").arg(count));
  else
    m_step_label->setText(tr("Step: -"));
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
  MarSystem *system = systemForPath(path);
  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }
  m_controls_widget->setSystem(system);
  m_realvec_widget->displayPort
      (QString::fromStdString(system->getAbsPath()), Input);
}

void Main::systemOutputClicked( const QString & path )
{
  MarSystem *system = systemForPath(path);
  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }
  m_controls_widget->setSystem(system);
  m_realvec_widget->displayPort
      (QString::fromStdString(system->getAbsPath()), Output);
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
  const Debug::BugReport & bugs = m_debugger->report();
  for (const auto & bug_mapping : bugs)
  {
    bug_paths << QString::fromStdString(bug_mapping.first);
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
       ( root_item, "navigateToItem", Qt::QueuedConnection,
         Q_ARG(QVariant, QVariant::fromValue(item)) );
  if (!ok) {
    qCritical() << "navigateToItem failed!";
    return;
  }
}

MarSystem *Main::systemForPath( const QString & path )
{
  if (!m_root_system)
    return 0;

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
