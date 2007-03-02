/* Filename: MarSystemQtWrapper.h
 */

#include "MarSystemQtWrapper.h"


MarSystemQtWrapper::MarSystemQtWrapper(MarSystem* msys)
{
  main_pnet_ = msys;
  running_ = false;
  pause_ = true;
  empty_ = false;
  abort_ = false;
  counter_ = 0;
  qRegisterMetaType<MarControlPtr>("MarControlPtr");
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(emitTrackedControls()));
  timer->start(250);
}


MarSystemQtWrapper::~MarSystemQtWrapper()
{
  mutex_.lock();
  abort_ = true;
  pause_ = false;
  condition_.wakeAll();
  mutex_.unlock();
  wait();
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
  if (abort_) 
    return;
  
  main_pnet_->updctrl("mrs_bool/active", false);
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
  condition_.wakeAll();
  mutex_.unlock();
} 




void 
MarSystemQtWrapper::emitTrackedControls()
{
  mutex_.lock();
  vector<MarControlPtr>::iterator vsi;
  for (vsi = tracked_controls_.begin();
       vsi != tracked_controls_.end(); ++vsi)
    {
      emit ctrlChanged(*vsi);
    }
  mutex_.unlock();
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
      vector<MarControlPtr>::iterator vsi;
      vector<MarControlPtr>::iterator vvi;
      	
      for (vsi = control_names_.begin(),
	     vvi = control_values_.begin();
	   vsi != control_names_.end(); ++vsi, ++vvi)
	{
	  
	  main_pnet_->updctrl(*vsi, *vvi);
	} 
      
      control_names_.clear();
      control_values_.clear();
      mutex_.unlock();

      // emitTrackedControls();

      if (!pause_) 
	main_pnet_->tick();

      mutex_.lock();
      if (pause_) 
	condition_.wait(&mutex_);
      mutex_.unlock();
      
      
      

    }
}



