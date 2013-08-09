/**
 * SeriesNode.cpp - IMplementation Dile
 * See SeriesNode.h
 */
#include "SeriesNode.h"

SeriesNode::SeriesNode(QString name,QWidget* parent)
  :CompositeNode(MarSystemNodeFactory::SERIES_TYPE,name,parent)
{

  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  setGeometry((parent->width()-width())/2,10,
              size.width()+12,size.height()+12);

  QPixmap pix(width(),height());

  QPainter paint;
  paint.begin(this);
  //painter.setRenderHint(QPainter::Antialiasing);
  paint.setBrush(Qt::white);
  paint.setPen(Qt::black);
  paint.drawRect(0,0,pix.width()-1,pix.height()-1);

  //QFont font = font();
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

SeriesNode::SeriesNode(MarSystem* msys, QWidget* parent)
  :CompositeNode(msys,parent)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  setGeometry((parent->width()-width())/2,10,
              size.width()+12,size.height()+12);

  QPixmap pix(width(),height());

  QPainter paint;
  paint.begin(this);
  //painter.setRenderHint(QPainter::Antialiasing);
  paint.setBrush(Qt::white);
  paint.setPen(Qt::black);
  paint.drawRect(0,0,pix.width(),pix.height());

  //QFont font = font();
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
 *Appends a MarSystemNode and handles all the Parent Child Logic
 *
 */
bool
SeriesNode::append(MarSystemNode* newWidget)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);
  if(CompositeNode::append(newWidget)) {
    if(nodes_.size()==1) {
      newWidget->setPrev(0);
      newWidget->move(newWidget->x(),size.height()+10);
    } else {
      //TODO catch exception if there was no node there
      MarSystemNode* oldLast = nodes_.at(nodes_.size()-2);
      oldLast->setNext(newWidget);;
      newWidget->setPrev(oldLast);
      newWidget->move(oldLast->getCenter()-(newWidget->width()/2),newWidget->y());
    }
    //TODO is this step nesscary
    drawAllWidgets();
    //Show the newly added widget
    newWidget->show();
    if(newWidget->isCollection()) {
      connect(newWidget,SIGNAL(resized(int,int,int,int)),
              this,SLOT(handleChildResize(int,int,int,int)));
    };
    return true;
  }
  return false;
}

/**
 *inserts a MarSystemNode at the location given
 *Returns true if the addition was a success
 */
bool
SeriesNode::insert(int index,MarSystemNode* newWidget)
{
  if(CompositeNode::insert(index,newWidget)) {
    if(index==0) { //Added to the first slot
      if(nodes_.size()>1) {
        MarSystemNode* oldFirst = nodes_.at(1);
        oldFirst->setPrev(newWidget);
        newWidget->setNext(oldFirst);
        newWidget->setPrev(0);
      }
    } else { //Not the first
      if(nodes_.size()>(index+1)) {
        //UPdate Connections
        MarSystemNode* newParent = nodes_.at(index-1);
        MarSystemNode* newChild = nodes_.at(index+1);
        newParent->setNext(newWidget);
        newWidget->setPrev(newParent);
        newWidget->setNext(newChild);
        newChild->setPrev(newWidget);
      }
    }
    drawAllWidgets();
    return true;
  }
  return false;
}

/**
 *Inserts before some other MarSystemNode in list
 *Returns True if the MarSystemNode is added without problem. false otherwise.
 */
bool
SeriesNode::insert(MarSystemNode* before,MarSystemNode* newWidget)
{
  if(CompositeNode::insert(before,newWidget)) {
    vector<MarSystemNode*>::iterator itr = nodes_.begin();
    int i=0;
    for(; itr!=nodes_.end(); itr++) {
      if(*itr==before)break;
      i++;
    }
    //TODO handle the case if we just added the first
    if(i>1) {
      newWidget->setPrev(before->getPrev());
    } else {
      newWidget->setPrev(0);
    }
    before->setPrev(newWidget);//TODO this may not be possible
    newWidget->setNext(before);
    drawAllWidgets();
    return true;
  }
  return false;
}

/**
 * UPdates the size of the
 */
void
SeriesNode::drawAllWidgets()
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);
  int maxX =width();
  int maxY =height();
  vector<MarSystemNode*>::iterator itr = nodes_.begin();
  //Begin can't use the bottom of his parent because that number is
  //greatly overestimated
  for(; itr!=nodes_.end(); itr++) {
    //adjust x()
    if((*itr)->x()<5) {
      (*itr)->move(5,(*itr)->y());
    }
    if((*itr)->width()+(*itr)->x()+5 > maxX) {
      maxX = (*itr)->width()+(*itr)->x()+5;
    }
    //adjust y()
    MarSystemNode* parent = (*itr)->getPrev();
    if(parent != NULL) {
      int minY= parent->getBottom()+10;
      if((*itr)->y()<minY) {
        (*itr)->move((*itr)->x(),minY);
      }
    }
    if((*itr)->getBottom()+10>maxY) {
      maxY =(*itr)->getBottom()+10;
    }
  }
  resize(maxX,maxY);
  update();
}

/**
 * Tries to draw all the attachments if there is no paint widget
 */
