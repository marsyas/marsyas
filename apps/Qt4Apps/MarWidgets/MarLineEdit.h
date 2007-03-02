#ifndef __MARLINEEDIT_H__
#define __MARLINEEDIT_H__

#include "MarSystem.h"
#include "MarSystemManager.h"
#include <QLineEdit> 
#include <QString>

#include <iostream>
using namespace std;

using namespace Marsyas;

	
class MarLineEdit: public QLineEdit
{
  Q_OBJECT
public:
  MarLineEdit(MarControlPtr mycname, QWidget *parent);
  
signals:
  void valueChanged(MarControlPtr cname, MarControlPtr val);
  
private slots:
void reemitvalueChanged();
void setValue(MarControlPtr cname);
  
private:
  MarControlPtr mycname;
};


#endif 

	

	
