/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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
                                                                                        

#include "Marx2DGraph.h"

using namespace std;
using namespace Marsyas;

namespace MarsyasQt
{
//! A default constructor
/*!
  A default constructor when buffer size isn't otherwise specified.
*/
Marx2DGraph::Marx2DGraph( QWidget *parent )
  : QWidget(parent)
{
  Marx2DGraph(0, 0);
}


//! The real constructor
/*!
  The real constructor
                                                                                        
  \param size an integer giving buffer size
  \param parent a pointer to the parent widget
*/

Marx2DGraph::Marx2DGraph( int size, QWidget *parent )
  : QWidget(parent)
{

  xpos = 0;  // x border around graph
  ypos = 0;  // y border around graph
  xaxisoffset = 80;  // how far in to draw right of xaxis
  yaxisoffset = 30;  // how far in to draw top of yaxis
  width = QWidget::width() - 2*xpos;    // widget width
  height = QWidget::height() - 2*ypos;  // widget height
  gheight = height - yaxisoffset*2;     // graph height
  gwidth  = width  - xaxisoffset*2;     // graph width

  yaxisscale = 20.0;
  showXaxis = true;

  buffersize = size;
  buffer = new realvec(buffersize);

  plot_type = POINTS;
  point_size = 5;
  point_type = XS;
  pen_width = 2.;
  data_pen_width = 2.;

  antialias = false;  // much faster

  ticks = true;
  axis_display_type = CONNECTED;
  show_axis_scale = false;

  data_color = QColor(255, 0, 0);
  label_color = QColor(0, 0, 0);
  bg_color    = QColor(255, 255, 255);

  QPen pen;

  pen.setColor( QColor(0, 0, 0) );
  pen.setWidthF( pen_width );

  label = "";
  xlabelmessage = "";
  xlabel = false;
  ylabelmessage = "";
  ylabel = false;

  display = false;
  dsamplestring = "";
  dvaluestring = "";

  setPalette( QPalette( QColor(0, 0, 0) ) );
}


//! displayXaxis
/*!
  Toggles whether the X axis is drawn.
                                                                                        
  \param bool on/off
*/
void
Marx2DGraph::displayXaxis( bool tf )
{
  showXaxis = tf;
  update();
}


//! setXAxisLabel
/*!
  Add a label below and center of the x axis.
                                                                                        
  \param string message
*/
void 
Marx2DGraph::setXAxisLabel( string m )
{
  xlabelmessage = m;
  update();
}


//! setYAxisLabel
/*!
  Add a label below and center of the y axis.
                                                                                        
  \param string message
*/
void 
Marx2DGraph::setYAxisLabel( string m )
{
  ylabelmessage = m;
  update();
}


//! setXAxisLabel
/*!
  Add a label below and center of the x axis.
                                                                                        
  \param string message
*/
void 
Marx2DGraph::setXAxisLabelOn( bool tf )
{
  if (tf && !xlabel) { yaxisoffset += 16; }
  else if (!tf && xlabel) { yaxisoffset -= 16; }  

  xlabel = tf;  
  update();
}


//! setYAxisLabel
/*!
  Add a label below and center of the y axis.
                                                                                        
  \param string message
*/
void 
Marx2DGraph::setYAxisLabelOn( bool tf )
{
  if (tf && !ylabel) { xaxisoffset += 24; }
  else if (!tf && ylabel) { xaxisoffset -= 24; } 

  ylabel = tf;
  update();
}



//! setAxisDisplayType
/*!
  Controls how the axis will be drawn: enum { CONNECTED, BROKEN }.
                                                                                        
  \param type an int given by the enumeration above
*/
void
Marx2DGraph::setAxisDisplayType( int type )
{
  axis_display_type = type;
}


//! setGraphDataColor
/*!
  Change the plot color.
                                                                                        
  \param c a QColor object
*/
void 
Marx2DGraph::setGraphDataColor( QColor c )
{
  data_color = c;
  update();
}


//! setGraphLabelsAndAxisColor
/*!
  Change the labels and axis colors, ie. labels and axis are displayed
  in the same color
                                                                                        
  \param c a QColor object
*/
void 
Marx2DGraph::setGraphLabelsAndAxisColor( QColor c )
{
  label_color = c;
  update();
}


//! setBGColor
/*!
  Set the background color of the graph.  The default color is white.
                                                                                        
  \param c a QColor object
*/
void 
Marx2DGraph::setBGColor( QColor c )
{
  bg_color = c;
  update();
}


//! setGraphDataPointSize
/*!
  Set the point size when the graph type is POINT.
                                                                                        
  \param p a float such as magnitude 5.0
*/
void 
Marx2DGraph::setGraphDataPointSize(float p)
{
  point_size = p;
  update();
}


//! setGraphDataPointType
/*!
  Set the point style when the graph type is POINT.  Options are given
  in the enumeration: enum { CIRCLES, SQUARES, XS };
                                                                                        
  \param t an int corresponding to the above enum
*/
void 
Marx2DGraph::setGraphDataPointType( int t )
{
  point_type = t;
  update();
}


//! setGraphDataLineSize
/*!
  Set the graph line thickness when plotting style is
  LINEAR_INTERPOLATION or POLYNOMIAL_INTERPOLATION.
                                                                                        
  \param p a float such as magnitude 2.0
*/
void 
Marx2DGraph::setGraphDataLineSize( float p )
{
  data_pen_width = p;
  update();
}


//! setPlotType
/*!
  Choose the style of the plot: enum { POINTS, PRECISION,
  LINEAR_INTERPOLATION, POLYNOMIAL_INTERPOLATION }.
                                                                                        
  \param type an int corresponding to the above enumeration
*/
void 
Marx2DGraph::setPlotType(int type)
{
  plot_type = type;
  update();
}


//! setAntialias
/*!
  Toggles antialiasing on and off.  When rapidly redrawing plot
  buffers, such as in plotting realtime audio output, antialias will
  probably need to be turned off, which is the default configuration.
*/
void 
Marx2DGraph::setAntialias(bool tf)
{
  antialias = tf;
  update();
}


//! setShowAxisScale
/*!
  Toggle the displaying of axis scale label.

  \param bool true/false is on/off respectively
*/
void 
Marx2DGraph::setShowAxisScale( bool tf )
{
  show_axis_scale = tf;
  update();
}


//! setBuffer
/*!
  Set the buffer and automatically update the graphics.
                                                                                        
  \param *b a pointer to an array of floats
  \param s the size of the buffer
                                                                                        
  \return bool true if the buffer's successfully updated
*/
bool 
Marx2DGraph::setBuffer( realvec& rv )
{
	
  if ( rv.getSize() == buffersize ) { 
     *buffer = rv;

    int xu = (int)(xaxisoffset+xpos) + pen.width() + 1;
    int yu = (int)(yaxisoffset+ypos);
    int xl = (int)(xaxisoffset+xpos+gwidth);
    int yl = (int)(yaxisoffset+ypos+gheight) - pen.width() - 1;

    // repaint( QRect( QPoint(xu, yu), QPoint(xl, yl)) );
	update();
	
    return true;
  }
  return false;
}

void 
Marx2DGraph::setYMaxMin(float ymax, float ymin)
{
	ymaxval = ymax;
	yminval = ymin;
}
	


void
Marx2DGraph::draw_y_ticks(QPainter *painter)
{
  float interval = gheight/10;
  
  int stop = 10;
  int start = 0;
  if (!showXaxis){ stop = 11; }
  if (axis_display_type == BROKEN){
    stop = 10;
    start = 1;
  }
                                                                                        
  for (int i=start; i<stop; i++) {
    painter->drawLine( QLineF( xaxisoffset+xpos, yaxisoffset+ypos+i*interval,
                               xaxisoffset+xpos-5, yaxisoffset+ypos+i*interval) );

    /* draw axis scale label */
    if (show_axis_scale) {
      ostringstream ds;
	  float tinterval;
	  tinterval = (ymaxval-yminval)/10.0;
	  
      // ds << fixed << setprecision(3) << i * tinterval + buffer->minval();
	  
      ds << setprecision(3) << ymaxval - i * tinterval;
	  
      // ds << fixed << setprecision(1) << (yaxisoffset + ypos + i * interval-10)/(gheight/2);
      //(gheight/2.0 + ypos + yaxisoffset) - (gheight/2)*val

      painter->drawText( QPointF(xaxisoffset+xpos-80, yaxisoffset+ypos+i*interval), 
			QString( QString::fromStdString(ds.str()) )
			); 
    }

  }
  
}



void Marx2DGraph::draw_x_ticks(QPainter *painter)
{
  float interval = gwidth/10;
                                                                                        
  int stop=11;
  if (axis_display_type == BROKEN){ stop = 10; }
  for (int i=1; i<stop; i++) {
    painter->drawLine( QLineF( xaxisoffset+xpos+i*interval, yaxisoffset+ypos+gheight,
                               xaxisoffset+xpos+i*interval, yaxisoffset+ypos+gheight+5)
		       );

    /* draw axis scale label */
    if (show_axis_scale) {
      ostringstream ds;
      ds << fixed << setprecision(0) << ((float)buffersize/10.0)*(float)i;

      painter->drawText( QPointF(xaxisoffset+xpos+i*interval-10, yaxisoffset+ypos+gheight+17), 
			QString( QString::fromStdString(ds.str()) )
			); 
    }
  }   
}



//! addLabel
/*!
  Give the graph a label and update the graphic.
                                                                                        
  \param lb a string providing the label
*/
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

