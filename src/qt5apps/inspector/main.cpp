#include "main.h"
#include "debug_controller.h"
#include "graph/marsystem_adaptor.h"
#include "graph/connection_item.h"
#include "widgets/controls_widget.h"
#include "widgets/realvec_widget.h"
#include "widgets/stats_widget.h"

#include <marsyas/system/MarSystemManager.h>

#ifdef MARSYAS_HAS_SCRIPT
# include <marsyas/script/script.h>
#endif

#ifdef MARSYAS_HAS_JSON
# include <marsyas/json_io.h>
#endif

#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QGuiApplication>

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QAction>

#include <QStringList>
#include <QPalette>
#include <QKeySequence>
#include <QTextCursor>

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

Main *Main::m_instance = nullptr;

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();

  QString filename;
  if (arguments.size() > 1)
    filename = arguments[1];

  Main *main = new Main();

  if (!filename.isEmpty())
    main->openSystem(filename);

  int result = app.exec();

  delete main;

  return result;
}


SignalDockWidget::SignalDockWidget(DebugController* debugger):
  m_signal_widget(new RealvecWidget(debugger))
{
  setWidget(m_signal_widget);

  setWindowTitle("Empty Signal View");

  connect( debugger, SIGNAL(ticked()),
           m_signal_widget, SLOT(refresh()) );

  connect( m_signal_widget, &RealvecWidget::pathChanged,
           [this](const QString & title)

  {
    if (!title.isEmpty())
      setWindowTitle(title);
    else
      setWindowTitle("Empty Signal View");
  });
}

void SignalDockWidget::mousePressEvent(QMouseEvent *)
{
  emit clicked(m_signal_widget);
}

void SignalDockWidget::closeEvent(QCloseEvent *event)
{
  event->ignore();
  deleteLater();
}


Main::Main():
  m_root_system(0)
{
  qmlRegisterType<ConnectionItem>("Marsyas", 1, 0, "ConnectionLine");

  m_qml_engine = new QQmlEngine(this);

  m_debugger = new DebugController(this);

  // Main window

  m_main_window = new QMainWindow;
  m_main_window->setWindowTitle("MarSystem Inspector");
  m_main_window->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  createActions();

  ///////////////////

  QPalette status_label_palette;
  status_label_palette.setColor(QPalette::Window, Qt::black);
  status_label_palette.setColor(QPalette::WindowText, Qt::white);

  m_system_label = new FilePathLabel("System");
  m_reference_label = new FilePathLabel("Reference");
  m_step_label = new StatusLabel("Step");

  m_graph = new QQuickView(m_qml_engine, 0);
  m_graph->setResizeMode(QQuickView::SizeRootObjectToView);

  createGraphStyles();
  setCurrentGraphStyle("black-on-white");

  // Central widget

  QWidget *graph_widget = QWidget::createWindowContainer(m_graph);
  graph_widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  QHBoxLayout *status_layout = new QHBoxLayout;
  status_layout->setSpacing(2);
  status_layout->addWidget(m_system_label, 4);
  status_layout->addWidget(m_reference_label, 4);
  status_layout->addWidget(m_step_label, 1);

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
  connect(m_controls_widget, SIGNAL(pathChanged(QString)),
          dock_controls_widget, SLOT(setWindowTitle(QString)) );

  addRealvecWidget();

  m_dock_msg_widget = new MessageDockWidget;
  m_main_window->addDockWidget(Qt::BottomDockWidgetArea, m_dock_msg_widget);

  m_stats_widget = new StatisticsWidget(&m_action_manager, m_debugger);
  m_dock_stats_widget = new QDockWidget;
  m_dock_stats_widget->setWidget(m_stats_widget);
  m_dock_stats_widget->setWindowTitle("Statistics");
  m_main_window->addDockWidget(Qt::BottomDockWidgetArea, m_dock_stats_widget);
  m_main_window->tabifyDockWidget(m_dock_msg_widget, m_dock_stats_widget);

  m_dock_msg_widget->raise();

  m_dock_msg_widget->hide();
  m_dock_stats_widget->hide();

  connect( this, SIGNAL(fileChanged(QString)), m_system_label,
           SLOT(setFileName(QString)) );
  connect( m_debugger, SIGNAL(ticked()),
           m_controls_widget, SLOT(refresh()) );
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

  createMenu();
  createToolbar();

  m_main_window->resize(1000, 600);
  m_main_window->showMaximized();

  m_instance = this;

  MrsLog::setMessageFunction( &Main::reportMessage );
  MrsLog::setWarningFunction( &Main::reportWarning );
  MrsLog::setErrorFunction( &Main::reportError );
  MrsLog::setDiagnosticFunction( &Main::reportMessage );
  MrsLog::setDebugFunction( &Main::reportMessage );

  qInstallMessageHandler( &Main::qtMessageHandler );
}

