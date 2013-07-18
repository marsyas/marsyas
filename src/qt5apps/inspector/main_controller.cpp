#include "main_controller.h"

#include "gui/marsystem_view.h"
#include "gui/controls_widget.h"
#include "gui/realvec_widget.h"

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

MarSystem *root_system;

QToolBar *toolbar;

QQuickView *system_view;
RealvecWidget *realvec_widget;
ControlsWidget *controls_widget;

Main::Main(Marsyas::MarSystem * system)
{
  root_system = system;

  //qRegisterMetaType<Marsyas::MarSystem*>();
  qmlRegisterType<MarSystemView>("Marsyas", 1, 0, "MarSystemView");
  qmlRegisterType<MarSystemViewAttached>();
  qmlRegisterType<MarSystemControlView>("Marsyas", 1, 0, "MarSystemControlView");

  QQmlEngine *engine = new QQmlEngine(this);

  //engine->rootContext()->setContextProperty("myModel", QVariant::fromValue<QObject*>(model));
  engine->rootContext()->setContextProperty("mySystem", QVariant::fromValue(system));

#if 0
  QQmlComponent component(&engine,
                          QString("main.qml"));

  if (component.status() != QQmlComponent::Ready) {
    qCritical("Not ready!");
    return 1;
  }

  QObject *root_view = component.create();
  if (!root_view)
    return 1;

  root_view->setProperty("model", QVariant::fromValue<QObject*>(model));
  //root_view->setProperty("index");
  //root_view->setProperty("text", QString::fromStdString(system->getPrefix()));

  QQuickWindow *window = new QQuickWindow();
  root_view->setParent( window->contentItem() );
  window->show();
#endif

  toolbar = new QToolBar();

  QAction *tick_action = toolbar->addAction("Tick");
  connect( tick_action, SIGNAL(triggered()), this, SLOT(tickSystem()) );

  ///////////////////

  system_view = new QQuickView(engine, 0);
  system_view->setColor( QApplication::palette().color(QPalette::Window) );
  system_view->setSource(QUrl::fromLocalFile("/home/jakob/programming/marsyas/src/qt5apps/inspector/main.qml"));
  system_view->setResizeMode(QQuickView::SizeRootObjectToView);

  QWidget *marsystem_widget = QWidget::createWindowContainer(system_view);
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

  QObject *system_item = system_view->rootObject();
  if (system_item) {
    QObject::connect( system_item, SIGNAL(clicked(QString)),
                      this, SLOT(systemClicked(QString)) );
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

  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }

  controls_widget->setSystem(system);
  realvec_widget->setSystem(system);
}
