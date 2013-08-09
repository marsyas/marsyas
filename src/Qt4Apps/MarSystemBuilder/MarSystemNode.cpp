/*
  MarSystemNode.cpp - Implementation File

  Author:Peter Minter
  Date Created: October 24 2005
*/
#include <iostream>
#include "MarSystemNode.h"

/******************************
 Constructor
 In the abstract case all we know is the type and name + the canvas to
 paint the object on.
******************************/
MarSystemNode::MarSystemNode(string type,QString name,QWidget * parent)
  :QWidget(parent)
{
  name_ = QString(type.c_str()).append(QString("::")).append(name);
  type_ = QString(type.c_str());

  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  // Default Location when a Widget is created... Not really that important
  setGeometry(((parent->width()/2)-(width()/2)),10,
              size.width()+13,size.height()+12);

  prev_=0;//Even if parent is null set the parent to it
  next_=0;//We don't know of any children yet
}

/**
 *  Constructor
 *
 */
MarSystemNode::MarSystemNode(MarSystem* msys,QWidget* parent)
  :QWidget(parent)
{
  name_ = QString(msys->getType().c_str()).append(QString("::")).
          append(QString(msys->getName().c_str()));
  type_ = QString(msys->getType().c_str());

  represents_ = msys;

  QFontMetrics fm(font());
  QSize size = fm.size(Qt::TextSingleLine,name_);

  // Default Location when a Widget is created... Not really that important
  setGeometry(((parent->width()/2)-(width()/2)),10,
              size.width()+13,size.height()+12);

  prev_=0;//Even if parent is null set the parent to it
  next_=0;//We don't know of any children yet
}


/*****************************
  Destructor remove all references to the pointer created in the first
  place.
  Simple remove from doubly linked list.
 *****************************/
MarSystemNode::~MarSystemNode()
{
  if(prev_!=0)
    prev_->setNext(getNext());
  if(next_!=0)
    next_->setPrev(getPrev());
}

/******************************
 When a MarSystemNode is clicked it creates a drag event.
 Perhaps we will also have to add actions for double clicking to load
 something else.
 The drag assumes that you have set a pixmap for the drag item.  The
 background should bet set to a pixmap.
 ******************************/
void
MarSystemNode::mousePressEvent(QMouseEvent *event)
{
  //All MarSystemNodes are draggable at the moment. SO this will be the
  //behavior should one be dragged.
  QByteArray widgetData;
  QDataStream dataStream(&widgetData,QIODevice::ReadWrite);
  dataStream<<name_;
  dataStream<<QPoint(event->pos() - rect().topLeft());

  QMimeData *mime = new QMimeData;
  mime->setData("application/x-MarSystemNode",widgetData);

  QDrag * drag = new QDrag(this);
  drag->setMimeData(mime);
  drag->setHotSpot(event->pos() - rect().topLeft());
  // drag->setPixmap(*pixmap_);
  // hide();

  if(drag->start(Qt::MoveAction) == Qt::MoveAction)
    close();
  else
    show();
}




/*****************************
 Get/Set Prev.
 *****************************/
MarSystemNode*
MarSystemNode::getPrev()
{
  return prev_;
}
void
MarSystemNode::setPrev(MarSystemNode* parent)
{
  prev_=parent;//->getParentForChild();
}

/******************************
  Get/Set Next.
 ******************************/
MarSystemNode*
MarSystemNode::getNext()
{
  return next_;
}

/**
 * Sets the Next Node in order to draw connections
*/
void
MarSystemNode::setNext(MarSystemNode* child)
{
  next_=child;
}

/******************************
  Shortcut function for getting center of Widget
******************************/
int
MarSystemNode::getBottom()
{
  return y()+height();
}

/******************************
  Shortcut function for getting bottom of Widget.
******************************/
int
MarSystemNode::getCenter() const
{
  return x()+(width()/2);
}

/**
 * Returns whether this is a composite Node or not
 */
bool
MarSystemNode::isCollection()
{
  return false;
}

/**
   Setting the name of the widget.
   I am pretty sure most nodese will have to resize themselves but
   this will be the default
*/
void
MarSystemNode::setName(const QString& name)
{
  name_ = QString(type_).append(QString("::")).append(name);
}

/**
 * Returns the name of the Node
*/
QString
MarSystemNode::getName() const
{
  return name_;
}

void
MarSystemNode::setPixmap(QPixmap pixmap)
{
  pixmap_=new QPixmap(pixmap);
}

QPixmap*
MarSystemNode::getPixmap()
{
  return pixmap_;
}

MarSystem*
MarSystemNode::getSystem()
{
  return represents_;
}

