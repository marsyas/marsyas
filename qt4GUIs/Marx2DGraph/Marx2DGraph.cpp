#include "Marx2DGraph.h"

#include <string> 
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;



Marx2DGraph::Marx2DGraph( QWidget *parent )
  : QWidget(parent)
{
  Marx2DGraph(0, 0);
}

Marx2DGraph::Marx2DGraph( int size, QWidget *parent )
  : QWidget(parent)
{

  xpos = 0;  // x border around graph
  ypos = 0;  // yborder around graph
  xaxisoffset = 20;  // how far in to draw right of xaxis
  yaxisoffset = 20;  // how far in to draw top of yaxis
  width = QWidget::width() - 2*xpos;  
  height = QWidget::height() - 2*ypos;  // widget height
  gheight = height - yaxisoffset*2;     // graph height
  gwidth  = width  - xaxisoffset*2;     // graph width

  yaxisscale = 20.0;

  buffersize = size;
  buffer = new float[buffersize];

  plot_type = POINTS;
  point_size = 5;
  point_type = XS;
  pen_width = 2.;

  antialias = false;  // much faster

  ticks = true;

  data_color = QColor(255, 0, 0);
  label_color = QColor(0, 0, 0);
  bg_color    = QColor(255, 255, 255);

  QPen pen;

  pen.setColor( QColor(0, 0, 0) );
  pen.setWidthF( pen_width );

  label = "";

  display = false;
  dsamplestring = "";
  dvaluestring = "";

  setPalette( QPalette( QColor(0, 0, 0) ) );
}


void 
Marx2DGraph::setGraphDataColor( QColor c )
{
  data_color = c;
  update();}


void 
Marx2DGraph::setGraphLabelsAndAxisColor( QColor c )
{
  label_color = c;
  update();
}


void 
Marx2DGraph::setBGColor( QColor c )
{
  bg_color = c;
  update();
}


void 
Marx2DGraph::setGraphDataPointSize(float p)
{
  point_size = p;
}


void 
Marx2DGraph::setGraphDataPointType( int t )
{
  point_type = t;
}

void 
Marx2DGraph::setGraphDataLineSize( float p )
{
  pen_width = p;
}


void 
Marx2DGraph::setPlotType(int type)
{
  plot_type = type;
}

void 
Marx2DGraph::setAntialias(bool tf)
{
  antialias = tf;
}


bool 
Marx2DGraph::setBuffer( float *b, int s )
{
  if ( s == buffersize ) { 
    buffer = b;

    int xu = (int)(xaxisoffset+xpos) + pen.width() + 1;
    int yu = (int)(yaxisoffset+ypos);
    int xl = (int)(xaxisoffset+xpos+gwidth);
    int yl = (int)(yaxisoffset+ypos+gheight) - pen.width() - 1;

    repaint( QRect( QPoint(xu, yu), QPoint(xl, yl)) );
    //repaint();
    return true;
  }
  return false;
}


void
Marx2DGraph::draw_y_ticks(QPainter *painter)
{
  float interval = gheight/10;

  for (int i=0; i<10; i++) {
    painter->drawLine( QLineF( xaxisoffset+xpos, yaxisoffset+ypos+i*interval,
			       xaxisoffset+xpos-5, yaxisoffset+ypos+i*interval) );
  }
}


void Marx2DGraph::draw_x_ticks(QPainter *painter)
{
  float interval = gwidth/10;

  for (int i=1; i<11; i++) {
    painter->drawLine( QLineF( xaxisoffset+xpos+i*interval, yaxisoffset+ypos+gheight,
			       xaxisoffset+xpos+i*interval, yaxisoffset+ypos+gheight+5) );
  }
}


void 
Marx2DGraph::addLabel( string lb )
{
  label = lb;
  update();
}


