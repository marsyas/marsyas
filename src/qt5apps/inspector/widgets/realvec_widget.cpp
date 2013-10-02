#include "realvec_widget.h"
#include "../debug_controller.h"
#include "../../common/realvec_table_widget.h"

#include <marsyas/system/MarControl.h>

#include <QDebug>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

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

  m_auto_scale_btn = new QToolButton;
  m_auto_scale_btn->setText(tr("AutoScale"));

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

  QHBoxLayout *tool_row = new QHBoxLayout();
  tool_row->addWidget(m_display_type_selector);
  tool_row->addWidget(m_auto_scale_btn);

  QVBoxLayout *column = new QVBoxLayout();
  column->setContentsMargins(0,0,0,0);
  column->setSpacing(0);
  column->addWidget(m_label);
  column->addLayout(tool_row);
  column->addLayout(m_stack);

  setLayout(column);

  connect( m_display_type_selector, SIGNAL(activated(int)), this, SLOT(setDisplayType(int)) );
  connect( m_auto_scale_btn, SIGNAL(clicked()), this, SLOT(autoScale()) );

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
    refresh(true);
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

  refresh(true);
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

  refresh(true);
}

void RealvecWidget::refresh(bool doAutoScale)
{
  if (m_path.isEmpty())
    return;

  if (m_display_port)
    refreshFromPort();
  else
    refreshFromControl();

  if (m_plot && doAutoScale)
    m_plot->fitRange();

  m_plotter->replot();
}

void RealvecWidget::autoScale()
{
  if (m_plot) {
    m_plot->fitRange();
    m_plotter->replot();
  }
}

void RealvecWidget::clear()
{
  m_path.clear();
  m_system = 0;
  clearPlot();
  m_plotter->replot();
  m_label->clear();
}

void RealvecWidget::refreshFromControl()
{
  Q_ASSERT(m_system);
  Q_ASSERT(!m_path.isEmpty());

  MarControlPtr control = m_system->getControl( m_path.toStdString() );
  if (control.isInvalid()) {
    qWarning() << "RealvecWidget: invalid control path:" << m_path;
    clearPlot();
    return;
  }

  if (control->getType() != "mrs_realvec") {
    //qWarning() << "RealvecWidget: control type not 'mrs_realvec':" << m_control_path;
    clearPlot();
    return;
  }

  const realvec & data = control->to<mrs_realvec>();
  m_table->setData(data);

  m_data = data;
  if (m_plot)
    m_plot->setData(&m_data);
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
    clearPlot();
    return;
  }

  m_table->setData(*data);

  m_data = *data;
  if (m_plot)
    m_plot->setData(&m_data);
}

void RealvecWidget::clearPlot()
{
  m_data = realvec();
  m_table->setData( realvec() );
  if (m_plot)
    m_plot->clear();
}

//////////////////////////////////////

RealvecPlotCurve::RealvecPlotCurve( QwtPlot *plotter ):
  RealvecPlot(plotter),
  m_style(QwtPlotCurve::Lines),
  m_fitted(false),
  m_data(0)
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

  m_data = data;
}

void RealvecPlotCurve::clear()
{
  foreach (QwtPlotCurve *curve, m_curves)
  {
    curve->detach();
    delete curve;
  }
  m_curves.clear();
  m_data = 0;
}

void RealvecPlotCurve::fitRange()
{
  if (m_data)
  {
    double min = m_data->minval();
    double max = m_data->maxval();
    if (min == max) { min -= 1; max += 1; }

    m_plotter->setAxisScale(QwtPlot::yLeft, min, max);
  }
}

//////////////////////////////////

RealvecPlotImage::RealvecPlotImage( QwtPlot *plotter ):
  RealvecPlot(plotter),
  m_data(0),
  m_range(-1, 1)
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
  raster->setInterval(Qt::ZAxis, m_range);

  m_image.setData( raster );
  m_image.attach(m_plotter);

  m_plotter->setAxisScale(QwtPlot::yLeft, 0, data->getRows());
  m_plotter->setAxisScale(QwtPlot::xBottom, 0, data->getCols());

  m_data = data;
}

void RealvecPlotImage::clear()
{
  m_image.detach();
  m_data = 0;
}

void RealvecPlotImage::fitRange()
{
  if (!m_data)
    return;

  double min = m_data->minval();
  double max = m_data->maxval();
  if (min == max) { min -= 1; max += 1; }

  m_range = QwtInterval(min, max);
  QwtRasterData *raster = m_image.data();
  if (raster)
      raster->setInterval(Qt::ZAxis, m_range);
}
