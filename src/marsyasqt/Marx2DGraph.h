




#ifndef MARX2DGRAPH_H
#define MARX2DGRAPH_H 

#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QPalette>
#include <QColor>
#include <QRectF>
#include <QLineF>
#include <QSize>
#include <QGridLayout>
#include <QString>
#include <QMouseEvent>

#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "realvec.h"
namespace MarsyasQt
{
/**
	\brief Fully-featured realvec plotting Qt object
	\ingroup MarsyasQt

Marx2DGraph provides 2D graphing for Marsyas where the ordinates are
given by a marsyas realvec object and the abscissa a per sample spacing.
Plot types are give in Marx2DGraph.h and include points, precsion,
linear and polynomial interpolation.
*/


class Marx2DGraph : public QWidget
{
  Q_OBJECT

 public: 

  Marx2DGraph( QWidget *parent=0 ); //warning C4520: 'Marx2DGraph' : multiple default constructors specified [!]	
  Marx2DGraph( int size=0, QWidget *parent=0 );//warning C4520: 'Marx2DGraph' : multiple default constructors specified [!]

  bool setBuffer( Marsyas::realvec& );

  void test();

  void setPlotType( int );
  void setAxisDisplayType( int );
  void setGraphDataColor( QColor );
  void setGraphLabelsAndAxisColor( QColor );
  void setBGColor( QColor );
  void setGraphDataPointSize( float );
  void setGraphDataPointType( int );
  void setGraphDataLineSize( float );
  void setShowAxisScale( bool );
  void setAntialias( bool );
  void setXAxisLabel( std::string );
  void setYAxisLabel( std::string );
  void setXAxisLabelOn( bool );
  void setYAxisLabelOn( bool );
		void setYMaxMin(float ymax, float ymin);
  void displayXaxis( bool );
  
  void addLabel( std::string );


  enum { POINTS, PRECISION, LINEAR_INTERPOLATION, POLYNOMIAL_INTERPOLATION };
  enum { CIRCLES, SQUARES, XS }; // point display shape
  enum { CONNECTED, BROKEN };    // axis display types
  

 protected:
  void mousePressEvent(QMouseEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);

		

 private:
  QPainter painter;

  void plotpoints(QPainter *);
  void plotprecision(QPainter *);
  void plotlinearinterpolation(QPainter *);
  void plotpolyinterpolation(QPainter *);

  void draw_x_ticks(QPainter *);
  void draw_y_ticks(QPainter *);

  bool ticks;
  bool display;
  bool showXaxis;
  bool show_axis_scale;

  bool antialias;

  float mousex;
  float mousey;
  float dsample; // sample associated with mouse click
  float dvalue;  // value associated with mouse click
  std::string dsamplestring;
  std::string dvaluestring;
  
/*   float* buffer; */
  Marsyas::realvec* buffer;
  long buffersize;
		float ymaxval, yminval;
		

  int plot_type;
  int axis_display_type;

  std::string xlabelmessage;
  std::string ylabelmessage;
  bool xlabel;
  bool ylabel;

  float width, height, xpos, ypos;
  float gheight, gwidth;
  float xaxisoffset;
  float yaxisoffset;
  float yaxisscale;

  float point_size;
  int point_type;

  QColor data_color;
  QColor label_color;
  QColor bg_color;

  QPen pen;
  float pen_width;
  float data_pen_width;

  std::string label;

};
} // namespace

#endif
