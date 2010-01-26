#ifndef __MARGLWIDGET_H__
#define __MARGLWIDGET_H__


#include "GlWidget.h" 
#include "MarSystem.h"

#include <QString>

#include <string> 
#include <iostream>
#include "MarControlValue.h" 

using namespace std;
	
class MarGLWidget: public GLWidget
{
  Q_OBJECT
public:
  MarGLWidget(QString mycname, QWidget *parent);
  
signals:
		void valueChanged(QString cname, Marsyas::MarControlValue value);

  
private slots:
		void reemitvalueChanged(Marsyas::realvec vec);
  
		void setValue(QString cname, Marsyas::MarControlValue value);
  
  
private:
  QString mycname;
  float* data_;
  int size_;
  
};


#endif 

	
