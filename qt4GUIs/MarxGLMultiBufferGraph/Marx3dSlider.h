#ifndef MARX3DSLIDER_H
#define MARX3DSLIDER_H
                                                                                      
#include <QWidget>
#include <QPainter>
#include <QColor>
#include <QMouseEvent>

#include <iostream>


class Marx3dSlider : public QWidget
{
    Q_OBJECT

public:
  Marx3dSlider( QWidget *parent=0 );

  // the values in screen coordinates
  float getXPos( );
  float getYPos( );
  float getZPos( );

  // the values adjusted to the set interval lengths
  float getXVal();
  float getYVal();
  float getZVal();

  void setXinterval( float, float );
  void setYinterval( float, float );
  void setZinterval( float, float );

  void debug();

public slots:
  void setXPos( float );
  void setYPos( float );
  void setZPos( float );


signals:
  void positionChanged( float x, float y, float z );

protected:

  void paintEvent(QPaintEvent *);
  void resizeEvent(QResizeEvent *);

  void mousePressEvent( QMouseEvent* );
  void mouseMoveEvent( QMouseEvent* );
  void mouseReleaseEvent( QMouseEvent* );


private:

  float xpos, ypos, width, height;
  float xpos_knob, ypos_knob, zpos_knob;  //zpos_knob is in y screen coordinates


  float x_low, x_high;
  float y_low, y_high;
  float z_low, z_high;

  QColor bgcolor, outline_color, fill_color;

  bool xy_moveable;
  bool z_moveable;

  void drawXYKnob( float, float, QPainter* );

};



#endif
