#include "realvec_widget.h"
#include "../debug_controller.h"
#include "../../common/realvec_table_widget.h"

#include <marsyas/system/MarControl.h>

#include <QDebug>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QAction>

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

  m_display_type_selector = new QComboBox;
  m_display_type_selector->addItems( QStringList()
                                     << "Table" << "Points" << "Sticks"
                                     << "Line" << "Curve" << "Spectrogram" );

  m_auto_scale_btn = new QToolButton;
  m_auto_scale_btn->setText("S");
  m_auto_scale_btn->setToolTip("Auto Scale");

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

  // Toolbar

  QStringList style_names;
  style_names << "Table" << "Points" << "Sticks"
              << "Line" << "Curve" << "Spectrogram";

  QMenu *style_menu = new QMenu(this);
  int style_idx = 0;
  foreach (const QString & style_name, style_names)
  {
    QAction * action = style_menu->addAction(style_name);
    m_style_menu_mapper.setMapping(action, style_idx);
    connect(action, SIGNAL(triggered()), &m_style_menu_mapper, SLOT(map()));
    style_idx++;
  }

  connect(&m_style_menu_mapper, SIGNAL(mapped(int)),
          this, SLOT(setDisplayType(int)));

  QToolButton *style_btn = new QToolButton;
  style_btn->setText("D");
  style_btn->setToolTip("Display Mode");
  style_btn->setMenu(style_menu);
  style_btn->setPopupMode(QToolButton::InstantPopup);

  // Layout

  QVBoxLayout *tool_layout = new QVBoxLayout();
  tool_layout->addWidget(style_btn);
  tool_layout->addWidget(m_auto_scale_btn);
  tool_layout->addStretch();

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setContentsMargins(0,0,0,0);
  layout->setSpacing(0);
  layout->addLayout(m_stack);
  layout->addLayout(tool_layout);

  setLayout(layout);

  connect( m_display_type_selector, SIGNAL(activated(int)), this, SLOT(setDisplayType(int)) );
  connect( m_auto_scale_btn, SIGNAL(clicked()), this, SLOT(autoScale()) );

  m_display_type_selector->setCurrentIndex(Line);
  setDisplayType(Line);
}

RealvecWidget::~RealvecWidget()
{
  // must manually delete m_plot because it's destructor accesses m_plotter
  delete m_plot;
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

  MarControlPtr control = system->getControl( path.toStdString() );
  if (!control->hasType<mrs_realvec>())
    return;

  m_display_port = false;
  m_path = path;
  m_system = system;

  refresh(true);

  QString abs_path = QString::fromStdString(control->path());
  emit pathChanged(abs_path);
}

void RealvecWidget::displayPort( MarSystem * system,
                                 FlowDirection port )
{
  if (m_display_port && m_system == system && m_port_type == port)
    return;

  m_display_port = true;
  m_port_type = port;
  m_system = system;
  m_path = QString::fromStdString(system->getAbsPath());

  refresh(true);

  QString display_path = QString::fromStdString(system->path());
  if (port == Input)
    display_path += " [input]";
  else
    display_path += " [output]";
  emit pathChanged(display_path);
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
  emit pathChanged(QString());
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

  if (!control->hasType<mrs_realvec>()) {
    qWarning() << "RealvecWidget: control type not 'mrs_realvec':" << m_path;
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
  Q_ASSERT(m_system);
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

  if (row_count < 1 || column_count < 1)
  {
    clear();
    return;
  }

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

  m_plotter->setAxisScale
      ( QwtPlot::xBottom,
        0.0,
        std::max((mrs_natural)(column_count - 1), (mrs_natural) 1) );

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
  m_plotter->setAxisScale( QwtPlot::xBottom, 0.0, 1.0 );
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
  m_color_map = new QwtLinearColorMap("white", "black");
  m_image.setDisplayMode(QwtPlotSpectrogram::ImageMode, true);
  m_image.setColorMap(m_color_map);
}

RealvecPlotImage::~RealvecPlotImage()
{
  clear();
}

void RealvecPlotImage::setData( const Marsyas::realvec * data )
{
  if (data->getRows() < 1 || data->getCols() < 1)
  {
    clear();
    return;
  }

  double y_range = (double) data->getRows();
  double x_range = (double) data->getCols();
  RealvecRaster *raster = new RealvecRaster(data);
  raster->setInterval(Qt::YAxis, QwtInterval(0, y_range, QwtInterval::ExcludeMaximum));
  raster->setInterval(Qt::XAxis, QwtInterval(0, x_range, QwtInterval::ExcludeMaximum));
  raster->setInterval(Qt::ZAxis, m_range);

  m_image.setData( raster );
  m_image.attach(m_plotter);

  m_plotter->setAxisScale(QwtPlot::yLeft, 0, y_range);
  m_plotter->setAxisScale(QwtPlot::xBottom, 0, x_range);

  m_data = data;
}

void RealvecPlotImage::clear()
{
  m_image.detach();
  m_data = 0;
  m_plotter->setAxisScale(QwtPlot::yLeft, 0, 1.0);
  m_plotter->setAxisScale(QwtPlot::xBottom, 0, 1.0);
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
