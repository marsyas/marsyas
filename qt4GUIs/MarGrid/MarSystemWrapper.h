#ifndef MARSYSTEMWRAPPER_H
#define MARSYSTEMWRAPPER_H

#include <QObject>
#include <QCheckBox>
#include <QThread>
#include <QString> 

#include "common.h"
#include "MarSystemManager.h"


class MarSystemWrapper: public QThread
{
  Q_OBJECT
  
public:
  MarSystemWrapper(MarSystem* msys);
  
  
public slots:

  void updctrl(QString cname, MarControlValue value);
  

  
  MarControlValue getctrl(string cname);
  
  void probe(int);
  void play();
  void pause();
  void run();
  void step();
  
  
signals: 
  void ctrlChanged(QString cname, MarControlValue value);
  void posChanged(int val);
  
private:
  QString cur_cname;
  MarControlValue cur_value;

  realvec vec_;
  
  
  MarSystem* msys_;
  
  vector<QString> cnames_;
  vector<MarControlValue> cvalues_;
  
  bool guard_;
  bool pause_;
  bool empty_;
  
  bool running_;  
  bool probing_;
  
};

#endif // MARSYSTEMWRAPPER_H