  pen.setColor( bg_color);
  painter.setPen( pen );
  painter.setBrush( QBrush( bg_color, Qt::SolidPattern ) );
  
  if (antialias) {
    painter.setRenderHint( QPainter::Antialiasing );
  }

    
  /* draw backdrop */
  painter.drawRect( QRectF( xpos, ypos, width, height ) );
  pen.setColor( QColor(0, 0, 0));
  painter.setPen( pen );
  
  /* draw y axis */
  float ext = 0.0;
  if (axis_display_type == BROKEN){ ext = gheight/10; }
  painter.drawLine( QLineF( xaxisoffset+xpos, yaxisoffset+ypos+ext,
                            xaxisoffset+xpos, yaxisoffset+ypos+gheight-ext ) );
  if (ticks)
    draw_y_ticks( &painter );

  if (ylabel) {
    painter.rotate(270); 
    painter.drawText( QPointF(-1*(height/2)-32, 
			      xpos+xaxisoffset-32 ), 
		      QString(QString::fromStdString(ylabelmessage))
		      );
    painter.rotate(90);   
  }
  
  /* draw x axis */
  if (showXaxis) {
    ext = 0.0;
    if (axis_display_type == BROKEN){ ext = gwidth/10; }
    painter.drawLine( QLineF( xaxisoffset+xpos+ext, yaxisoffset+ypos+gheight,
                              xaxisoffset+xpos+gwidth-ext, yaxisoffset+ypos+gheight ) );
    if (ticks)
      draw_x_ticks( &painter );

    if (xlabel) {
      painter.drawText( QPointF((width/2)-32, 
				yaxisoffset+ypos+gheight+30 ), 
			QString(QString::fromStdString(xlabelmessage))
			);     
    }
  }
  
