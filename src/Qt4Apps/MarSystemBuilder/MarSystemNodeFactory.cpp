/**********************************
MarSystemNodeFactory Implementation File
***********************************/
#include "MarSystemNodeFactory.h"

MarSystemNodeFactory* MarSystemNodeFactory::instance=NULL;
string MarSystemNodeFactory::GAIN_TYPE="Gain";
string MarSystemNodeFactory::FANOUT_TYPE="Fanout";
string MarSystemNodeFactory::SERIES_TYPE = "Series";

MarSystemNodeFactory::MarSystemNodeFactory()
{
  msManager= new MarSystemManager();
}

MarSystemNodeFactory*
MarSystemNodeFactory::getInstance()
{
  if(MarSystemNodeFactory::instance==0)
    MarSystemNodeFactory::instance = new MarSystemNodeFactory();
  return MarSystemNodeFactory::instance;
}

MarSystemNode*
MarSystemNodeFactory::newNode(string type,QString name,QWidget* parent)
{
  MarSystemNode* newWidget=NULL;
  if(type == MarSystemNodeFactory::GAIN_TYPE) {
    newWidget = new GainNode(name,parent);
  } else if(type == MarSystemNodeFactory::FANOUT_TYPE) {
    newWidget = new FanoutNode(name,parent);
  } else if(type == MarSystemNodeFactory::SERIES_TYPE) {
    newWidget = new SeriesNode(name,parent);
  } else {
    cout<<type<<endl;
    //Create an abstract MarSystemNode Should this exist?
    MarSystem* msys = msManager->getPrototype(type);
    //TODO Create a New MarSysteMnode that knows how to draw itself.
    if(msys!=NULL) {
      newWidget = new DefaultMarSystemNode(type,name,parent);
    }
  }
  return newWidget;
}

MarSystemNode*
MarSystemNodeFactory::newNode(MarSystem* msys,QWidget* parent,bool isComposite)
{
  MarSystemNode* newWidget=NULL;
  if(msys!=NULL) {
    string type = msys->getType();

    if(isComposite) {
      if(type == MarSystemNodeFactory::FANOUT_TYPE) {
        newWidget = new FanoutNode(dynamic_cast<MarSystem*>(msys),parent);
      } else if(type == MarSystemNodeFactory::SERIES_TYPE) {
        newWidget = new SeriesNode(dynamic_cast<MarSystem*>(msys),parent);
      } else {
        newWidget = new DefaultCompositeNode(dynamic_cast<MarSystem*>(msys),parent);
      }
    } else {
      if(type == MarSystemNodeFactory::GAIN_TYPE) {
        newWidget = new GainNode(msys,parent);
      } else {
        newWidget = new DefaultMarSystemNode(msys,parent);
      }
    }
  }
  return newWidget;
}
