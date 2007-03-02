
#include "Marx3dSlider.h"

using namespace std;

Marx3dSlider::Marx3dSlider( QWidget *parent ) :
  QWidget( parent )
{
  xpos = ypos = 0.0f;

  height = QWidget::height();
  width  = QWidget::height();  // this is not a bug, width=QWidget::height()!


  setMinimumWidth( int(1.025*width + width*.15) );

  x_low = y_low = z_low = 0.f;
  x_high = width - width*.1;
  y_high = z_high = height - height*.1;

  xpos_knob = ypos_knob = zpos_knob = 0.f;  // indicates needs initialization

  bgcolor = QColor( 255, 255, 255);
  outline_color = QColor( 0, 0, 0);
  fill_color = QColor( 128, 128, 128);

  xy_moveable = false;  // can we reposition x,y slider currently?
  z_moveable = false;  // can we reposition z slider currently?

}


void 
Marx3dSlider::paintEvent(QPaintEvent *)
{

  setMinimumWidth( int(1.025*width + width*.15) );

  QPainter painter( this );
  //QPen pen;
                                                                                    
  //pen.setColor( bgcolor );
  painter.setPen( QPen( bgcolor) );
  painter.setBrush( QBrush( bgcolor, Qt::SolidPattern ) );
  painter.setRenderHint( QPainter::Antialiasing );
                                                                                          
  /* draw backdrop */
  painter.drawRect( QRectF( xpos, ypos, width, height ) );
  painter.drawRect( QRectF( xpos + 1.025*width, 
			    ypos, 
			    width*.15, 
			    height ) );
  painter.setPen( QPen( outline_color) );
  painter.setBrush( QBrush( outline_color, Qt::SolidPattern ) );

  // float zbackground_xpos = xpos + width + width*.025 + (width*.15)/2 - (width*.015)/2;
  // the commented equation above reduces to the uncommented equation below
  float zbackground_xpos = xpos + 1.0925*width;
  painter.drawRect( QRectF( zbackground_xpos,
			    ypos + height*.05,
			    width*.015,
			    height*.9) );


  drawXYKnob( xpos_knob, ypos_knob, &painter);
  drawXYKnob( xpos + 1.05*width, zpos_knob, &painter);


}


void 
Marx3dSlider::resizeEvent(QResizeEvent *)
{

  float old = height;

  height = QWidget::height();
  width =  QWidget::height();


  float ratio = height/old;

  if (xpos_knob != -1 && ypos_knob != -1) {
    xpos_knob *= ratio;
    ypos_knob *= ratio;
    zpos_knob *= ratio;
  }

}


void
Marx3dSlider::setXinterval( float l, float h )
{
  if ( h >= l ) {
    x_low = l; x_high = h;
  }
  else {
    x_low = h; x_high = l;
  }
}


void
Marx3dSlider::setYinterval( float l, float h )
{
  if ( h >= l ) {
    y_low = l; y_high = h;
  }
  else {
    y_low = h; y_high = l;
  }
}


void
Marx3dSlider::setZinterval( float l, float h )
{
  if ( h >= l ) {
    z_low = l; z_high = h;
  }
  else {
    z_low = h; z_high = l;
  }
}


float
Marx3dSlider::getXPos( )
{
  return xpos_knob;
}


float
Marx3dSlider::getYPos( )
{
  return ypos_knob;
}


float
Marx3dSlider::getZPos( )
{
  return zpos_knob;
}


float
Marx3dSlider::getXVal()
{
  return (xpos_knob/width) * (x_high - x_low) + x_low;
}


float
Marx3dSlider::getYVal()
{
  return (ypos_knob/height)*(y_high - y_low) + y_low;
}


float
Marx3dSlider::getZVal()
{
  return (zpos_knob/height)*(z_high - z_low) + z_low;
}


void
Marx3dSlider::debug()
{
  cout << xpos_knob << "\t" << ypos_knob << "\t" << zpos_knob << endl;
}

void
Marx3dSlider::setXPos( float x )
{
  float interval_width = x_high - x_low;

  if (interval_width != 0) {
    x = ((x - x_low)*width)/interval_width;

    if (x >= 0 && x <= width-width*.1) {
      xpos_knob = x;
      emit positionChanged( getXVal(), getYVal(), getZVal() );
    }
  }
  
  update();
}


void
Marx3dSlider::setYPos( float y )
{
  float interval_height = y_high - y_low;
  y = ((y - y_low)*height)/interval_height;

  if (y >= 0 && y <= height-height*.1) {
    ypos_knob = y;
    emit positionChanged( getXVal(), getYVal(), getZVal() );
  }

  update();
}


void
Marx3dSlider::setZPos( float z )
{
  float interval_height = z_high - z_low;

  z = ((z - z_low)*QWidget::height())/interval_height;

  if (z >= 0 && z <= height-height*.1) {
    zpos_knob = z;
    emit positionChanged( getXVal(), getYVal(), getZVal() );
  }
  
  update();
}


void
Marx3dSlider::drawXYKnob( float x, float y, QPainter *p)
{
  p->setPen( QPen( outline_color ) );
  p->setBrush( QBrush( fill_color, Qt::SolidPattern ) );  
  p->drawEllipse( QRectF( x, y, width*.1, height*.1) );
}

void
Marx3dSlider::mousePressEvent(QMouseEvent* me) 
{
  float mousex = (float)me->x();
  float mousey = (float)me->y();

  if (mousex >= xpos_knob && 
      mousex <= xpos_knob + width*.1 &&
      mousey >= ypos_knob &&
      mousey <= ypos_knob + height*.1) {
    xy_moveable = true;
  }
  else if (mousex >= xpos + 1.05*width && 
	   mousex <= xpos + 1.15*width &&
	   mousey >= zpos_knob &&
	   mousey <= zpos_knob + height*.1) {
    z_moveable = true;
  }


}


void
Marx3dSlider::mouseMoveEvent(QMouseEvent* me)
{
  float mousex = (float)me->x();
  float mousey = (float)me->y();

  if (xy_moveable) {
    if (mousex > width*.05 && mousex < width - width*.05) { xpos_knob = mousex - width*.05; }
    if (mousey > height*.05 && mousey < height - height*.05) { ypos_knob = mousey - height*.05; }

    emit positionChanged( getXVal(), getYVal(), getZVal() );
  }
  if (z_moveable) {
    if (mousey > height*.05 && mousey < height - height*.05) { zpos_knob = mousey - height*.05; }

    emit positionChanged( getXVal(), getYVal(), getZVal() );
  }

  update();
}


void
Marx3dSlider::mouseReleaseEvent( QMouseEvent* )
{
  xy_moveable = false;
  z_moveable = false;
}
