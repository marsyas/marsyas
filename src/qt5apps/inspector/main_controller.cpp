#include "main_controller.h"

#include "graph/marsystem_adaptor.h"
#include "widgets/controls_widget.h"
#include "widgets/realvec_widget.h"

#include <MarSystem.h>

#include <QApplication>

#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>
#include <QPalette>

#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QToolBar>
#include <QVBoxLayout>

#include <QShortcut>
#include <QAction>

using namespace Marsyas;
using namespace MarsyasQml;

MarSystem *root_system;

QToolBar *toolbar;

QQuickView *system_graph;
RealvecWidget *realvec_widget;
ControlsWidget *controls_widget;

Main::Main(Marsyas::MarSystem * system)
{
  root_system = system;

  MarSystemAdaptor *system_adaptor = new MarSystemAdaptor(system, this);

  QQmlEngine *engine = new QQmlEngine(this);
  engine->rootContext()->setContextProperty("system", QVariant::fromValue<QObject*>(system_adaptor));

  toolbar = new QToolBar();

  QAction *tick_action = toolbar->addAction("Tick");
  connect( tick_action, SIGNAL(triggered()), this, SLOT(tickSystem()) );

  ///////////////////

  system_graph = new QQuickView(engine, 0);
  system_graph->setColor( QApplication::palette().color(QPalette::Window) );
  system_graph->setSource(QUrl::fromLocalFile("/home/jakob/programming/marsyas/src/qt5apps/inspector/graph/qml/Graph.qml"));
  system_graph->setResizeMode(QQuickView::SizeRootObjectToView);

  QWidget *marsystem_widget = QWidget::createWindowContainer(system_graph);
  marsystem_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  controls_widget = new ControlsWidget;
  controls_widget->setSystem(system);
  //controls_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

  realvec_widget = new RealvecWidget;
  realvec_widget->setSystem(system);
  //realvec_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );

  QObject::connect( controls_widget, SIGNAL(controlClicked(QString)),
                    realvec_widget, SLOT(displayControl(QString)) );

  QSplitter *data_splitter = new QSplitter();
  data_splitter->setOrientation( Qt::Vertical );
  data_splitter->addWidget( controls_widget );
  data_splitter->addWidget( realvec_widget );

  QSplitter *splitter = new QSplitter();
  splitter->addWidget( marsystem_widget );
  splitter->addWidget( data_splitter );

  QVBoxLayout *column = new QVBoxLayout();
  column->addWidget(toolbar);
  column->addWidget(splitter);

  QWidget *window = new QWidget();
  window->setLayout(column);

  window->resize(1000, 600);
  window->show();
  //window->showMaximized();

  QObject *system_item = system_graph->rootObject();
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

  QShortcut *quit_shortcut = new QShortcut(QString("Ctrl+Q"), splitter);
  QObject::connect( quit_shortcut, SIGNAL(activated()), qApp, SLOT(quit()) );
}

void Main::tickSystem()
{
  qDebug() << "Main::tickSystem";
  root_system->tick();
  controls_widget->refresh();
  realvec_widget->refresh();
}

void Main::systemClicked( const QString & path )
{
  //qDebug() << "Main: System clicked:" << path;

  MarSystem *system = systemForPath(path);
  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }

  controls_widget->setSystem(system);
  realvec_widget->setSystem(system);
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
  controls_widget->setSystem(system);
  realvec_widget->setSystem(system);
  realvec_widget->displayControl("mrs_realvec/processedData");
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
    system = root_system;
  else
    system = root_system->getChildMarSystem( relative_path.toStdString() );

  return system;
}