Main::~Main()
{
  // Must delete GUI explicitly, else can deadlock
  // in ~QApplication, while ongoing QML animation.
  // See GitHub issue #25.
  delete m_main_window;
  delete m_root_system;
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
  connect(a, SIGNAL(triggered()), this, SLOT(tick()));

  a = action(ActionManager::Rewind) = new QAction(tr("Rewind"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(rewind()));

  a = action(ActionManager::Quit) = new QAction(tr("Quit"), this);
  a->setShortcut(QKeySequence::Quit);
  connect(a, SIGNAL(triggered()), qApp, SLOT(quit()));

  a = action(ActionManager::AddRealvecWidget) = new QAction(tr("Add Signal View"), this);
  connect(a, SIGNAL(triggered()), this, SLOT(addRealvecWidget()));
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

  menu = menuBar->addMenu(tr("&View"));
  menu->addAction(m_dock_msg_widget->toggleViewAction());
  menu->addAction(m_dock_stats_widget->toggleViewAction());
  menu->addAction(action(ActionManager::AddRealvecWidget));
  menuBar->addMenu(menu);
}

void Main::createToolbar()
{
  m_step_control = new QSpinBox();
  m_step_control->setRange(1, 1000);
  m_step_control->setValue(1);

  QToolBar *toolbar = m_main_window->addToolBar("Actions");

  toolbar->addAction(action(ActionManager::OpenSystem));
  toolbar->addAction(action(ActionManager::OpenRecording));

  toolbar->addSeparator();

  toolbar->addWidget(m_step_control);
  toolbar->addAction(action(ActionManager::Tick));
  toolbar->addAction(action(ActionManager::Rewind));
}

void Main::createGraphStyles()
{
  QQmlPropertyMap *black_on_white = m_graph_styles["black-on-white"] = new QQmlPropertyMap(this);
  black_on_white->insert("background", QColor("white"));
  black_on_white->insert("node_border", QColor("black"));
  black_on_white->insert("node_text", QColor("black"));
  black_on_white->insert("port_background", QColor(220, 220, 220));
  black_on_white->insert("port_text", QColor(120, 120, 120));
  black_on_white->insert("connection", QColor("blue"));
  black_on_white->insert("expand_icon", QColor(0, 0, 0));
  black_on_white->insert("expand_icon_background", QColor(200, 200, 200));
  black_on_white->insert("selection", QColor("blue"));
  black_on_white->insert("selection_background", QColor(180,180,230));

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

  if (filename.endsWith(".mrs"))
  {
#ifdef MARSYAS_HAS_SCRIPT
    system = system_from_script(filename.toStdString());
#endif
  }
  else if (filename.endsWith(mpl_ending))
  {
    ifstream plugin_stream( filename.toStdString().c_str() );
    system = m_system_manager.getMarSystem(plugin_stream);
  }
  else if (filename.endsWith(json_ending))
  {
#ifdef MARSYAS_HAS_JSON
    try {
      system = system_from_json_file(filename.toStdString());
    }
    catch (const std::runtime_error &e)
    {
      qCritical() << "Failed to parse JSON file:" << e.what();
    }
#endif
  }

  if (!system) {
    qCritical("Could not open MarSystem file!");
  }

  // Handle old state

  QQmlContext *graph_context = m_graph->rootContext();

  QObject *old_system_adaptor = graph_context->contextProperty("system").value<QObject*>();
  MarSystem *old_system = m_root_system;

  m_graph->setSource(QUrl());
  graph_context->setContextProperty("system", QVariant());

  // Apply new state

  m_root_system = system;

  if (system)
  {
    MarSystemAdaptor *system_adaptor = new MarSystemAdaptor(system, this);
    graph_context->setContextProperty("system", static_cast<QObject*>(system_adaptor));
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
  }

  m_debugger->setSystem(system);
  m_stats_widget->setSystem(system);
  m_controls_widget->setSystem(system);
  m_system_filename = filename;

  delete old_system_adaptor;
  delete old_system;

  emit systemChanged();
  emit fileChanged( filename );
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
  m_debugger->tick( m_step_control->value() );
}

void Main::rewind()
{
  m_debugger->rewind();
  openSystem( m_system_filename );
}

void Main::addRealvecWidget()
{
  SignalDockWidget * dock_widget = new SignalDockWidget(m_debugger);

  connect(dock_widget, &SignalDockWidget::clicked,
          [this](RealvecWidget* widget){ m_current_signal_widget = widget; });

  connect(this, SIGNAL(systemChanged()),
          dock_widget->widget(), SLOT(clear()));

  m_main_window->addDockWidget(Qt::RightDockWidgetArea, dock_widget);

  m_current_signal_widget = dock_widget->widget();
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
}

void Main::systemInputClicked( const QString & path )
{
  MarSystem *system = systemForPath(path);
  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }

  if (m_current_signal_widget)
    m_current_signal_widget->displayPort(system, Input);
}