  /* draw label */
  if (label != "") {
	  // painter.rotate(270);      
	  // painter.drawText( QPointF(-1*(gheight+ypos+2*yaxisoffset-2), 
	  // gwidth+xpos+2*xaxisoffset-5 ), 
	  // QString(QString::fromStdString(label)));
	  

	  painter.drawText( QPointF(60,30),
						QString(QString::fromStdString(label))
						
		  );
	  // painter.rotate(90);
  }
  

  

  /* draw data */
  painter.setBrush( QBrush( data_color, Qt::SolidPattern ) );
  pen.setColor( data_color );
  pen.setWidthF( data_pen_width );
  painter.setPen(pen);
  
  switch (plot_type) {
  case POINTS:
    plotpoints(&painter); break;
  case PRECISION:
    plotprecision(&painter); break;
  case LINEAR_INTERPOLATION:
    plotlinearinterpolation(&painter); break;
  case POLYNOMIAL_INTERPOLATION:
    plotpolyinterpolation(&painter); 
    break;
  default:
    plotpoints(&painter); break;
  }
  pen.setWidthF( pen_width );


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
    table[0] = (float)((*buffer)(h)); 
    table[1] = (float)((*buffer)(h+1));
    table[2] = (float)((*buffer)(h+2)); 
    table[3] = (float)((*buffer)(h+3));
    
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
    float val = (float)((*buffer)(i));

    float w = ((gwidth/(buffersize-1)))*i + xpos + xaxisoffset;
    float h = (gheight/2.0 + ypos + yaxisoffset) - (gheight/2)*val;

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
    float val = (float)((*buffer)(i));

    float w = (gwidth/(buffersize-1))*i + xpos + xaxisoffset;
    float h = (gheight/2.0 + ypos + yaxisoffset) - (gheight/2)*val;
    float b = yaxisoffset+ypos+gheight;
    painter->drawLine( QLineF( w, h, w, b ) );
  }
}


void 
Marx2DGraph::plotpoints(QPainter* painter)
{
  float p = point_size/2.;
  for (int i=0; i<buffersize; i++) {
    float val = (float)((*buffer)(i));



    float w = (gwidth/(buffersize-1))*i + xpos + xaxisoffset;
    float h = (gheight/2.0 + ypos + yaxisoffset) - (gheight/2.0)*val;

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

  dsample = (mousex - xpos - xaxisoffset)*(buffersize - 1)/gwidth;
  display = true;

  if (me->button() == Qt::LeftButton) {
	  dsamplestring = "x = ";
	  dvaluestring  = "y = ";
	  
	  if (dsample - floor(dsample) > .5) { dsample = ceil(dsample);}
	  else { dsample = floor(dsample); }
	  dvalue  = (*buffer)((int)dsample); 


	  dvalue *= 0.5;
	  dvalue += 0.5;
	  dvalue *= (ymaxval - yminval);
	  dvalue += yminval;

	  
  }
  else if (me->button() == Qt::RightButton) {
	  dsamplestring = "x = ";
	  dvaluestring  = "y = ";
	  
	  dvalue  = (mousey - gheight/2. - ypos - yaxisoffset)*-2/gheight;
  }
  
  ostringstream ds, dv;
  ds << setprecision(4) << dsample;
  dsamplestring.append( ds.str() );
  dv << setprecision(4) << dvalue;
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

} //namespace
