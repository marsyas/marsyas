#include "realvec_widget.h"
#include "../debug_controller.h"
#include "../../common/realvec_table_widget.h"

#include <MarControl.h>

#include <QDebug>
#include <QStackedLayout>
#include <QVBoxLayout>

using namespace Marsyas;
using namespace MarsyasQt;

RealvecWidget::RealvecWidget( DebugController *debugger, QWidget * parent ):
  QWidget(parent),
  m_debugger(debugger),
  m_display_debugger(false),
  m_system(0),
  m_graph(0)
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

  m_label = new QLineEdit;
  m_label->setReadOnly(true);
  {
    QFont f = m_label->font();
    f.setPointSize( f.pointSize() + 1 );
    m_label->setFont(f);
  }

  m_display_type_selector = new QComboBox;
  m_display_type_selector->addItems( QStringList()
                                     << "Table" << "Points" << "Precision"
                                     << "Linear Interpolation" << "Polynomial Interpolation" );

  m_table = new MarsyasQt::RealvecTableWidget;
  m_table->setEditable(false);

  m_graph = new Marx2DGraph(0);

  m_stack = new QStackedLayout();
  m_stack->addWidget(m_table);
  m_stack->addWidget(m_graph);

  QVBoxLayout *column = new QVBoxLayout();
  column->addWidget(m_label);
  column->addWidget(m_display_type_selector);
  column->addLayout(m_stack);

  setLayout(column);

  connect( m_display_type_selector, SIGNAL(activated(int)), this, SLOT(setDisplayType(int)) );
}

void RealvecWidget::setDisplayType( int type )
{
  if (type == Table)
    m_stack->setCurrentWidget(m_table);
  else {
    int graph_plot_type = (int) type - 1;
    m_graph->setPlotType( graph_plot_type );
    m_stack->setCurrentWidget(m_graph);
  }
}

void RealvecWidget::displayControl( MarSystem * system, const QString & path )
{
  if (!m_display_debugger && m_system == system && m_path == path)
    return;

  m_display_debugger = false;
  m_path = path;
  m_system = system;

  QString abs_path = QString::fromStdString(system->getAbsPath()) + path;
  m_label->setText(abs_path);

  refresh();
}

void RealvecWidget::displayDebugValue(const QString & path )
{
  if (m_display_debugger && m_path == path)
    return;

  m_display_debugger = true;
  m_system = 0;
  m_path = path;

  m_label->setText(path);

  refresh();
}

void RealvecWidget::refresh()
{
  if (m_path.isEmpty())
    return;

  if (m_display_debugger)
    refreshFromDebugger();
  else
    refreshFromControl();
}

void RealvecWidget::clear()
{
  m_path.clear();
  m_system = 0;
  clearData();
  m_label->clear();
}

void RealvecWidget::refreshFromControl()
{
  Q_ASSERT(m_system);
  Q_ASSERT(!m_path.isEmpty());

  MarControlPtr control = m_system->getControl( m_path.toStdString() );
  if (control.isInvalid()) {
    qWarning() << "RealvecWidget: invalid control path:" << m_path;
    clearData();
    return;
  }

  if (control->getType() != "mrs_realvec") {
    //qWarning() << "RealvecWidget: control type not 'mrs_realvec':" << m_control_path;
    clearData();
    return;
  }

  const realvec & data = control->to<mrs_realvec>();

  m_table->setData(data);
  m_graph->resetBuffer(data);
}

void RealvecWidget::refreshFromDebugger()
{
  Q_ASSERT(!m_path.isEmpty());

  const realvec * data = m_debugger->currentValue(m_path);
  if (!data)
  {
    qWarning() << "RealvecWidget: invalid debugger path:" << m_path;
    clearData();
    return;
  }

  m_table->setData(*data);
  m_graph->resetBuffer(*data);
}

void RealvecWidget::clearData()
{
  m_table->setData( realvec() );
  m_graph->resetBuffer(realvec());
}
