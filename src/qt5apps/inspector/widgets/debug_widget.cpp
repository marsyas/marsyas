#include "debug_widget.h"
#include "../debug_controller.h"
#include "../main.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QToolBar>

DebugWidget::DebugWidget( ActionManager *action_mng,
                          DebugController * debugger, QWidget * parent ):
  QWidget(parent),
  m_debugger(debugger)
{
  QToolBar *tool_bar = new QToolBar;
  tool_bar->addAction( action_mng->action(ActionManager::OpenRecording) );
  tool_bar->addSeparator();
  tool_bar->addAction( action_mng->action(ActionManager::Tick) );

  m_rec_label = new QLabel("<No Recording>");
  {
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    palette.setColor(QPalette::WindowText, Qt::white);
    m_rec_label->setPalette(palette);
    m_rec_label->setAutoFillBackground(true);
  }

  m_bug_list = new QListWidget();

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addWidget(tool_bar);
  layout->addWidget(m_rec_label);
  layout->addWidget(m_bug_list);
  setLayout(layout);

  connect(m_debugger, SIGNAL(recordingChanged(QString)),
          this, SLOT(onRecordingChanged(QString)));
  connect(m_debugger, SIGNAL(ticked()),
          this, SLOT(updateReport()));
  connect(m_bug_list, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(onItemClicked(QListWidgetItem*)));
}

void DebugWidget::onRecordingChanged(const QString & filename)
{
  m_rec_label->setText(filename);
}

void DebugWidget::updateReport()
{
  if (!m_debugger)
    return;

  m_bug_list->clear();

  const debugger::report *bugs = m_debugger->bugReport();
  if (!bugs)
  {
    QListWidgetItem *item = new QListWidgetItem;
    item->setText("No record to compare.");
    m_bug_list->addItem(item);
    return;
  }

  for (const auto & bug_mapping : *bugs)
  {
    QString report_path = QString::fromStdString(bug_mapping.first);
    QString report_text;

    const debugger::bug & state = bug_mapping.second;
    switch (state.flags)
    {
    case debugger::path_missing:
      report_text =
        report_path
        + " FAILURE: invalid path.";
      break;
    case debugger::format_mismatch:
      report_text =
        report_path
        + " FAILURE: format mismatch: ";
      break;
    case debugger::value_mismatch:
      report_text =
        report_path
        + " MISMATCH:"
        + " average deviation = " + QString::number(state.average_deviation)
        + ", maximum deviation = " + QString::number(state.max_deviation);
      break;
    default:
      report_text =
        report_path
        + " Ooops: unrecognized bug.";
      break;
    }

    QListWidgetItem *item = new QListWidgetItem;
    item->setText(report_text);
    item->setData(PathRole, report_path);
    m_bug_list->addItem(item);
  }

  if (bugs->empty())
  {
    QListWidgetItem *item = new QListWidgetItem;
    item->setText("OK.");
    m_bug_list->addItem(item);
  }
}

void DebugWidget::clear()
{
  m_bug_list->clear();
}

void DebugWidget::onItemClicked( QListWidgetItem * item )
{
  QString path = item->data(PathRole).toString();
  emit pathClicked(path);
}
