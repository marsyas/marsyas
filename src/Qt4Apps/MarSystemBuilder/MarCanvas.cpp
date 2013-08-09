/*
  MarCanvas.cpp - Implementation File

  Author: Peter Minter
  Date Created: October 24 2005
*/

#include "MarCanvas.h"
#include "DefaultMarSystemNode.h"

/******************
 *Constructor
 * Sets Background
 * Sets up the MarSystemNode Holder
 *******************/
MarCanvas::MarCanvas(QWidget * parent)
  :CanvasWidget(parent)
{
  cout << "MarCanvas::MarCanvas" << endl;
  setGeometry(0,0,parent->width(),parent->height());
  setPalette(QColor(0,0,0));
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);


  MarSystemNode *myMar = new GainNode("test", parent);
  paintWidget=myMar;
  connect(paintWidget,SIGNAL(resized(int,int,int,int)),this,SLOT(handleChildResize(int,int,int,int)));

  paintWidget->show();
  lastWidget=myMar;

  /* ifstream stream ("/home/pminter/code/c++/marsyas-0.2/src/plugins/playback.mpl",fstream::in);

  MarSystemNode *myMar = loadMarSystem(stream,0);;



  saveMarSystem("/home/pminter/code/c++/MarSystemBuilder/test.mpl");
  */
  cout << "end of MarCanvas::MarCanvas" << endl;
}

/*************************
 * this is Pretty much all the individual stuff left unless I need to
 * override\
 TODO this function needs huge revamp currently hacked together
**************************/
void
MarCanvas::addNewMarSystemNode(QString widgetType)
{
  cout << "Adding " << widgetType.toStdString() << endl;

  /**Add Functionality Here to specify the right type of MarSystemNode
   Probably use a factory*/
  MarSystemManager* manager = new MarSystemManager();
  MarSystem* msys = manager->create(widgetType.toStdString(), "default" + widgetType.toStdString()); // getPrototype(widgetType.toStdString());
  MarSystemNodeFactory* factory = MarSystemNodeFactory::getInstance();
  // msys->setName("Default");//TODO get this infor somewhere

  cout << "Created stuff" << endl;



  if(!lastWidget->isCollection()) {
    cout << "Not a collection adding" << endl;
    cout << *msys << endl;
    MarSystemNode *newMarSystemNode =  factory->newNode(msys,this);
    newMarSystemNode->show();
    cout << "after showing" << endl;
    lastWidget = newMarSystemNode;
    paintWidget=lastWidget;//be sure to do this before update since it
    //is required.



    cout << "before updating " << endl;
    update();
    /*  update(QRect(0,paintWidget->getPrev()->getBottom(),
         width(),paintWidget->y()));
    */




  } else {
    MarSystemNode *newMarSystemNode =
      factory->newNode(msys,
                       (dynamic_cast<CompositeNode*>(lastWidget))->getChildrenCanvas());
    (dynamic_cast<CompositeNode*>(lastWidget))->append(newMarSystemNode);
  }
}

/**
 * Default Behaviour for handleing dragged MarSystemNodes
 */
void
MarCanvas::dragEnterEvent(QDragEnterEvent *event)
{
  cout << "DragEnterEvent" << endl;
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
MarCanvas::dragMoveEvent(QDragMoveEvent *event)
{
  cout << "dragMoveEvent" << endl;

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
        // update();
        /* if(paintWidget->getPrev() !=0 && paintWidget->getNext()!=0){
          update(QRect(0,paintWidget->getPrev()->getBottom(),
        	       width(),
        	       paintWidget->getNext()->y()
        	        -paintWidget->getPrev()->getBottom()));
        }else if(paintWidget->getPrev() !=0){
          update(QRect(0,paintWidget->getPrev()->getBottom(),
        	width(),paintWidget->y()-paintWidget->getPrev()->getBottom()));
        }else if(paintWidget->getNext() !=0){
          update(QRect(0,paintWidget->getBottom(),
                width(),paintWidget->getNext()->y()-paintWidget->getBottom()));
        }
        */
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
MarCanvas::dropEvent(QDropEvent *event)
{

  cout << "*********** MARCANVAS DROP EVENT ******" << endl;

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

    paintWidget->move(event->pos()-offset);

    cout << "widgetName = " << widgetName.toStdString() << endl;

    //Drop the object here and show it.
    // event->setDropAction(Qt::TargetMoveAction);
    // event->accept();

    return;
  }
  event->ignore();
}


void
MarCanvas::paintEvent(QPaintEvent* event)
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
    paintWidget->update();
    paintWidget->show();
    // paintWidget=0;
  }
}




