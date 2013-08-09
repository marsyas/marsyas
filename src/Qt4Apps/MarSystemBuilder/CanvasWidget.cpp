/**
 * CanvasWidget Implmentation file
 * This class is a representation of classes that place object inside
 * themselves and draw the attachments
 */

#include "CanvasWidget.h"

CanvasWidget::CanvasWidget()
{
  setAcceptDrops(true);
}

CanvasWidget::CanvasWidget(QWidget* parent)
  :QWidget(parent)
{
  setAcceptDrops(true);
}

void
CanvasWidget::paintEvent(QPaintEvent* event)
{
  cout << "CanvasWidget::PaintEvent called" << endl;
  if(paintWidget!=NULL) {
    cout << "Painting " << endl;
    QPainter painter(this);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::white);



    if(paintWidget->getPrev() !=0) {
      cout << "There is previous " << endl;

      painter.drawLine(paintWidget->getPrev()->getCenter(),
                       paintWidget->getPrev()->getBottom(),
                       paintWidget->getCenter(),
                       paintWidget->y());
    }
    if(paintWidget->getNext() !=0) {
      cout << "There is next " << endl;
      painter.drawLine(paintWidget->getCenter(),
                       paintWidget->getBottom(),
                       paintWidget->getNext()->getCenter(),
                       paintWidget->getNext()->y());
    }
    paintWidget=0;
  }
}

/**
 * Default Behaviour for handleing dragged MarSystemNodes
 */
void
CanvasWidget::dragEnterEvent(QDragEnterEvent *event)
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
 * Default Behavior for handling dragging of MarSystemNodes
 */
void
CanvasWidget::dragMoveEvent(QDragMoveEvent *event)
{
  if(event->mimeData()->hasFormat("application/x-MarSystemNode")) {
    if(children().contains(event->source())) {
      //set the widget to draw the lines to.
      paintWidget=(MarSystemNode*)event->source();

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
                       width(),paintWidget->y()-paintWidget->getPrev()->getBottom()));
        } else if(paintWidget->getNext() !=0) {
          update(QRect(0,paintWidget->getBottom(),
                       width(),paintWidget->getNext()->y()-paintWidget->getBottom()));
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
CanvasWidget::dropEvent(QDropEvent *event)
{
  //Only accept MarSystemNodes that started in this box
  if(event->mimeData()->hasFormat("application/x-MarSystemNode")
      && children().contains(event->source()))
  {

    QByteArray itemData =
      event->mimeData()->data("application/x-MarSystemNode");

    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    QString widgetName;
    QPoint offset;

    dataStream >> widgetName >> offset;

    cout << "widgetName = " << widgetName.toStdString() << endl;

    //Get a reference to a MarSystemNode that is being moved about
    MarSystemNode *newMarSystemNode= (MarSystemNode *)event->source();


    //Move the eidget to it's new location
    newMarSystemNode->move(event->pos()-offset);

    //Drop the object here and show it.
    event->setDropAction(Qt::TargetMoveAction);
    event->accept();
    newMarSystemNode->setParent(this);
    newMarSystemNode->show();
    newMarSystemNode->update();
    update();

    //set the widget to draw the lines to.
    paintWidget=newMarSystemNode;

    if(paintWidget->getPrev() !=0 && paintWidget->getNext()!=0)
    {
      update();
      /* update(QRect(0,paintWidget->getNext()->getBottom(),
           width(),
           paintWidget->getPrev()->y()-paintWidget->getNext()->getBottom()));
      */
    }
    else if (paintWidget->getNext()!=0)
    {
      update();
      /* update(QRect(0,paintWidget->getNext()->getBottom(),
         width(),paintWidget->y()-paintWidget->getNext()->getBottom())); */
    }
    else if (paintWidget->getPrev()!=0) {
      update();
      /* update(QRect(0,paintWidget->getBottom(),
         width(),painWidget->getPrev()->y() - paintWidget->getBottom())); */
    }
    else
      update();
    return;
  }
  event->ignore();
}

void
CanvasWidget::drawAttachmentsFor(MarSystemNode* widget)
{
  paintWidget=widget;
  update(getDimensionsFor(widget));
}

/**
 * TODO REVAMP THIS FUNCTION TO MAKE IT WORK CORRECTLY
*/
QRect
CanvasWidget::getDimensionsFor(MarSystemNode* widget)
{
  MarSystemNode* theParent = widget->getPrev();
  MarSystemNode* theChild = widget->getNext();

  int minX=0;
  int maxWidth=0;
  int minY=0;
  int maxHeight=0;
  //if(theParent != NULL && theChild != NULL){
  //  minX=minimum(theParent->x(),theChild->x(),widget->x());
  //  minY=theParent->y(),
  //  maxWidth=maximum(theParent->x()+theParent->width(),
  //		      theChild->x()+theChild->width(),
  //		      widget->x()+widget->width())-minX;
  //  maxHeight=theChild->y()-minY;
  /*}else*/ if(theParent != NULL) {
    minX =minimum(widget->x(),theParent->x());
    minY=theParent->getBottom();
    maxWidth=maximum(widget->x()+widget->width(),
                     theParent->x()+theParent->width())-minX;
    maxHeight=widget->y()-minY;
  }//else if(theChild != NULL){
  // minX=minimum(widget->x(),theChild->x());
  // minY=widget->y();
  // maxWidth=maximum(theChild->x()+theChild->width(),
  //		     widget->x()+widget->width())-minX;
  //  maxHeight=theChild->y()-minY;
  //}//else there shouldn't be anything to update
  std::cout<<"QRect "<<minX<<","<<minY<<","<<maxWidth<<","<<maxHeight<<std::endl;
  return QRect(minX,minY,maxWidth,maxHeight);
}

int
CanvasWidget::maximum(int a,int b,int c)
{
  if(a>b) {
    if(a>c)
      return a;
    else
      return c;
  } else {
    if(b>c)
      return b;
    else
      return c;
  }
}

int
CanvasWidget::minimum(int a,int b,int c)
{
  if(a<b) {
    if(a<c)
      return a;
    else
      return c;
  } else {
    if(b<c)
      return b;
    else
      return c;
  }
}
