/* Filename: MarSystemQtWrapper.h
* Purpose: Wrapper around MarSystem framework.  This is a modified version
* of the MarPlayer application's MarSystemQtWrapper.h file by Dr. George Tzanetakis.
*/

#ifndef MARSYSTEMWRAPPER_H
#define MARSYSTEMWRAPPER_H


#include <QThread>
#include <QVector>
#include <QString>
#include <QMutex>
#include <QSemaphore>
#include <QWaitCondition>
#include <QTimer> 
#include <QMetaType>
#include "common_source.h"
#include "MarSystemManager.h"


using namespace std;
using namespace Marsyas;
namespace MarsyasQt
{
/**
	\ingroup MarsyasQt
	\brief This is the basic wrapper which allows MarSystems to function
inside a multithreaded Qt app.

	TODO: Long descr of libmarsyasqt.
*/

class MarSystemQtWrapper: public QThread
{
	Q_OBJECT

public:
	MarSystemQtWrapper(MarSystem* msys, bool withTimer = false);
	~MarSystemQtWrapper();
	void tickForever();
	void trackctrl(MarControlPtr control);

	QVector<MarControlPtr> getTrackedControls();

	public slots:
		void updctrl(MarControlPtr control, MarControlPtr cval);
		void updctrl(std::string cname, MarControlPtr newcontrol) 
		{
			MarControlPtr control = main_pnet_->getControl(cname);
			return updctrl(control, newcontrol);
		}
		void updctrl(char *cname, MarControlPtr newcontrol) 
		{
			MarControlPtr control = main_pnet_->getControl(cname);
			return updctrl(control, newcontrol);
		}
		void emitTrackedControls();

		MarControlPtr getctrl(string cname);

		void play();
		void pause();

signals:
		void ctrlChanged(MarControlPtr cname);

protected:
	void run();
private:
	QMutex mutex_;
	bool abort_;
	bool withTimer_;

	QWaitCondition condition_; 
	int counter_;

	// the underlying MarSystem
	MarSystem* main_pnet_;

	// Vectors for pushing in events that cannot be
	// processes while the main MarSystem is ticking
	QVector<MarControlPtr> control_names_;
	QVector<MarControlPtr> control_values_;
	QVector<MarControlPtr> tracked_controls_;
	bool pause_;

	bool running_;
};
} // namespace
#endif // MARSYSTEMWRAPPER_H
