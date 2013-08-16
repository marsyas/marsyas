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
  m_display_port(false),
  m_port_type(UndefinedFlowDirection),
  m_system(0),
  m_plot(0)
{
  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );


  {
    QPalette palette;
    palette.setColor(QPalette::Base, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);

    m_label = new QLineEdit;
    m_label->setReadOnly(true);
    m_label->setFrame(false);
    m_label->setPalette(palette);
  }

  m_display_type_selector = new QComboBox;
  m_display_type_selector->addItems( QStringList()
                                     << "Table" << "Points" << "Sticks"
                                     << "Line" << "Curve" << "Spectrogram" );

  m_table = new MarsyasQt::RealvecTableWidget;
  m_table->setEditable(false);

  m_plotter = new QwtPlot;
  m_plotter->setCanvasBackground( Qt::black );
  m_plotter->setAxisFont(QwtPlot::yLeft, font());
  m_plotter->setAxisFont(QwtPlot::xBottom, font());
  {
    QFrame *frame = qobject_cast<QFrame*>(m_plotter->canvas());
    if (frame)
      frame->setFrameShape(QFrame::NoFrame);
  }
  m_plotter->setContentsMargins(2,2,2,2);

  m_stack = new QStackedLayout();
  m_stack->addWidget(m_table);
  m_stack->addWidget(m_plotter);

  QVBoxLayout *column = new QVBoxLayout();
  column->setContentsMargins(0,0,0,0);
  column->setSpacing(0);
  column->addWidget(m_label);
  column->addWidget(m_display_type_selector);
  column->addLayout(m_stack);

  setLayout(column);

  connect( m_display_type_selector, SIGNAL(activated(int)), this, SLOT(setDisplayType(int)) );

  m_display_type_selector->setCurrentIndex(Line);
  setDisplayType(Line);
}

void RealvecWidget::setDisplayType( int type )
{
  if (type == Table)
  {
    m_stack->setCurrentWidget(m_table);
  }
  else
  {
    delete m_plot;
    if (type == Image)
    {
      m_plot = new RealvecPlotImage(m_plotter);
    }
    else
    {
      RealvecPlotCurve *plot = new RealvecPlotCurve(m_plotter);
      plot->setType(type);
      m_plot = plot;
    }
    refresh();
    m_stack->setCurrentWidget(m_plotter);
  }
}

void RealvecWidget::displayControl( MarSystem * system, const QString & path )
{
  if (!m_display_port && m_system == system && m_path == path)
    return;

  m_display_port = false;
  m_path = path;
  m_system = system;

  QString abs_path = QString::fromStdString(system->getAbsPath()) + path;
  m_label->setText(abs_path);

  refresh();
}

void RealvecWidget::displayPort(const QString & path,
                                           FlowDirection port)
{
  if (m_display_port && m_path == path && m_port_type == port)
    return;

  m_display_port = true;
  m_port_type = port;
  m_system = 0;
  m_path = path;

  m_label->setText(path);

  refresh();
}

void RealvecWidget::refresh()
{
  if (m_path.isEmpty())
    return;

  if (m_display_port)
    refreshFromPort();
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

  m_data = data;
  if (m_plot)
    m_plot->setData(&m_data);
  m_plotter->replot();
}

void RealvecWidget::refreshFromPort()
{
  Q_ASSERT(!m_path.isEmpty());

  const realvec * data = 0;

  const Debug::Record *record =  m_debugger->currentState();
  if (record)
  {
    const Debug::Record::Entry *entry = record->entry(m_path.toStdString());
    if (entry)
    {
      switch(m_port_type)
      {
      case Input:
        data = &entry->input; break;
      case Output:
        data = &entry->output; break;
      default:
        break;
      }
    }
  }

  if (!data)
  {
    clearData();
    return;
  }

  m_table->setData(*data);

  m_data = *data;
  if (m_plot)
    m_plot->setData(&m_data);
  m_plotter->replot();
}

void RealvecWidget::clearData()
{
  m_data = realvec();
  m_table->setData( realvec() );
  if (m_plot)
    m_plot->clear();
  m_plotter->replot();
}

//////////////////////////////////////
RealvecPlotCurve::RealvecPlotCurve( QwtPlot *plotter ):
  RealvecPlot(plotter),
  m_style(QwtPlotCurve::Lines),
  m_fitted(false)
{}

RealvecPlotCurve::~RealvecPlotCurve()
{
  clear();
}

void RealvecPlotCurve::setType(int type )
{
  switch(type)
  {
  case RealvecWidget::Points:
    m_style = QwtPlotCurve::Dots;
    break;
  case RealvecWidget::Sticks:
    m_style = QwtPlotCurve::Sticks;
    break;
  case RealvecWidget::Line:
    m_style = QwtPlotCurve::Lines;
    break;
  case RealvecWidget::Curve:
    m_style = QwtPlotCurve::Lines;
    m_fitted = true;
    break;
  default:
    qCritical() << "RealvecPlotCurve: Can not handle plot type:" << type;
    return;
  }

  applyType();
}

void RealvecPlotCurve::applyType()
{
  foreach (QwtPlotCurve *curve, m_curves)
  {
    curve->setStyle( m_style );
    curve->setCurveAttribute( QwtPlotCurve::Fitted, m_fitted );
  }
}

void RealvecPlotCurve::setData( const realvec * data )
{
  mrs_natural row_count = data->getRows();
  mrs_natural column_count = data->getCols();

  if (row_count != m_curves.count())
  {
    clear();

    for (mrs_natural r = 0; r<row_count; ++r)
    {
      QwtPlotCurve *curve = new QwtPlotCurve;
      curve->setPen( QColor::fromHsvF( (qreal)r / row_count, 1, 1) );
      curve->attach(m_plotter);
      m_curves.append(curve);
    }

    applyType();
  }

  for (mrs_natural r = 0; r<row_count; ++r)
  {
    m_curves[r]->setData(new RealvecRow(data, r));
  }

  m_plotter->setAxisScale(QwtPlot::xBottom, 0.0, column_count);
  m_plotter->setAxisScale(QwtPlot::yLeft, -1, 1);
}

void RealvecPlotCurve::clear()
{
  foreach (QwtPlotCurve *curve, m_curves)
  {
    curve->detach();
    delete curve;
  }
  m_curves.clear();
}

//////////////////////////////////

RealvecPlotImage::RealvecPlotImage( QwtPlot *plotter ):
  RealvecPlot(plotter)
{
  m_image.setDisplayMode(QwtPlotSpectrogram::ImageMode, true);
}

RealvecPlotImage::~RealvecPlotImage()
{
  clear();
}

void RealvecPlotImage::setData( const Marsyas::realvec * data )
{
  RealvecRaster *raster = new RealvecRaster(data);
  raster->setInterval(Qt::YAxis, QwtInterval(0, data->getRows(), QwtInterval::ExcludeMaximum));
  raster->setInterval(Qt::XAxis, QwtInterval(0, data->getCols(), QwtInterval::ExcludeMaximum));
  raster->setInterval(Qt::ZAxis, QwtInterval(-1, 1));

  m_image.setData( raster );
  m_image.attach(m_plotter);

  m_plotter->setAxisScale(QwtPlot::yLeft, 0, data->getRows());
  m_plotter->setAxisScale(QwtPlot::xBottom, 0, data->getCols());
}

void RealvecPlotImage::clear()
{
  m_image.detach();
}
