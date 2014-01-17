/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_INSPECTOR_REALVEC_WIDGET_INCLUDED
#define MARSYAS_INSPECTOR_REALVEC_WIDGET_INCLUDED

#include <marsyas/system/MarSystem.h>
#include <marsyas/realvec.h>

#include <qwt_plot.h>
#include <qwt_series_data.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>

#include <QWidget>
#include <QTableView>
#include <QAbstractTableModel>
#include <QLineEdit>
#include <QStackedLayout>
#include <QComboBox>
#include <QToolButton>
#include <QSignalMapper>

namespace MarsyasQt {
class RealvecTableWidget;
}

class DebugController;
class RealvecPlot;

enum FlowDirection {
  UndefinedFlowDirection,
  Input,
  Output
};

class RealvecWidget : public QWidget
{
  Q_OBJECT
public:
  enum DisplayType {
    Table,
    Points,
    Sticks,
    Line,
    Curve,
    Image
  };

  RealvecWidget( DebugController *debugger, QWidget * parent = 0 );
  ~RealvecWidget();
  QSize sizeHint() const { return QSize(500,300); }
  QSize minimumSizeHint() const { return QSize(50,50); }

public slots:
  void setDisplayType( int type );
  void displayControl( Marsyas::MarSystem *system, const QString & path );
  void displayPort( Marsyas::MarSystem * system, FlowDirection port );
  void refresh(bool autoScale = false);
  void clear();
  void autoScale();

signals:
  void pathChanged(const QString &);

private:
  void refreshFromControl();
  void refreshFromPort();
  void clearPlot();

  DebugController *m_debugger;

  bool m_display_port;
  FlowDirection m_port_type;
  QString m_path;
  Marsyas::MarSystem * m_system;

  QStackedLayout * m_stack;
  QComboBox * m_display_type_selector;
  QToolButton * m_auto_scale_btn;

  MarsyasQt::RealvecTableWidget *m_table;
  QwtPlot *m_plotter;
  RealvecPlot *m_plot;
  Marsyas::realvec m_data;

  QSignalMapper m_style_menu_mapper;
};

class RealvecRow : public QwtSeriesData<QPointF>
{
public:
  RealvecRow():
    m_row(0)
  {}

  RealvecRow(const Marsyas::realvec * data, int row ):
    m_data(data),
    m_row(row),
    have_bounding_rect(false)
  {}

  int row() const { return m_row; }

  virtual size_t size () const
  {
    return (size_t) m_data->getCols();
  }

  virtual QPointF sample (size_t i) const
  {
    return QPointF(i, (*m_data)(m_row, (Marsyas::mrs_natural)i));
  }

  virtual QRectF boundingRect () const
  {
    using Marsyas::mrs_real;
    using Marsyas::mrs_natural;
    const Marsyas::realvec & data = *m_data;

    if (have_bounding_rect)
      return d_boundingRect;

    int count = data.getCols();
    if (!count)
      return QRectF(0, 0, count, 0);

    mrs_real min, max;
    min = max = data(m_row, 0);
    for (mrs_natural i=1; i<count; ++i)
    {
      mrs_real d = data(m_row, i);
      if (d < min)
        min = d;
      else if (d > max)
        max = d;
    }
    d_boundingRect = QRectF(0, min, count, max - min);
    const_cast<bool&>(have_bounding_rect) = true;
    return d_boundingRect;
  }

private:
  const Marsyas::realvec *m_data;
  int m_row;
  bool have_bounding_rect;
};

class RealvecRaster : public QwtRasterData
{
public:
  RealvecRaster( const Marsyas::realvec * data ):
    m_data(data)
  {}

  QRectF pixelHint ( const QRectF & area ) const
  {
    (void) area;
    return QRectF(0, 0, 1, 1);
  }

  double value (double x, double y) const
  {
    using Marsyas::mrs_natural;
    using Marsyas::mrs_real;
    const Marsyas::realvec & data = *m_data;

    mrs_natural column = (mrs_natural)(x);
    mrs_natural row = (mrs_natural)(y);
    if (column < 0 || column >= data.getCols()
        || row < 0 || row >= data.getRows())
      return 0.0;
    else
      return data(row, column);
  }

private:
  const Marsyas::realvec *m_data;
};

class RealvecPlot : public QObject
{
  Q_OBJECT
public:
  RealvecPlot( QwtPlot *plotter ):
    m_plotter(plotter)
  {}
  virtual ~RealvecPlot() {}
  virtual void setData( const Marsyas::realvec * data ) = 0;
  virtual void clear() = 0;
  virtual void fitRange() = 0;
protected:
  QwtPlot *m_plotter;
};

class RealvecPlotCurve : public RealvecPlot
{
  Q_OBJECT
public:
  RealvecPlotCurve( QwtPlot *plotter );
  ~RealvecPlotCurve();
  void setType( int );
  void setData( const Marsyas::realvec * data );
  void clear();
  void fitRange();
private:
  void applyType();
  QList<QwtPlotCurve*> m_curves;
  QwtPlotCurve::CurveStyle m_style;
  bool m_fitted;
  const Marsyas::realvec *m_data;
};

class RealvecPlotImage : public RealvecPlot
{
  Q_OBJECT
public:
  RealvecPlotImage( QwtPlot *plotter );
  ~RealvecPlotImage();
  void setData( const Marsyas::realvec * data );
  void clear();
  void fitRange();
private:
  QwtPlotSpectrogram m_image;
  QwtLinearColorMap *m_color_map;
  const Marsyas::realvec *m_data;
  QwtInterval m_range;
};

#endif // MARSYAS_INSPECTOR_REALVEC_WIDGET_INCLUDED
