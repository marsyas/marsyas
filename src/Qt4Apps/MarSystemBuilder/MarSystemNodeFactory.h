/*****************************
 MarSystemNodeFactory.h - Header File
 Defines a class that creates MarSystemNodes, at the moment the only
 thing it needs to knwo is the type of MarSystemNodes to create
******************************/
#ifndef MARSYSTEMNODEFACTORY_H
#define MARSYSTEMNODEFACTORY_H

#include "MarSystemNode.h"
#include "DefaultMarSystemNode.h"
#include "GainNode.h"
#include "CompositeNode.h"
#include "FanoutNode.h"
#include "SeriesNode.h"
#include "DefaultCompositeNode.h"
#include <marsyas/system/MarSystem.h>
#include <MarSystemManager.h>
#include <string>
#include <iostream>
using namespace std;

class MarSystemNodeFactory
{
public:
  static MarSystemNodeFactory* getInstance();
  MarSystemNode* newNode(string type,QString name,QWidget* parent);
  MarSystemNode* newNode(MarSystem* msys,QWidget* parent,bool isComposite=0);
  static string GAIN_TYPE;
  static string FANOUT_TYPE;
  static string SERIES_TYPE;
private:
  MarSystemNodeFactory();
  static MarSystemNodeFactory* instance;
  MarSystemManager* msManager;
};
#endif