void 
Marx2DGraph::paintEvent( QPaintEvent * )
{
  QPainter painter(this);

  painter.setPen( QPen( QColor(0, 0, 0) ) );
  painter.setBrush( QBrush( QColor(255, 255, 255), Qt::SolidPattern ) );
  
  if (antialias) {
    painter.setRenderHint( QPainter::Antialiasing );
  }

    
  /* draw backdrop */
  painter.drawRect( QRectF( xpos, ypos, width, height ) );
  
  /* draw y axis */
  painter.drawLine( QLineF( xaxisoffset+xpos, yaxisoffset+ypos, 
			    xaxisoffset+xpos, yaxisoffset+ypos+gheight ) );
  if (ticks)
    draw_y_ticks( &painter );
  
  /* draw x axis */
  painter.drawLine( QLineF( xaxisoffset+xpos, yaxisoffset+ypos+gheight, 
			    xaxisoffset+xpos+gwidth, yaxisoffset+ypos+gheight ) );
  if (ticks)
    draw_x_ticks( &painter );
  
  /* draw label */
  if (label != "") {
    painter.rotate(270);      painter.drawText( QPointF(-1*(gheight+ypos+2*yaxisoffset-2), 
							gwidth+xpos+2*xaxisoffset-5 ), 
						QString(QString::fromStdString(label))
			);
    painter.rotate(90);
  }



  /* draw data */
  painter.setBrush( QBrush( data_color, Qt::SolidPattern ) );
  pen.setColor( data_color );
  painter.setPen(pen);
  
  switch (plot_type) {
  case POINTS:
    plotpoints(&painter); break;
  case PRECISION:
    plotprecision(&painter); break;
  case LINEAR_INTERPOLATION:
    plotlinearinterpolation(&painter); break;
  case POLYNOMIAL_INTERPOLATION:
    plotpoints(&painter);
    plotpolyinterpolation(&painter); 
    break;
  default:
    plotpoints(&painter); break;
  }


  // if necessary, draw the floating pane displaying the cursor coordinates
  if (display) {
    mousex += 15; // a little offset to keep window out from under cursor
    mousey += 15; 

    if (mousex + 150 > QWidget::width()) { mousex = mousex - 150; }
    if (mousey + 33  > QWidget::height()) { mousey = mousey - 33; }

    painter.setBrush( QBrush( QColor(215, 215, 215), Qt::Dense4Pattern ) );
    painter.setPen( QPen( Qt::NoPen ) );
    painter.drawRect( QRectF( mousex, mousey, 150, 33 ) );

    painter.setPen( QPen( QColor(0, 0, 0) ) );
    painter.drawText( QPointF( mousex + 5, mousey + 13) , 
		      QString(QString::fromStdString(dsamplestring)) );
    painter.drawText( QPointF( mousex + 5, mousey + 30) , 
		      QString(QString::fromStdString(dvaluestring)) );

    display = false;
  }

  painter.end();
}


void 
Marx2DGraph::plotpolyinterpolation(QPainter* painter)
{

  // divided difference algorithm
  // modified from kyle louden's "algorithms in c"

  float lastw=-1, lasth;
  for (int h=0; h<buffersize-3; h++) {
    float *table, *coeff, *x, *pz, *z;
    float term;

    int steps = 64;
    
    table = new float[4];
    x     = new float[4];
    coeff = new float[4];
    pz    = new float[steps];
    z     = new float[steps];
    
    x[0] = h+0; x[1] = h+1; x[2] = h+2; x[3] = h+3;
    table[0] = buffer[h+0]; 
    table[1] = buffer[h+1];
    table[2] = buffer[h+2]; 
    table[3] = buffer[h+3];
    
    coeff[0] = table[0];
    
    int j;
    int n=4;
    for (int k=1; k<n; k++) {
      for (int i=0; i<n-k; i++) {
	j = i + k;
	table[i] = (table[i+1] - table[i])/(x[j] - x[i]);
      }
      coeff[k] = table[0];
    }
    
    int m=steps;
    float zn=h;
    for (int k=0; k<m; k++) {
      pz[k] = coeff[0];
      
      for (j=1; j<n; j++) {
	term = coeff[j];
	
	for (int i=0; i<j; i++) {
	  term = term * (zn - x[i]);
	}

	pz[k] = pz[k] + term;
      }

      z[k] = zn;
      zn = zn + 4./m;  // there are four points covered in m steps

      if ((h != buffersize-4 && k <= m/4) || 
	  h == buffersize-4) {
	float w = (gwidth/(buffersize-1))*z[k] + xpos + xaxisoffset;
	float h = (gheight/2.0 + ypos + yaxisoffset) - (gheight/2)*pz[k];
	if ( w > gwidth + xpos + xaxisoffset) {  // hack that needs fixing
	  break;
	}
	
	if (lastw >=0) { painter->drawLine( QLineF( lastw, lasth, w, h ) ); }
	
	lastw = w;
	lasth = h;
      }
    }
  }

}