void
SeriesNode::paintEvent(QPaintEvent*)
{
  MarSystemNode* last=NULL;
  QPainter painter(this);
  painter.setPen(Qt::red);

  if(nodes_.size()>0) {
    vector<MarSystemNode*>::iterator itr = nodes_.begin();
    painter.drawLine(width()/2,0,
                     (*itr)->getCenter(),(*itr)->y());
    last=(*itr);
    itr++;

    painter.setPen(Qt::black);
    for(; itr!=nodes_.end(); itr++) {
      MarSystemNode* parent = (*itr)->getPrev();
      if(parent!=NULL) {
        painter.drawLine(parent->getCenter(),parent->getBottom(),
                         (*itr)->getCenter(),(*itr)->y());
      }
      last=(*itr);
    }

    painter.setPen(Qt::red);
    painter.drawLine(last->getCenter(),last->getBottom(),
                     width()/2,height());
  } else {
    painter.drawLine(width()/2,0,width()/2,height());
  }
}


/**
 * TODO add logic that does things related to children
*/
void
SeriesNode::resizeEvent(QResizeEvent*)
{
  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  QPixmap pix(width(),height());

  QPainter paint;
  paint.begin(&pix);
  //painter.setRenderHint(QPainter::Antialiasing);
  paint.setBrush(Qt::white);
  paint.setPen(Qt::black);
  paint.drawRect(0,0,pix.width()-1,pix.height()-1);

  //QFont font = font();
  paint.setFont(font());
  paint.setPen(Qt::black);
  paint.setBrush(Qt::NoBrush);
  paint.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, name_);
  paint.end();

  QPalette pal(palette());
  pal.setBrush(backgroundRole(),QBrush(pix));
  setPalette(pal);

  setPixmap(pix);
  emit resized(x(),y(),width(),height());
}

QWidget*
SeriesNode::getChildrenCanvas()
{
  return this;
}

/**
 * Default Behaviour for handleing dragged MarSystemNodes
 */
void
SeriesNode::dragEnterEvent(QDragEnterEvent *event)
{
  if(event->mimeData()->hasFormat("application/x-MarSystemNode")) {
    if(children().contains(event->source())) {
      event->setDropAction(Qt::MoveAction);
      event->accept();
    } else {
      event->ignore();
    }
  } else {
    event->ignore();
  }
}

/**
 * Default SeriesNode for handling dragging of MarSystemNodes
 * Need to make this Node resize if the Cell is trying to out draw the box
 */
void
SeriesNode::dragMoveEvent(QDragMoveEvent *event)
{
  if(event->mimeData()->hasFormat("application/x-MarSystemNode")) {
    if(children().contains(event->source())) {
      //set the widget to draw the lines to.
      MarSystemNode* paintWidget=(MarSystemNode*)event->source();

      if(true) {
        event->setDropAction(Qt::MoveAction);
        event->accept();

        QByteArray itemData =
          event->mimeData()->data("application/x-MarSystemNode");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QString widgetName;
        QPoint offset;

        dataStream >> widgetName >> offset;

        paintWidget->move(event->pos()-offset);
        if(paintWidget->getPrev() !=0 && paintWidget->getNext()!=0) {
          update(QRect(0,paintWidget->getPrev()->getBottom(),
                       width(),
                       paintWidget->getNext()->y()
                       -paintWidget->getPrev()->getBottom()));
        } else if(paintWidget->getPrev() !=0) {
          update(QRect(0,paintWidget->getPrev()->getBottom(),
                       width(),height()-paintWidget->getPrev()->getBottom()));
        } else if(paintWidget->getNext() !=0) {
          //This is first
          update(QRect(0,0,
                       width(),paintWidget->getNext()->y()));
        }
        //do not carry out final statement
        return;
      }
    }
  }
  //if any of the tests fail do not use the event
  event->ignore();
}

/**
 * Default code for handling dropped MarSystemNodes
 */
void
SeriesNode::dropEvent(QDropEvent *event)
{
  //Only accept MarSystemNodes that started in this box
  if(event->mimeData()->hasFormat("application/x-MarSystemNode")
      && children().contains(event->source())) {

    QByteArray itemData =
      event->mimeData()->data("application/x-MarSystemNode");

    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    QString widgetName;
    QPoint offset;

    dataStream >> widgetName >> offset;

    //Get a reference to a MarSystemNode that is being moved about
    MarSystemNode *newMarSystemNode= (MarSystemNode *)event->source();

    //Move the eidget to it's new location
    newMarSystemNode->move(event->pos()-offset);

    //Drop the object here and show it.
    event->setDropAction(Qt::TargetMoveAction);
    event->accept();
    newMarSystemNode->show();

    //set the widget to draw the lines to.
    MarSystemNode* paintWidget=newMarSystemNode;

    if(paintWidget->getPrev() !=0 && paintWidget->getNext()!=0) {
      update(QRect(0,paintWidget->getPrev()->getBottom(),
                   width(),
                   paintWidget->getNext()->y()-paintWidget->getPrev()->getBottom()));
    } else if(paintWidget->getPrev()!=0) {
      //assumes that this is the last one
      update(QRect(0,paintWidget->getPrev()->getBottom(),
                   width(),height()-paintWidget->getPrev()->getBottom()));
    } else if(paintWidget->getNext()!=0) {
      //assumes this is the first one
      update(QRect(0,0,
                   width(),paintWidget->getNext()->y()));
    }
    return;
  }
  event->ignore();
}

void
SeriesNode::handleChildResize(int x,int y,int w,int h)
{
  drawAllWidgets();
}
