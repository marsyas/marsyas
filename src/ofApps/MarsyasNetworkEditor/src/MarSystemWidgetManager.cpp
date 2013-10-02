//
//  MarSystemWidgetManager.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/1/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "MarSystemWidgetManager.h"
#include <marsyas/system/MarSystemManager.h>
#include "MarSystemWidget.h"
#include "SeriesWidget.h"
#include "FanoutWidget.h"
#include "FaninWidget.h"
#include "FanOutInWidget.h"
#include "ParallelWidget.h"
#include "CascadeWidget.h"
#include "AccumulatorWidget.h"
#include "ShredderWidget.h"
#include "FlowThruWidget.h"
#include "SimilarityMatrixWidget.h"
#include "SelfSimilarityMatrixWidget.h"
#include "SilenceRemoveWidget.h"


using namespace Marsyas;
using namespace std;

MarSystemWidgetManager::MarSystemWidgetManager(GraphicalEnvironment* env)
{
  env_ = env;
}

MarSystemWidgetManager::~MarSystemWidgetManager()
{

}

MarSystemWidget* MarSystemWidgetManager::setupWidgets(MarSystem *msys)
{
  MarSystemWidget *aux;
  aux = createWidget(msys);
  createWidgetsMaps(aux);
  createControlWidgets(aux);
  createControlLinkWidgets(aux);

  return aux;
}


MarSystemWidget* MarSystemWidgetManager::createWidget(MarSystem *msys)
{

  MarSystemWidget *aux1;
  MarSystemWidget *aux2;

  vector <MarSystem*> children;
  children = msys->getChildren();

  aux1 = getWidget(msys);

  //create widgets for all children
  for(int i=0; i<children.size(); i++)
  {
    aux2 = createWidget(children[i]);
    aux1->addMarSystemWidget(aux2);
  }

  return aux1;
}


MarSystemWidget *MarSystemWidgetManager::getWidget(MarSystem *msys)
{

  string msysType = msys->getType();
  string msysName = msys->getName();

  MarSystemWidget *aux;

  cout<<endl<<"creating wiget for: "<<msysType<< " "<<msysName;

  //check if passed marsystem is a composite...
  if(msysType == "Series")
  {
    aux = new SeriesWidget(msys, env_); //FIXME: why do we need to provide the name??
  }
  else if(msysType == "Fanout")
  {
    aux = new FanoutWidget(msys, env_);
  }
  else if(msysType == "Fanin")
  {
    aux = new FaninWidget(msys, env_);
  }
  else if(msysType == "FanOutIn")
  {
    aux = new FanOutInWidget(msys, env_);
  }
  else if(msysType == "Parallel")
  {
    aux = new ParallelWidget(msys, env_);
  }
  else if(msysType == "Cascade")
  {
    aux = new CascadeWidget(msys, env_);
  }
  else if(msysType == "Accumulator")
  {
    aux = new AccumulatorWidget(msys, env_);
  }
  else if(msysType == "Shredder")
  {
    aux = new ShredderWidget(msys, env_);
  }
  else if(msysType == "FlowThru")
  {
    aux = new FlowThruWidget(msys, env_);
  }
  else if(msysType == "SimilarityMatrix")
  {
    aux = new SimilarityMatrixWidget(msys, env_);
  }
  else if(msysType == "SelfSimilarityMatrix")
  {
    aux = new SelfSimilarityMatrixWidget(msys, env_);
  }
  else if(msysType == "SilenceRemove")
  {
    aux = new SilenceRemoveWidget(msys, env_);
  }
  else //not a composite, so create non-composite MarSystem
  {
    aux = new MarSystemWidget(msys, env_);
  };

  return aux;
}

void MarSystemWidgetManager::createWidgetsMaps(MarSystemWidget* msysw)
{
  msysw->getTopMostParent()->insertToMsysMap(msysw->getMarSystem(), msysw);

  vector <MarSystemWidget*> children;
  children = msysw->getChildren();
  for(int i=0; i<children.size(); i++)
  {
    createWidgetsMaps(children[i]);
  }
}

void MarSystemWidgetManager::createControlWidgets(MarSystemWidget* msysw)
{
  msysw->createControlWidgets();

  vector <MarSystemWidget*> children;
  children = msysw->getChildren();
  for(int i=0; i<children.size(); i++)
  {
    createControlWidgets(children[i]);
  }
}

void MarSystemWidgetManager::createControlLinkWidgets(MarSystemWidget* msysw)
{
  msysw->createControlLinkWidgets();

  vector <MarSystemWidget*> children;
  children = msysw->getChildren();
  for(int i=0; i<children.size(); i++)
  {
    createControlLinkWidgets(children[i]);
  }
}






