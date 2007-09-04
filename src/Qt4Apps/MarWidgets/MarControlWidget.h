#ifndef __MARCONTROLWIDGET_H__
#define __MARCONTROLWIDGET_H__


#include <QWidget> 
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMetaType> 

#include "MarSpinBox.h"
#include "MarSlider.h"
#include "MarLineEdit.h"
#include "MarSystemQtWrapper.h"
// #include "MarGLWidget.h"
 

#include <string> 
#include <iostream>
using namespace std;
using namespace MarsyasQt;

	
class MarControlWidget: public QWidget
{
  Q_OBJECT
public:
  MarControlWidget(MarControlPtr cname, QString widget_type, MarSystemQtWrapper* mwr, QWidget *parent);
  
public slots:
 void valueChanged(MarControlPtr cname, MarControlPtr val);
  
private: 
  MarControlPtr mycname;
  
  
};

	
#endif