/**
 * Loads all the MarSystemNodes into this MarCanvas
 * First Removes all MarSystemNodes Currently in the window.
 * Returns a MarSystemNode* because it is recursive.
 */
MarSystemNode*
MarCanvas::loadMarSystem(istream& is, QWidget* canvas)
{
  string skipstr;
  mrs_natural i;
  //skip #
  is >> skipstr;
  string mcomposite;
  bool   isComposite;

  is >> mcomposite;
  string marSystem = "MarSystem";
  string marSystemComposite = "MarSystemComposite";
  if (mcomposite == marSystem)
    isComposite = false;
  else if (mcomposite == marSystemComposite)
    isComposite = true;
  else
    return 0;

  //Skip #/type/=
  is >> skipstr >> skipstr >> skipstr;
  string mtype;
  is >> mtype;
  //Skip #/name/=
  is >> skipstr >> skipstr >> skipstr;
  string mname;
  is >> mname;

  MarSystemManager* man = new MarSystemManager();
  MarSystem* msys = man->getPrototype(mtype);

  if (msys == 0) {
    MRSWARN("MarSystem::getMarSystem - MarSystem not supported");
    return 0;
  } else {
    msys->setName(mname);
    // is >> (msys->ncontrols_);
    msys->update();

    //Don't think this is important for my work
    //workingSet[msys->getName()] = msys; // add to workingSet

    MarSystemNodeFactory* factory = MarSystemNodeFactory::getInstance();
    MarSystemNode* newMarSystemNode;
    if(canvas!=0) {
      newMarSystemNode =factory->newNode(msys,canvas,isComposite);
    } else {
      newMarSystemNode =factory->newNode(msys,this,isComposite);
    }

    //Skip #/Number/of
    is >> skipstr >> skipstr >> skipstr;
    is >> skipstr;

    if (skipstr != "links") {
      cout << "Problem with reading links" << endl;
      cout << "skipstr = " << skipstr << endl;
      cout << "mtype = " << mtype << endl;
      cout << "mname = " << mname << endl;
    }

    //skip =
    is >> skipstr;

    mrs_natural nLinks;
    is >> nLinks;

    //Handle All Links for the MarSystem
    for (i=0; i < nLinks; i++) {
      is >> skipstr >> skipstr >> skipstr;
      string visible;
      is >> visible;
      is >> skipstr;

      is >> skipstr >> skipstr >> skipstr >> skipstr >> skipstr;

      mrs_natural nSynonyms;
      is >> nSynonyms;

      vector<string> synonymList;
      // synonymList = msys->synonyms_[visible];
      for (int j=0; j < nSynonyms; j++) {
        string inside;
        is >> skipstr;
        is >> inside;
        synonymList.push_back(inside);
        // msys->synonyms_[visible] = synonymList;
      }
    }

    if (isComposite == true) {
      is >> skipstr >> skipstr >> skipstr;
      mrs_natural nComponents;
      is >> nComponents;
      for (i=0; i < nComponents; i++) {
        MarSystemNode* childWidget =
          loadMarSystem(is,
                        dynamic_cast<CompositeNode*>(newMarSystemNode)->getChildrenCanvas());
        if (childWidget == 0)
          return 0;
        dynamic_cast<CompositeNode*>(newMarSystemNode)->append(childWidget);
      }
      //TODO Make update work for me
      newMarSystemNode->update();
      msys->update();
    }
    return newMarSystemNode;;
  }
}

void
MarCanvas::handleChildResize(int x,int y,int w,int h)
{
  if(x+w>width())
    resize(x+w+20,height());
  if(y+h>height())
    resize(width(),y+h+20);
}

void
MarCanvas::saveMarSystem(string filename)
{
  MarSystem* system = lastWidget->getSystem();
  ofstream stream (filename.c_str(),fstream::out);
  stream<<*system;
}


/**
 * Remove all Widgets from the Cancas and paint the new ones.
*/
void
MarCanvas::loadFromFile(string filename)
{
  if(lastWidget!=NULL)
    delete(lastWidget);

  ifstream stream (filename.c_str(),fstream::in);
  MarSystemNode *myMar = loadMarSystem(stream,0);
  myMar->show();
  if(myMar->isCollection()) {
    connect(myMar,SIGNAL(resized(int,int,int,int)),
            this,SLOT(handleChildResize(int,int,int,int)));
  }
  paintWidget=myMar;
  lastWidget=myMar;
}
