/* Filename: MarSystemWrapper.h
 * Purpose: Wrapper around MarSystem framework.  This is a modified version
 * of the MarPlayer application's MarSystemWrapper.cpp file by Dr. George Tzanetakis.
 *
 * Some differences between MarPlayer's MarSystemWrapper (from marsyas-0.2.7)
 * and MCI's MarSystemWrapper:
 * 1) MCI's MarSystemWrapper uses a semaphores to protect the main_pnet_ MarSystem.
 * 2) updatectrl methods take in strings instead of QStrings
 */

#include "MarSystemWrapper.h"


MarSystemWrapper::MarSystemWrapper(MarSystem* msys)
{
  main_pnet_ = msys;
  running_ = false;
  pause_ = false;
  empty_ = false;
}

MarControlPtr 
MarSystemWrapper::getctrl(string cname)
{
  mutex_.lock();
  MarControlPtr value;
  value = main_pnet_->getctrl(cname);
  mutex_.unlock();
  
  return value;

  
} 


void 
MarSystemWrapper::updctrl(MarControlPtr cname, 
			  MarControlPtr cvalue)
{

  cout << "mrw::updctrl called" << endl;
  
  if ( !running_ )
    {
      main_pnet_->updctrl(cname, cvalue);
      emit ctrlChanged(cname, cvalue);
    } 
  else 
    {
      mutex_.lock();
      control_names_.push_back(cname);
      control_values_.push_back(cvalue);
      emit ctrlChanged(cname, cvalue);
      mutex_.unlock();
    } 
  
} 


void 
MarSystemWrapper::pause()
{ 
  mutex_.lock();
  main_pnet_->updctrl("mrs_bool/active", false);
  mutex_.unlock();
} 

void 
MarSystemWrapper::play()
{
  mutex_.lock();
  main_pnet_->updctrl("mrs_bool/active", true);
  mutex_.unlock();
} 

void 
MarSystemWrapper::run() 
{
  while(1) 
    {
      running_ = true;
      // udpate stored controls
      mutex_.lock();
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
      
      // now we are ready to tick the network
      main_pnet_->tick();	
      

    }
}



