/************************************
  AggregateMarSystemNode.h - Header File
  Base Class For all collections of MarSystemNodes
*************************************/

#ifndef COMPOSITENODE_H
#define COMPOSITENODE_H
#include <vector>
#include <string>
#include <iostream>
using namespace std;
#include "MarSystemNode.h"
#include <marsyas/system/MarSystem.h>
using namespace Marsyas;

class CompositeNode:public MarSystemNode
{
public:
  CompositeNode(string type,QString name,QWidget* parent);
  CompositeNode(MarSystem* msys,QWidget* parent);

  //Basic addition member functions
  virtual bool append(MarSystemNode* newTail);
  virtual bool insert(MarSystemNode* before, MarSystemNode* node);//possible?
  virtual bool insert(int index,MarSystemNode* node);//possible?
  bool isMember(MarSystemNode* findMe);

  //need to override the default functionality of this obviously
  virtual bool isCollection();
  //Gets the Canvas to draw children on,,, most likely to be itself
  virtual QWidget* getChildrenCanvas();

  //TODO pretty sure we don't need to override this anymore
  //virtual void setChild(MarSystemNode* child);

public slots:
  void handleChildResize(int x,int y,int w,int h);

signals:
  void resized(int x,int y,int w,int h);

protected:
  vector<MarSystemNode*> nodes_;

  //MarSystemNode* next;
  //setBy setMarChild() currently only used by SeriesMarSystemNodes

};

#endif
