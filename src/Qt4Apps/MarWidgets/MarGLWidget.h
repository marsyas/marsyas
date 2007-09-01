#ifndef __MARGLWIDGET_H__
#define __MARGLWIDGET_H__


#include "GlWidget.h" 
#include "MarSystem.h"

#include <QString>

#include <string> 
#include <iostream>
using namespace std;

	
class MarGLWidget: public GLWidget
{
  Q_OBJECT
public:
  MarGLWidget(QString mycname, QWidget *parent);
  
signals:
  void valueChanged(QString cname, MarControlValue value);

  
private slots:
void reemitvalueChanged(realvec vec);
  
  void setValue(QString cname, MarControlValue value);
  
  
private:
  QString mycname;
  float* data_;
  int size_;
  
};


#endif 

	
