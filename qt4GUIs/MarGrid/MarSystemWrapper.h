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
  MarSystemWrapper(Marsyas::MarSystem* msys);
  
  
public slots:

  void updctrl(QString cname, Marsyas::MarControlValue value);
  

  
  Marsyas::MarControlValue getctrl(std::string cname);
  
  void probe(int);
  void play();
  void pause();
  void run();
  void step();
  
  
signals: 
  void ctrlChanged(QString cname, Marsyas::MarControlValue value);
  void posChanged(int val);
  
private:
  QString cur_cname;
  Marsyas::MarControlValue cur_value;

  Marsyas::realvec vec_;
  
  
  Marsyas::MarSystem* msys_;
  
  std::vector<QString> cnames_;
  std::vector<Marsyas::MarControlValue> cvalues_;
  
  bool guard_;
  bool pause_;
  bool empty_;
  
  bool running_;  
  bool probing_;
  
};

#endif // MARSYSTEMWRAPPER_H
