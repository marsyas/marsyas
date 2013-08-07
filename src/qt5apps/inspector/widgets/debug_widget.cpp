#include "debug_widget.h"

#include <apps/debugger/debugger.hpp>
#include <apps/debugger/file_io.hpp>

#include <QDebug>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QAction>

DebugWidget::DebugWidget( QWidget * parent ):
  QWidget(parent),
  m_recording(0),
  m_debugger(0)
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

  connect(open_recording_action, SIGNAL(triggered()),
          this, SLOT(openRecording()));
  connect(m_bug_list, SIGNAL(itemClicked(QListWidgetItem*)),
          this, SLOT(onItemClicked(QListWidgetItem*)));
}

void DebugWidget::setSystem( Marsyas::MarSystem * system )
{
  if (m_system == system)
    return;
  m_system = system;
  recreateDebugger();
}

void DebugWidget::openRecording()
{
  QString filename =
      QFileDialog::getOpenFileName(this,
                                   "Open MarSystem Recording");
  if (filename.isEmpty())
    return;

  recording_reader reader;
  recording *new_recording = reader.read(filename.toStdString());

  if (!new_recording) {
    qWarning() << "*** Recording file could not be opened:" << filename;
    return;
  }

  delete m_recording;
  m_recording = new_recording;

  m_rec_label->setText(filename);

  recreateDebugger();
}

void DebugWidget::evaluate()
{
  if (!m_debugger)
    return;

  m_bug_list->clear();

  debugger::report *bugs = m_debugger->evaluate();
  if (!bugs)
  {
    QListWidgetItem *item = new QListWidgetItem;
    item->setText("End of recording.");
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

  delete bugs;
}

void DebugWidget::advance()
{
  if (m_debugger)
    m_debugger->advance();
}

void DebugWidget::rewind()
{
  if (m_debugger)
    m_debugger->rewind();
}

void DebugWidget::clear()
{
  m_bug_list->clear();
}

void DebugWidget::recreateDebugger()
{
  if (m_system && m_recording)
    m_debugger = new debugger(m_system, m_recording);
  else
  {
    delete m_debugger;
    m_debugger = 0;
  }
}

void DebugWidget::onItemClicked( QListWidgetItem * item )
{
  QString path = item->data(PathRole).toString();
  qDebug() << "path clicked:" << path;
  emit pathClicked(path);
}
