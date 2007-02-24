#ifndef __MARSLIDER_H__
#define __MARSLIDER_H__

#include "MarSystem.h" 
#include "MarSystemManager.h"
#include <QSlider> 
#include <QString> 

#include <string> 
#include <iostream>
using namespace std;

using namespace Marsyas;
	
class MarSlider: public QSlider
{
  Q_OBJECT
public:
  MarSlider(MarControlPtr mycname, QWidget *parent);
  
signals:
  void valueChanged(MarControlPtr cname, MarControlPtr val);
  
private slots:
void reemitvalueChanged(int);
void setValue(MarControlPtr cname);

  
private:
  MarControlPtr mycname;
  int myval;
  float pfval;
  
};


#endif 
