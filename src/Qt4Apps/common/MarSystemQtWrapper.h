/* Filename: MarSystemQtWrapper.h
* Purpose: Wrapper around MarSystem framework.  This is a modified version
* of the MarPlayer application's MarSystemQtWrapper.h file by Dr. George Tzanetakis.
*/

#ifndef MARSYSTEMWRAPPER_H
#define MARSYSTEMWRAPPER_H


#include <QThread>
#include <QVector>
#include <QMap>
#include <QString>
#include <QMutex>
#include <QSemaphore>
#include <QWaitCondition>
#include <QTimer>
#include <QMetaType>
#include <marsyas/common_source.h>
#include <marsyas/system/MarSystemManager.h>


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
  void updctrl(const char *cname, MarControlPtr newcontrol)
  {
    MarControlPtr control = main_pnet_->getControl(cname);
    return updctrl(control, newcontrol);
  }
  void emitTrackedControls();

  MarControlPtr getctrl(string cname);

  void play();
  void pause();
  void stop();

signals:
  void ctrlChanged(MarControlPtr cname);

protected:
  void run();
private:
  void do_queued_updates();

  QMutex mutex_;
  bool abort_;
  bool withTimer_;

  QWaitCondition condition_;
  int counter_;

  // the underlying MarSystem
  MarSystem* main_pnet_;

  QMap<MarControlPtr, MarControlPtr> queued_updates_;

  QVector<MarControlPtr> tracked_controls_;
  bool pause_;

  bool running_;
};
} // namespace
#endif // MARSYSTEMWRAPPER_H
