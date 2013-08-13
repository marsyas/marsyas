#include "debug_widget.h"
#include "../debug_controller.h"

#include <QDebug>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QAction>

DebugWidget::DebugWidget( DebugController * debugger, QWidget * parent ):
  QWidget(parent),
  m_debugger(debugger)
{
  m_rec_label = new QLabel("<No recording. Right-click to open...>");
  QAction * open_recording_action = new QAction("Open Recording...", this);
  m_rec_label->addAction( open_recording_action );
  m_rec_label->setContextMenuPolicy( Qt::ActionsContextMenu );

  m_bug_list = new QListWidget();

  QVBoxLayout *layout = new QVBoxLayout;
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(2);
  layout->addWidget(m_rec_label);
  layout->addWidget(m_bug_list);
  setLayout(layout);

  connect(m_debugger, SIGNAL(ticked()), this, SLOT(updateReport()));
  connect(open_recording_action, SIGNAL(triggered()),
          this, SLOT(openRecording()));
  connect(m_bug_list, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(onItemClicked(QListWidgetItem*)));
}

void DebugWidget::openRecording()
{
  QString filename =
    QFileDialog::getOpenFileName(this,
                                 "Open MarSystem Recording");
  if (filename.isEmpty())
    return;

  if (!m_debugger->setRecording(filename))
    return;

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
