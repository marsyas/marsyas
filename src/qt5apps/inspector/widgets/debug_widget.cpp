#include "debug_widget.h"
#include "../debug_controller.h"
#include "../main.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QToolBar>

using namespace Marsyas;

DebugWidget::DebugWidget( ActionManager *action_mng,
                          DebugController * debugger, QWidget * parent ):
  QWidget(parent),
  m_debugger(debugger)
{
  QToolBar *tool_bar = new QToolBar;
  tool_bar->addAction( action_mng->action(ActionManager::OpenRecording) );
  tool_bar->addSeparator();
  tool_bar->addAction( action_mng->action(ActionManager::Tick) );
  tool_bar->addAction( action_mng->action(ActionManager::Rewind) );

  m_rec_label = new QLabel("<No Recording>");
  {
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(QPalette::WindowText, Qt::white);
    m_rec_label->setPalette(palette);
    m_rec_label->setAutoFillBackground(true);
  }

  m_report_view = new QTreeWidget();
  m_report_view->setHeaderLabels( QStringList()
                                  << "Name" << "Type" << "CPU Time"
                                  << "Real Time" << "Max Dev" << "Avg Dev");
  m_report_view->setIndentation(5);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(tool_bar);
  layout->addWidget(m_rec_label);
  layout->addWidget(m_report_view);
  setLayout(layout);

  connect(m_debugger, SIGNAL(recordingChanged(QString)),
          this, SLOT(onRecordingChanged(QString)));
  connect(m_debugger, SIGNAL(ticked()),
          this, SLOT(updateReport()));
  connect(m_report_view, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
          this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
}

void DebugWidget::setSystem( Marsyas::MarSystem * system )
{
  m_report_view->clear();
  recursiveAddSystem(system, m_report_view->invisibleRootItem());
  m_report_view->expandAll();
}

void DebugWidget::recursiveAddSystem( MarSystem *system,
                                      QTreeWidgetItem * parent )
{
  QString abs_path = QString::fromStdString(system->getAbsPath());
  QString rel_path = QString::fromStdString(system->getPrefix());
  QStringList path_elem = rel_path.split('/', QString::SkipEmptyParts);
  QString type, name;
  if (path_elem.size() > 0)
    type = path_elem[0];
  if (path_elem.size() > 1)
    name = path_elem[1];

  QTreeWidgetItem *item = new QTreeWidgetItem(parent);
  item->setData(0, AbsolutePathRole, abs_path);
  item->setData(0, RelativePathRole, rel_path);
  item->setData(0, Qt::DisplayRole, name);
  item->setData(1, Qt::DisplayRole, type);

  std::vector<MarSystem*> children = system->getChildren();
  for (MarSystem *child : children)
    recursiveAddSystem(child, item);
}

void DebugWidget::onRecordingChanged(const QString & filename)
{
  m_rec_label->setText(filename);
}

void DebugWidget::updateReport()
{
  if (!m_debugger)
    return;

  const Debug::Record * record = m_debugger->currentState();
  Q_ASSERT(record);

  recursiveUpdateChildItems( m_report_view->invisibleRootItem(), record );
}

void DebugWidget::recursiveUpdateChildItems
(QTreeWidgetItem *parent, const Marsyas::Debug::Record * record )
{
  int child_count = parent->childCount();
  for (int i = 0; i < child_count; ++i)
  {
    QTreeWidgetItem *item = parent->child(i);
    QString path = item->data(0, AbsolutePathRole).toString();

    const Debug::Record::Entry * entry = record->entry(path.toStdString());
    item->setData(2, Qt::DisplayRole,
                  entry ? QVariant(entry->cpu_time) : QVariant());
    item->setData(3, Qt::DisplayRole,
                  entry ? QVariant(entry->real_time) : QVariant());

    const Debug::BugReport & bug_report = m_debugger->report();
    const auto & bug_info = bug_report.find(path.toStdString());
    bool valid_bug_info = bug_info != bug_report.end();
    item->setData(4, Qt::DisplayRole,
                  valid_bug_info ?
                    QVariant(bug_info->second.max_deviation) :
                    QVariant());
    item->setData(5, Qt::DisplayRole,
                  valid_bug_info ?
                    QVariant(bug_info->second.average_deviation) :
                    QVariant());

    recursiveUpdateChildItems(item, record);
  }
}

void DebugWidget::onItemClicked( QTreeWidgetItem * item, int column )
{
  (void) column;
  QString path = item->data(0, AbsolutePathRole).toString();
  emit pathClicked(path);
}
