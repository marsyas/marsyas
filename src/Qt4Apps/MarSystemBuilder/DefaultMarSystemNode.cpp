/****************************************
  DefaultMarSystemNode - Implementation File
  TODO find that balance between the bas class an inheriting classes
*****************************************/
#include "DefaultMarSystemNode.h"

DefaultMarSystemNode::DefaultMarSystemNode(string type,QString name,QWidget* parent)
  :MarSystemNode(type,name,parent)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  QPixmap pix(width(),height());
  QPainter paint(&pix);
  //QBrush testBrush(QPoint(0,0),Qt::blue,QPoint(pix.width()/5,pix.height()),Qt::white);
  paint.setBrush(Qt::white/*testBrush*/);
  paint.setRenderHint(QPainter::Antialiasing);
  paint.drawRoundRect(QRectF(0.5, 0.5, pix.width()-1, pix.height()-1), 25, 25);

  //QFont font(font());
  //font.setStyleStrategy(QFont::ForceOutline);

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint((width()-size.width())/2, (height()-size.height())/2), size),
                 Qt::AlignCenter,  name_);
  paint.end();
  //paint.drawRect(0,0,pix.width(),pix.height());
  QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);
  setPixmap(pix);
}

/**
 * the Constructor I imagine will be called more often
*/
DefaultMarSystemNode::DefaultMarSystemNode(MarSystem* msys,QWidget* parent)
  :MarSystemNode(msys,parent)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  QPixmap pix(width(),height());
  QPainter paint(&pix);
  //QBrush testBrush(QPoint(0,0),Qt::blue,QPoint(pix.width()/5,pix.height()),Qt::white);
  paint.setBrush(Qt::white/*testBrush*/);
  paint.setRenderHint(QPainter::Antialiasing);
  paint.drawRoundRect(QRectF(0.5, 0.5, pix.width(), pix.height()), 25, 25);

  //QFont font(font());
  //font.setStyleStrategy(QFont::ForceOutline);

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint((width()-size.width())/2, (height()-size.height())/2), size),
                 Qt::AlignCenter,  name_);
  paint.end();
  //paint.drawRect(0,0,pix.width(),pix.height());
  QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);
  setPixmap(pix);
}

void
DefaultMarSystemNode::resizeEvent(QResizeEvent*)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  QPixmap pix(width(),height());
  QPainter paint(&pix);
  //QBrush testBrush(QPoint(0,0),Qt::blue,QPoint(pix.width()/5,pix.height()),Qt::white);
  paint.setBrush(Qt::white/*testBrush*/);
  paint.setRenderHint(QPainter::Antialiasing);
  paint.drawRoundRect(QRectF(0.5, 0.5, pix.width()-1, pix.height()-1), 25, 25);

  //QFont font(font());
  //font.setStyleStrategy(QFont::ForceOutline);

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint((width()-size.width())/2, (height()-size.height())/2), size),
                 Qt::AlignCenter,  name_);
  paint.end();
  //paint.drawRect(0,0,pix.width(),pix.height());
  QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);
  setPixmap(pix);
  update();
}
