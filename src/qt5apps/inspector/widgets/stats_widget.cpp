#include "stats_widget.h"
#include "../debug_controller.h"
#include "../main.h"

#include <QDebug>
#include <QVBoxLayout>

using namespace Marsyas;

StatisticsWidget::StatisticsWidget( ActionManager *,
                          DebugController * debugger, QWidget * parent ):
  QWidget(parent),
  m_debugger(debugger)
{
  m_report_view = new QTreeWidget();
  m_report_view->setHeaderLabels( QStringList()
                                  << "Name" << "Type" << "CPU Time"
                                  << "Real Time" << "Max Dev" << "Avg Dev");
  m_report_view->setIndentation(5);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(m_report_view);
  setLayout(layout);

  connect(m_debugger, SIGNAL(ticked()),
          this, SLOT(updateReport()));
  connect(m_report_view, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
          this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
}

void StatisticsWidget::setSystem( Marsyas::MarSystem * system )
{
  m_report_view->clear();
  if (!system)
    return;
  recursiveAddSystem(system, m_report_view->invisibleRootItem());
  m_report_view->expandAll();
}

void StatisticsWidget::recursiveAddSystem( MarSystem *system,
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

void StatisticsWidget::updateReport()
{
  if (!m_debugger)
    return;

  const Debug::Record * record = m_debugger->currentState();
  Q_ASSERT(record);

  recursiveUpdateChildItems( m_report_view->invisibleRootItem(), record );
}

void StatisticsWidget::recursiveUpdateChildItems
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

void StatisticsWidget::onItemClicked( QTreeWidgetItem * item, int column )
{
  (void) column;
  QString path = item->data(0, AbsolutePathRole).toString();
  emit pathClicked(path);
}
