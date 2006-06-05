#ifndef MARX2DGRAPH_H
#define MARX2DGRAPH_H 

#include <QWidget>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QColor>
#include <QRectF>
#include <QLineF>
#include <QSize>
#include <QPoint>
#include <QGridLayout>
#include <QString>
#include <QMouseEvent>

#include <math.h> 

class Marx2DGraph : public QWidget
{
  //Q_OBJECT

 public: 

  Marx2DGraph( QWidget *parent=0 );
  Marx2DGraph( int size=0, QWidget *parent=0 );

  bool setBuffer( float*, int );

  void setPlotType(int);
  void setGraphDataColor( QColor );
  void setGraphLabelsAndAxisColor( QColor );
  void setBGColor( QColor );
  void setGraphDataPointSize( float );
  void setGraphDataPointType( int );
  void setGraphDataLineSize( float );
  void setAntialias(bool tf);
  
  void addLabel( std::string );


  enum { POINTS, PRECISION, LINEAR_INTERPOLATION, POLYNOMIAL_INTERPOLATION };
  enum { CIRCLES, SQUARES, XS };
  

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

  bool antialias;

  float mousex;
  float mousey;
  float dsampl; // sample associated with mouse click
  float dvalue;  // value associated with mouse click
  std::string dsamplestring;
  std::string dvaluestring;
  
  float* buffer;
  int buffersize;

  int plot_type;

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

  std::string label;

};


#endif
