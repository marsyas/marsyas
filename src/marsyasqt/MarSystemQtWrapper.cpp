#include "MarSystemQtWrapper.h"
namespace MarsyasQt
{

MarSystemQtWrapper::MarSystemQtWrapper(MarSystem* msys, bool withTimer)
{
  main_pnet_ = msys;
  running_ = false;
  pause_ = true;
  abort_ = false;
  counter_ = 0;
  qRegisterMetaType<MarControlPtr>("MarControlPtr");

  withTimer_ = withTimer;

  if (withTimer_)
  {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(emitTrackedControls()));
    timer->start(100);
  }
}

MarSystemQtWrapper::~MarSystemQtWrapper()
{
  stop();
}


void
MarSystemQtWrapper::tickForever()
{
  if (!isRunning())
    start(HighPriority);
  else
    play();
}



MarControlPtr
MarSystemQtWrapper::getctrl(string cname)
{
  mutex_.lock();
  MarControlPtr value;
  value = main_pnet_->getctrl(cname);
  mutex_.unlock();
  return value;
}

void
MarSystemQtWrapper::updctrl(MarControlPtr cname,
                            MarControlPtr cvalue)
{
  mutex_.lock();
  if (!running_)
    main_pnet_->updControl(cname, cvalue);
  else
    // FIXME: not real-time safe:
    queued_updates_.insert(cname, cvalue);
  mutex_.unlock();
}

void
MarSystemQtWrapper::pause()
{
  mutex_.lock();
  pause_ = true;
  mutex_.unlock();
}



void
MarSystemQtWrapper::trackctrl(MarControlPtr control)
{
  mutex_.lock();
  tracked_controls_.push_back(control);
  mutex_.unlock();
}

void
MarSystemQtWrapper::play()
{
  mutex_.lock();
  pause_ = false;
  condition_.wakeOne();
  mutex_.unlock();
}

void MarSystemQtWrapper::stop()
{
  mutex_.lock();
  abort_ = true;
  condition_.wakeOne();
  mutex_.unlock();
  wait();
}

void
MarSystemQtWrapper::emitTrackedControls()
{
  // FIXME: this is not safe, because the mutex is not held while the MarSystem is ticked.
  mutex_.lock();
  QVector<MarControlPtr>::iterator vsi;
  for (vsi = tracked_controls_.begin();
       vsi != tracked_controls_.end(); ++vsi)
  {
    emit ctrlChanged(*vsi);
  }
  mutex_.unlock();
}

QVector<MarControlPtr> MarSystemQtWrapper::getTrackedControls()
{
  return tracked_controls_;
}

void
MarSystemQtWrapper::run()
{
  mutex_.lock();

  running_ = true;
  main_pnet_->updControl("mrs_bool/active", true);

  while(!abort_)
  {
    do_queued_updates();

    if (pause_)
    {
      main_pnet_->updControl("mrs_bool/active", false);
      condition_.wait(&mutex_);
      main_pnet_->updControl("mrs_bool/active", true);
    }
    else
    {
      mutex_.unlock();

      // NOTE:
      // The mutex is not held while ticking the MarSystem,
      // because tick() can potentially block indefinitely.
      main_pnet_->tick();

      mutex_.lock();
    }
  }

  // make sure not to miss latest updates:
  do_queued_updates();

  running_ = false;
  main_pnet_->updControl("mrs_bool/active", false);

  mutex_.unlock();
}

void MarSystemQtWrapper::do_queued_updates()
{
  QMap<MarControlPtr, MarControlPtr>::iterator queued_update;
  for(queued_update = queued_updates_.begin();
      queued_update != queued_updates_.end();
      ++queued_update)
  {
    queued_update.key()->setValue( queued_update.value() );
  }
  queued_updates_.clear();
}

} // namespace

