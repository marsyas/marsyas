/****************************************
  GainNode - Implementation File
  TODO find that balance between the bas class an inheriting classes
*****************************************/
#include "GainNode.h"

GainNode::GainNode(QString name,QWidget* parent)
  :MarSystemNode(MarSystemNodeFactory::GAIN_TYPE,name,parent)
{
}

GainNode::GainNode(MarSystem* msys,QWidget* parent)
  :MarSystemNode(msys,parent)
{
}

void
GainNode::resizeEvent(QResizeEvent*)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);
  QPainter paint(this);
  //QBrush testBrush(QPoint(0,0),Qt::blue,QPoint(pix.width()/5,pix.height()),Qt::white);
  paint.setBrush(Qt::white/*testBrush*/);
  paint.setRenderHint(QPainter::Antialiasing);
  paint.drawRoundRect(QRectF(0.5, 0.5, width()-1, height()-1), 25, 25);

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint((width()-size.width())/2, (height()-size.height())/2), size),
                 Qt::AlignCenter,  name_);
  paint.end();
  //paint.drawRect(0,0,pix.width(),pix.height());
  /* QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);
  */
  update();
}



void GainNode::paintEvent(QPaintEvent*)
{
  cout << "GainNode::paintEvent" << endl;
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  QPixmap pix(width(), height());
  QPainter paint(this);
  //QBrush testBrush(QPoint(0,0),Qt::blue,QPoint(pix.width()/5,pix.height()),Qt::white);
  paint.setBrush(Qt::white/*testBrush*/);
  paint.setRenderHint(QPainter::Antialiasing);
  paint.drawRoundRect(QRectF(0.5, 0.5, width()-1, height()-1), 25, 25);

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint((width()-size.width())/2, (height()-size.height())/2), size),
                 Qt::AlignCenter,  name_);
  paint.end();
  //paint.drawRect(0,0,pix.width(),pix.height());
  QPalette pal(palette());

  setPalette(pal);
  setPixmap(pix);

}
