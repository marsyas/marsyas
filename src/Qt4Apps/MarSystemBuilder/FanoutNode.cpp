/**
 * FanoutNode Implementation File
 * Implements functions from FanMarSystemNode.h
 * Author: Peter Minter
 */

#include "FanoutNode.h"

/**
 * Constructor
 * Creates the pix map for the Fanout.... TODO: should also make it
 * droppable so new MarSystemNodes can be dropped on it. And added to it
 */
FanoutNode::FanoutNode(QString name,QWidget* parent)
  :CompositeNode(MarSystemNodeFactory::FANOUT_TYPE,name,parent)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  setGeometry((parent->width()-width())/2,10,
              size.width()+12,size.height()+12);
  QPixmap pix(width(),height());

  QPainter paint;
  paint.begin(&pix);
  paint.setBrush(Qt::white);
  paint.setPen(Qt::black);
  paint.drawRect(0,0,pix.width()-1,pix.height()-1);

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, name_);
  paint.end();

  QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);

  setPixmap(pix);
}

/**
 * Constructor
 * Creates the pix map for the Fanout....
 */
FanoutNode::FanoutNode(MarSystem* msys,QWidget* parent)
  :CompositeNode(msys,parent)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  setGeometry((parent->width()-width())/2,10,
              size.width()+12,size.height()+12);
  QPixmap pix(width(),height());

  QPainter paint;
  paint.begin(&pix);
  paint.setBrush(Qt::white);
  paint.setPen(Qt::black);
  paint.drawRect(0,0,pix.width(),pix.height());

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, name_);
  paint.end();

  QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);

  setPixmap(pix);
}

bool
FanoutNode::append(MarSystemNode* newTail)
{
  if(CompositeNode::append(newTail)) { //Check that it was new
    newTail->setPrev(NULL);
    newTail->setNext(NULL);
    placeAllWidgets();//Resize the fanout to include this
    newTail->show();
    if(newTail->isCollection()) {
      connect(newTail,SIGNAL(resized(int,int,int,int)),
              this,SLOT(handleChildResize(int,int,int,int)));
    }
    return true;
  }
  return false;
}

bool
FanoutNode::insert(int index,MarSystemNode* newWidget)
{
  if(CompositeNode::insert(index,newWidget)) { //Check it was new
    newWidget->setPrev(NULL);
    newWidget->setNext(NULL);
    placeAllWidgets();//Resize the fanout to include this
    newWidget->show();
    return true;
  }
  return false;
}

bool
FanoutNode::insert(MarSystemNode* before,MarSystemNode* newWidget)
{
  if(CompositeNode::insert(before,newWidget)) { //Check it was new
    newWidget->setPrev(NULL);
    newWidget->setNext(NULL);
    placeAllWidgets();//Resize the fanout to include this
    newWidget->show();
    return true;
  }
  return false;
}

QWidget*
FanoutNode::getChildrenCanvas()
{
  return this;
}

/**
   places all the Widgets
*/
void
FanoutNode::placeAllWidgets()
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine, name_);

  int maxX=width();
  int maxY=height();
  vector<MarSystemNode*>::iterator itr;
  MarSystemNode* prev = NULL;
  for(itr=nodes_.begin(); itr!=nodes_.end(); itr++) {
    //Move the MarSYstemNode
    if((*itr)->x()<0) {
      (*itr)->move(5,(*itr)->y());
    }
    if(prev!=NULL && (*itr)->x()<prev->x()+prev->width()) {
      (*itr)->move(prev->x()+prev->width()+5,(*itr)->y());
    }
    //Stretch the Width
    if((*itr)->x()+(*itr)->width()+5>maxX) {
      maxX=(*itr)->x()+(*itr)->width()+5;
    }
    //move the MarSystemNode Down
    if((*itr)->y()<size.height()+10) {
      (*itr)->move((*itr)->x(),size.height()+10);
    }
    //Stretch the COntatiner
    if(((*itr)->getBottom()+10)>maxY) {
      maxY+=(*itr)->getBottom()+10;
    }
    prev=(*itr);
  }
  //Actually Provide any streching required
  resize(maxX,maxY);
}

void
FanoutNode::resizeEvent(QResizeEvent*)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  QPixmap pix(width(),height());

  QPainter paint;
  paint.begin(&pix);
  paint.setBrush(Qt::white);
  paint.setPen(Qt::black);
  paint.drawRect(0,0,pix.width()-1,pix.height()-1);

  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, name_);
  paint.end();

  QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);

  setPixmap(pix);
  update();
  emit resized(x(),y(),width(),height());
}

void
FanoutNode::paintEvent(QPaintEvent*)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  QPainter paint(this);
  paint.setBrush(Qt::NoBrush);
  paint.setPen(Qt::red);
  paint.drawLine(width()/2,0,width()/2,size.height()+6);
  if(nodes_.size()>0) {
    MarSystemNode* first = nodes_.front();
    MarSystemNode* last = nodes_.back();
    paint.drawLine(first->getCenter(),size.height()+6,last->getCenter(),size.height()+6);
    vector<MarSystemNode*>::iterator itr;
    for(itr=nodes_.begin(); itr!=nodes_.end(); itr++) {
      paint.drawLine((*itr)->getCenter(),size.height()+6,(*itr)->getCenter(),(*itr)->y());
      paint.drawLine((*itr)->getCenter(),(*itr)->getBottom(),(*itr)->getCenter(),height()-5);
    }
    paint.drawLine(first->getCenter(),height()-5,last->getCenter(),height()-5);
  }
  paint.drawLine(width()/2,height()-5,width()/2,height());
}

void
FanoutNode::handleChildResize(int x,int y,int w,int h)
{
  //TODO is this all?
  placeAllWidgets();
}