void 
Marx2DGraph::plotlinearinterpolation(QPainter* painter)
{
  float lastw=-1, lasth;
  for (int i=0; i<buffersize; i++) {
    float w = ((gwidth/(buffersize-1)))*i + xpos + xaxisoffset;
    float h = (gheight/2.0 + ypos + yaxisoffset) - (gheight/2)*buffer[i];

    if (lastw >=0) {
      painter->drawLine( QLineF( lastw, lasth, w, h ) );  
    }

    lastw = w;
    lasth = h;
  }
}


void 
Marx2DGraph::plotprecision(QPainter* painter)
{
  for (int i=0; i<buffersize; i++) {
    float w = (gwidth/(buffersize-1))*i + xpos + xaxisoffset;
    float h = (gheight/2.0 + ypos + yaxisoffset) - (gheight/2)*buffer[i];
    float b = yaxisoffset+ypos+gheight;
    painter->drawLine( QLineF( w, h, w, b ) );
  }
}


void 
Marx2DGraph::plotpoints(QPainter* painter)
{
  float p = point_size/2.;
  for (int i=0; i<buffersize; i++) {
    float w = (gwidth/(buffersize-1))*i + xpos + xaxisoffset;
    float h = (gheight/2.0 + ypos + yaxisoffset) - (gheight/2)*buffer[i];

    switch (point_type) {
    case CIRCLES:
      painter->drawEllipse( QRectF( w, h, point_size, point_size ) );
      break;
    case SQUARES:
      painter->drawRect( QRectF( w, h, point_size, point_size ) );
      break;
    case XS:
      painter->drawLine( QLineF(w-p, h-p, w+p, h+p ) );
      painter->drawLine( QLineF(w-p, h+p, w+p, h-p ) );
      break;
    default:
      painter->drawEllipse( QRectF( w, h, point_size, point_size ) );
    }
  }
}


void  
Marx2DGraph::resizeEvent(QResizeEvent *)
{

  if ( QWidget::width() < width - 2*xpos ||
       QWidget::width() > width - 2*xpos - 20 ) {
    width = QWidget::width() - 2*xpos;  
    gwidth  = width  - xaxisoffset*2;  // graph width
  }

  if ( QWidget::height() < height - 2*ypos ||
       QWidget::height() > height - 2*ypos - 20 ) {
    height = QWidget::height() - 2*ypos;  // widget height
    gheight = height - yaxisoffset*2;  // graph height
  }
  
}


void  
Marx2DGraph::mousePressEvent(QMouseEvent* me)
{
  mousex = (float)me->x();
  mousey = (float)me->y();

  double dsample = (mousex - xpos - xaxisoffset)*(buffersize - 1)/gwidth;
  display = true;

  if (me->button() == Qt::LeftButton) {
    dsamplestring = "closest sample = ";
    dvaluestring  = "     its value = ";

    if (dsample - floor(dsample) > .5) { dsample = ceil(dsample);}
    else { dsample = floor(dsample); }
    dvalue  = buffer[(int)dsample]; 

    //std::cout << "left dsample: " << dsample << "\t" << "dvalue " << dvalue << "\n";
  }
  else if (me->button() == Qt::RightButton) {
    dsamplestring = "estimated sample = ";
    dvaluestring  = "       its value = ";

    dvalue  = (mousey - gheight/2. - ypos - yaxisoffset)*-2/gheight;
    //std::cout << "right dsample: " << dsample << "\t" << "dvalue " << dvalue << "\n";
  }

  ostringstream ds, dv;
  ds << setprecision(2) << dsample;
  dsamplestring.append( ds.str() );
  dv << setprecision(2) << dvalue;
  dvaluestring.append( dv.str() );

  update();
}


void 
Marx2DGraph::mouseMoveEvent(QMouseEvent *)
{
  //std::cout << "mouse moved\n";
}


void 
Marx2DGraph::mouseReleaseEvent(QMouseEvent *)
{
  update();
}
