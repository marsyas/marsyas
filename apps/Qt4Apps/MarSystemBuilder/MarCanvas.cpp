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
  setGeometry(0,0,parent->width(),parent->height());
  setPalette(QColor(0,0,0));
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

  ifstream stream ("/home/pminter/code/c++/marsyas-0.2/src/plugins/playback.mpl",fstream::in);

  MarSystemNode *myMar = loadMarSystem(stream,0);;
  myMar->show();
  connect(myMar,SIGNAL(resized(int,int,int,int)),this,SLOT(handleChildResize(int,int,int,int)));
  paintWidget=myMar;
  lastWidget=myMar;
  saveMarSystem("/home/pminter/code/c++/MarSystemBuilder/test.mpl");
  
}

/*************************
 * this is Pretty much all the individual stuff left unless I need to
 * override\
 TODO this function needs huge revamp currently hacked together
**************************/
void 
MarCanvas::addNewMarSystemNode(QString widgetType)
{
  /**Add Functionality Here to specify the right type of MarSystemNode
   Probably use a factory*/
  MarSystemManager* manager = new MarSystemManager();
  MarSystem* msys = manager->getPrototype(widgetType.toStdString());
  MarSystemNodeFactory* factory = MarSystemNodeFactory::getInstance();
  msys->setName("Default");//TODO get this infor somewhere

  if(!lastWidget->isCollection()){
  MarSystemNode *newMarSystemNode =
    factory->newNode(msys,this);
    newMarSystemNode->show();
    lastWidget = newMarSystemNode;
    paintWidget=lastWidget;//be sure to do this before update since it
			   //is required.
    update(QRect(0,paintWidget->getPrev()->getBottom(),
	       width(),paintWidget->y()));
  }else{
  MarSystemNode *newMarSystemNode =
    factory->newNode(msys, 
		     (dynamic_cast<CompositeNode*>(lastWidget))->getChildrenCanvas());
    (dynamic_cast<CompositeNode*>(lastWidget))->append(newMarSystemNode);
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
  
  if (msys == 0){
    MRSWARN("MarSystem::getMarSystem - MarSystem not supported");
    return 0;
  }else{
    msys->setName(mname);
    // is >> (msys->ncontrols_);
    msys->update();
      
    //Don't think this is important for my work
    //workingSet[msys->getName()] = msys; // add to workingSet
      
    MarSystemNodeFactory* factory = MarSystemNodeFactory::getInstance();
    MarSystemNode* newMarSystemNode;
    if(canvas!=0){
      newMarSystemNode =factory->newNode(msys,canvas,isComposite);
    }else{
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
    for (i=0; i < nLinks; i++){
      is >> skipstr >> skipstr >> skipstr;
      string visible;
      is >> visible;
      is >> skipstr;
      
      is >> skipstr >> skipstr >> skipstr >> skipstr >> skipstr;
      
      mrs_natural nSynonyms;
      is >> nSynonyms;
      
      vector<string> synonymList;
      // synonymList = msys->synonyms_[visible];
      for (int j=0; j < nSynonyms; j++){
	string inside;
	is >> skipstr;
	is >> inside;
	synonymList.push_back(inside);
	// msys->synonyms_[visible] = synonymList;
      }
    }
    
    if (isComposite == true){
      is >> skipstr >> skipstr >> skipstr;
      mrs_natural nComponents;
      is >> nComponents;
      for (i=0; i < nComponents; i++){
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
  if(myMar->isCollection()){
    connect(myMar,SIGNAL(resized(int,int,int,int)),
	    this,SLOT(handleChildResize(int,int,int,int)));
  }
  paintWidget=myMar;
  lastWidget=myMar;
}
