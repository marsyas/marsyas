#include "realvec_widget.h"

#include <MarControl.h>

#include <QDebug>
#include <QStackedLayout>
#include <QVBoxLayout>

using namespace Marsyas;
using namespace MarsyasQt;

RealvecWidget::RealvecWidget( QWidget * parent ):
  QWidget(parent),
  m_system(0),
  m_graph(0)
{
  m_label = new QLabel;
  {
    QFont f = m_label->font();
    f.setPointSize( f.pointSize() + 1 );
    m_label->setFont(f);
  }

  m_display_type_selector = new QComboBox;
  m_display_type_selector->addItems( QStringList()
                                  << "Table" << "Points" << "Precision"
                                  << "Linear Interpolation" << "Polynomial Interpolation" );

  m_table = new QTableView;
  m_realvec_model = new RealvecModel(this);
  m_table->setModel(m_realvec_model);

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

void RealvecWidget::setSystem( Marsyas::MarSystem * system )
{
  if (m_system == system)
    return;

  m_system = system;
  m_control_path.clear();
  m_label->clear();

  refresh();
}

void RealvecWidget::displayControl( const QString & path )
{
  if (m_control_path == path)
    return;

  m_control_path = path;
  m_label->setText(path);

  refresh();
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

void RealvecWidget::refresh()
{
  if (!m_system || m_control_path.isEmpty()) {
    clear();
    return;
  }

  MarControlPtr control = m_system->getControl( m_control_path.toStdString() );
  if (control.isInvalid()) {
    qWarning() << "RealvecWidget: invalid control path:" << m_control_path;
    clear();
    return;
  }

  if (control->getType() != "mrs_realvec") {
    //qWarning() << "RealvecWidget: control type not 'mrs_realvec':" << m_control_path;
    clear();
    return;
  }

  const realvec & data = control->to<mrs_realvec>();

  refreshTable(data);
  refreshGraph(data);
#if 0
  int columns = data->getColumns();
  int rows = data->getRows();
  m_image = QImage(columns, rows);
  for (int r = 0; r < rows; ++r)
  {
    QRgb *line = static_cast<QRgb*>( scanLine(r) );
    for (int c = 0; c < columns; ++c)
    {
      double value = data(r,c);
      line[c] = qRgb(value * 0.5 + 0.5 * 255, 0, 0);
    }
  }
#endif
}

void RealvecWidget::refreshTable( const Marsyas::realvec & data )
{
  m_realvec_model->setData(data);
}

void RealvecWidget::refreshGraph( const Marsyas::realvec & data )
{
  m_graph->resetBuffer(data);
}

void RealvecWidget::clear()
{
  m_realvec_model->setData( realvec() );
  m_graph->resetBuffer(realvec());
}

//////////////////////

void RealvecModel::setData( const Marsyas::realvec & data )
{
  bool same_dimensions = data.getRows() == m_data.getRows() && data.getCols() == m_data.getCols();
  bool is_empty = data.getRows() == 0 || data.getCols() == 0;

  if (same_dimensions) {
    if (!is_empty) {
      m_data = data;
      emit dataChanged( index(0,0), index(m_data.getRows()-1, m_data.getCols()-1) );
    }
  }
  else
  {
    beginResetModel();
    m_data = data;
    endResetModel();
  }
}

int RealvecModel::rowCount(const QModelIndex & parent) const
{
  (void) parent;
  return m_data.getRows();
}

int RealvecModel::columnCount(const QModelIndex & parent) const
{
  (void) parent;
  return m_data.getCols();
}

Qt::ItemFlags RealvecModel::flags(const QModelIndex & index) const
{
  (void) index;
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant RealvecModel::data(const QModelIndex & index, int role) const
{
  switch (role)
  {
  case Qt::DisplayRole:
    return QVariant( m_data(index.row(), index.column()) );
  default:
    return QVariant();
  }
}
