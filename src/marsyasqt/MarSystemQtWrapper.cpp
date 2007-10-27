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
	mutex_.lock();
	abort_ = true;
	pause_ = false;
	condition_.wakeOne();
	mutex_.unlock();
	wait();
}


void 
MarSystemQtWrapper::tickForever()
{
  mutex_.lock();
  if (!isRunning()) {
    start(HighPriority);
  } 
  else {
    main_pnet_->updctrl("mrs_bool/active", true);
    pause_ = false;
    condition_.wakeOne();
  }
  
  mutex_.unlock();
  
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
	if ( !running_ )
	{
	  main_pnet_->updctrl(cname, cvalue);
	} 
	else if (pause_) 
	  {
	    main_pnet_->updctrl(cname, cvalue);
	  }
	else 
	{
		mutex_.lock();
	  
		control_names_.push_back(cname);
		control_values_.push_back(cvalue);
		mutex_.unlock();
	} 
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
	main_pnet_->updctrl("mrs_bool/active", true);
	pause_ = false;
	condition_.wakeOne();
	mutex_.unlock();
} 


void 
MarSystemQtWrapper::emitTrackedControls()
{
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
	while(1) 
	{
	  if (abort_) 
	    return;

	  running_ = true;
	  // udpate stored controls
	  mutex_.lock();
	  counter_ ++;
	  QVector<MarControlPtr>::iterator vsi;
	  QVector<MarControlPtr>::iterator vvi;
	  
	  for (vsi = control_names_.begin(),
		 vvi = control_values_.begin();
	       vsi != control_names_.end(); ++vsi, ++vvi)
	    {
	      main_pnet_->updctrl(*vsi, *vvi);
	    } 
	  
	  control_names_.clear();
	  control_values_.clear();
	  mutex_.unlock();

	  if (!withTimer_)
	    emitTrackedControls();
	  
	  if (!pause_) 
	    main_pnet_->tick();
	  
	  mutex_.lock();
	  if (pause_)
	    {	      
	      main_pnet_->updctrl("mrs_bool/active", false);
	      condition_.wait(&mutex_);
	    } 
	  pause_ = false;
	  mutex_.unlock();
	}
}

} // namespace