void Main::systemOutputClicked( const QString & path )
{
  MarSystem *system = systemForPath(path);
  if (!system) {
    qWarning() << "Main: System not found for path:" << path;
    return;
  }

  if (m_current_signal_widget)
    m_current_signal_widget->displayPort(system, Output);
}

void Main::controlClicked( const QString & path )
{
  MarSystem *system = m_controls_widget->system();

  if (m_current_signal_widget)
    m_current_signal_widget->displayControl(system, path);
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

void Main::setCurrentGraphStyle(const QString & name)
{
  m_graph_style = name;
  m_graph->setColor(currentGraphStyle()->value("background").value<QColor>());
  m_graph->rootContext()->setContextProperty("global_style", currentGraphStyle());
}

StatusLabel::StatusLabel( QWidget * parent ):
  QLabel(parent)
{
  QPalette palette;
  palette.setColor(QPalette::Window, Qt::black);
  palette.setColor(QPalette::WindowText, Qt::white);
  setPalette(palette);

  setAutoFillBackground(true);
  setMargin(1);

  updateFullText();
}

StatusLabel::StatusLabel( QString label, QString text, QWidget * parent ):
  QLabel(parent),
  m_label(label),
  m_text(text)
{
  QPalette palette;
  palette.setColor(QPalette::Window, Qt::black);
  palette.setColor(QPalette::WindowText, Qt::white);
  setPalette(palette);

  setAutoFillBackground(true);
  setMargin(1);

  updateFullText();
}

FilePathLabel::FilePathLabel( QWidget * parent ):
  StatusLabel(parent)
{
  setFileName(QString());
}

FilePathLabel::FilePathLabel( QString label, QString filePath, QWidget * parent ):
  StatusLabel(parent)
{
    setLabel(label);
    setFileName(filePath);
}

void FilePathLabel::setFileName( const QString & filePath )
{
  if (filePath.isEmpty())
  {
    setText("-");
    return;
  }

  QFileInfo info(filePath);
  QString dir_path = info.absolutePath();
  QString file_name = info.fileName();

  QString text = file_name;
  if (!dir_path.isEmpty())
    text += QString(" (%1)").arg(dir_path);

  setText(text);
}

class MessageWidget : public QPlainTextEdit
{
protected:
  virtual void contextMenuEvent(QContextMenuEvent *event)
  {
      QMenu *menu = createStandardContextMenu();

      QAction *a = menu->addAction(tr("Clear"));
      connect(a, SIGNAL(triggered()), this, SLOT(clear()));

      menu->exec(event->globalPos());
      delete menu;
  }
};

MessageDockWidget::MessageDockWidget()
{
  m_text_view = new MessageWidget;
  m_text_view->setMaximumBlockCount(1000);
  m_text_view->setReadOnly(true);

  setWidget(m_text_view);

  setWindowTitle("Messages");
}

void MessageDockWidget::message(const QString & msg)
{
  m_text_view->appendPlainText(msg);
}

void MessageDockWidget::warning(const QString &msg)
{
  m_text_view->appendPlainText(QString("WARNING: ") + msg);
}

void MessageDockWidget::error(const QString &msg)
{
  m_text_view->appendPlainText(QString("PROBLEM: ") + msg);
}

void MessageDockWidget::append(const QString & text)
{
  QTextCursor cursor = m_text_view->textCursor();
  cursor.movePosition(QTextCursor::End);
  cursor.insertText(text);
}

void Main::reportMessage( const std::string & msg )
{
  instance()->messageWidget()->message(QString::fromUtf8(msg.c_str()));
  instance()->messageWidget()->show();
  instance()->messageWidget()->raise();
}

void Main::reportWarning( const std::string & msg )
{
  instance()->messageWidget()->warning(QString::fromUtf8(msg.c_str()));
  instance()->messageWidget()->show();
  instance()->messageWidget()->raise();
}

void Main::reportError( const std::string & msg )
{
  instance()->messageWidget()->error(QString::fromUtf8(msg.c_str()));
  instance()->messageWidget()->show();
  instance()->messageWidget()->raise();
}

void Main::qtMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
  switch (type)
  {
  case QtDebugMsg:
    instance()->messageWidget()->message(msg);
    instance()->messageWidget()->show();
    instance()->messageWidget()->raise();
    break;
  case QtWarningMsg:
    instance()->messageWidget()->warning(msg);
    instance()->messageWidget()->show();
    instance()->messageWidget()->raise();
    break;
  case QtCriticalMsg:
    instance()->messageWidget()->error(msg);
    instance()->messageWidget()->show();
    instance()->messageWidget()->raise();
    break;
  case QtFatalMsg:
    cerr << "Fatal: " << msg.toLocal8Bit().constData() << endl;
    abort();
  }
}
