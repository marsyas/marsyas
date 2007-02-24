#ifndef __MARSPINBOX_H__
#define __MARSPINBOX_H__

#include "MarSystem.h"
#include "MarSystemManager.h" 
#include "MarControl.h"
#include <QSpinBox> 
#include <QString> 

#include <iostream> 
using namespace std;

using namespace Marsyas;
	
class MarSpinBox: public QSpinBox
{
  Q_OBJECT
public:
  MarSpinBox(MarControlPtr cname, QWidget *parent);
  
signals:
  void valueChanged(MarControlPtr cname, MarControlPtr val);
  
private slots:
void reemitvalueChanged(int);
void setValue(MarControlPtr cname);
  
private:
  MarControlPtr mycname;
};


#endif 

	
