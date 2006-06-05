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

#include<sstream>
#include <iostream>
#include <iomanip>
#include <string>

#include "realvec.h"

using namespace std;

class Marx2DGraph : public QWidget
{
  //Q_OBJECT

 public: 

  Marx2DGraph( QWidget *parent=0 );
  Marx2DGraph( int size=0, QWidget *parent=0 );

  bool setBuffer( realvec& );

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
  void setXAxisLabel( string );
  void setYAxisLabel( string );
  void setXAxisLabelOn( bool );
  void setYAxisLabelOn( bool );

  void displayXaxis( bool );
  
  void addLabel( string );


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
  string dsamplestring;
  string dvaluestring;
  
/*   float* buffer; */
  realvec* buffer;
  long buffersize;

  int plot_type;
  int axis_display_type;

  string xlabelmessage;
  string ylabelmessage;
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

  string label;

};


#endif
