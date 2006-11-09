/* Filename: MarSystemWrapper.h
 * Purpose: Wrapper around MarSystem framework.  This is a modified version
 * of the MarPlayer application's MarSystemWrapper.h file by Dr. George Tzanetakis.
 */

#ifndef MARSYSTEMWRAPPER_H
#define MARSYSTEMWRAPPER_H

#ifndef QTHREAD
#include <QThread>
#endif

#ifndef QSTRING
#include <QString>
#endif

#ifndef QSEMAPHORE
#include <QSemaphore>
#endif

#ifndef COMMON_H
#include "common.h"
#endif

// include the MARSYAS system
#ifndef MARSYSTEMMANAGER_H
#include "MarSystemManager.h"
#endif

using namespace std;
using namespace Marsyas;

class MarSystemWrapper: public QThread
{
	Q_OBJECT

public:
	MarSystemWrapper(MarSystem* msys);

public slots:
  void updctrl(string cname, MarControlPtr cvalue);
  MarControlPtr getctrl(string cname);

	void play();
	void pause();
	void run();

signals:
	void ctrlChanged(string cname, MarControlPtr cvalue);
	void posChanged(int val);
	
private:
	QSemaphore* pnet_sema_;

	// the underlying MarSystem
	MarSystem* main_pnet_;
	
	// Vectors for pushing in events that cannot be
	// processes while the main MarSystem is ticking
	vector<QString> control_names_;
	vector<MarControlPtr> control_values_;
	
	bool guard_;
	bool pause_;
	bool empty_;
	
	bool running_;
};

#endif // MARSYSTEMWRAPPER_H
