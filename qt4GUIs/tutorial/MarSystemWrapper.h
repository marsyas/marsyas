/* Filename: MarSystemWrapper.h
 * Purpose: Wrapper around MarSystem framework.  This is a modified version
 * of the MarPlayer application's MarSystemWrapper.h file by Dr. George Tzanetakis.
 */

#ifndef MARSYSTEMWRAPPER_H
#define MARSYSTEMWRAPPER_H


#include <QThread>
#include <QString>
#include <QMutex>
#include <QSemaphore>
#include "common.h"
#include "MarSystemManager.h"


using namespace std;
using namespace Marsyas;

class MarSystemWrapper: public QThread
{
	Q_OBJECT

public:
	MarSystemWrapper(MarSystem* msys);

public slots:
  void updctrl(MarControlPtr control, MarControlPtr cval);
  MarControlPtr getctrl(string cname);
  
  void play();
  void pause();
  void run();

signals:
  void ctrlChanged(MarControlPtr cname, MarControlPtr cvalue);
  void posChanged(int val);
	
private:
  QMutex mutex_;
  
  // the underlying MarSystem
  MarSystem* main_pnet_;
  
  // Vectors for pushing in events that cannot be
  // processes while the main MarSystem is ticking
  vector<MarControlPtr> control_names_;
  vector<MarControlPtr> control_values_;
  
  bool guard_;
  bool pause_;
  bool empty_;
  
  bool running_;
};

#endif // MARSYSTEMWRAPPER_H
