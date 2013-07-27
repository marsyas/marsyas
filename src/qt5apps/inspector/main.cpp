#include "main.h"
#include "graph/marsystem_adaptor.h"
#include "widgets/controls_widget.h"
#include "widgets/realvec_widget.h"

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
  MarSystemAdaptor *system_adaptor = new MarSystemAdaptor(system, this);

  QQmlEngine *engine = new QQmlEngine(this);
  engine->rootContext()->setContextProperty("system", QVariant::fromValue<QObject*>(system_adaptor));

  // Main window

  m_main_window = new QMainWindow;
  m_main_window->setWindowTitle("MarSystem Inspector");

  m_toolbar = new QToolBar();

  QAction *tick_action = m_toolbar->addAction("Tick");
  connect( tick_action, SIGNAL(triggered()), this, SLOT(tickSystem()) );

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

  m_main_window->setCentralWidget( graph_widget );

  QObject::connect( m_controls_widget, SIGNAL(controlClicked(QString)),
                    m_realvec_widget, SLOT(displayControl(QString)) );

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
  m_main_window->show();
}

void Main::addRealvecWidget()
{
  RealvecWidget * realvec_widget = new RealvecWidget;
  realvec_widget->setSystem(m_root_system);

  QDockWidget * dock_widget = new QDockWidget;
  dock_widget->setWidget(realvec_widget);
  dock_widget->setWindowTitle("Realvec Data");
  m_main_window->addDockWidget(Qt::RightDockWidgetArea, dock_widget);

  m_realvec_widget = realvec_widget;
}

void Main::tickSystem()
{
  qDebug() << "Main::tickSystem";
  m_root_system->tick();
  m_controls_widget->refresh();
  m_realvec_widget->refresh();
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
  m_realvec_widget->setSystem(system);
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
  m_realvec_widget->setSystem(system);
  m_realvec_widget->displayControl("mrs_realvec/processedData");
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
