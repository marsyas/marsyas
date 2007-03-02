

#include "MarSystemWrapper.h"

/* Function: MarSystemWrapper constructor
   Parameters: MarSystem
   Returns: nothing
   Initializes the MarSystemWrapper by creating the pnet_ and mix_ MarSystem.
*/
MarSystemWrapper::MarSystemWrapper(MarSystem* msys)
{
	main_pnet_ = msys;
	pnet_sema_ = new QSemaphore(1);

	guard_ = false;
	running_ = false;

	pause_ = false;
	empty_ = false;
}


MarControlPtr MarSystemWrapper::getctrl(string cname)
{
  MarControlPtr value;
  value = main_pnet_->getctrl(cname);
  return value;
} // end function


void MarSystemWrapper::updctrl(string cname, MarControlPtr cvalue)
{
  
  if (pnet_sema_->tryAcquire(1)) {
    
    if ( !running_ )
      {
	main_pnet_->updctrl(cname, cvalue);
	
	emit ctrlChanged(cname, cvalue);
      } else {
	control_names_.push_back(tr(cname.c_str()));
	control_values_.push_back(cvalue);
	guard_ = true;
	
	emit ctrlChanged(cname, cvalue);
      } 
    
    // check to make sure available is 0
    // otherwise release will create 1 more semaphore
    if ( pnet_sema_->available() == 0) {
      pnet_sema_->release(1);
    } 
  } 
} 


/* Function: pause
   Parameters: nothing
   Returns: nothing
   Handy function for changing pause variable.
*/
void MarSystemWrapper::pause()
{
  pause_ = true;
} 

/* Function: play
   Parameters: nothing
   Returns: nothing
   Handy function for changing pause variable.
*/
void MarSystemWrapper::play()
{
	pause_ = false;
} // end function


void MarSystemWrapper::run() 
{
  while(1) {
    running_ = true;
    
    // if tryAcquire returns true, the semaphore is acquired
    // if tryAcquire returns false, the controls waiting to be
    // updated remain in the vectors.
    if (pnet_sema_->tryAcquire(1)) {
      if (guard_ == true)
	{

	  // udpate stored controls
	  vector<QString>::iterator vsi;
	  vector<MarControlPtr>::iterator vvi;
	  
	  for (vsi = control_names_.begin(),
		 vvi = control_values_.begin();
	       vsi != control_names_.end(); ++vsi, ++vvi)
	    {
	      main_pnet_->updctrl(vsi->toStdString(), *vvi);
	      
	      //cout << vsi->toStdString() << " " << *vvi << endl;
	    } // end for
	  
	  control_names_.clear();
	  control_values_.clear();
	  guard_ = false;
	} // end if
      
      // check to make sure available is 0
      // otherwise release will create 1 more semaphore
      if ( pnet_sema_->available() == 0) {
	pnet_sema_->release(1);
      } // end inner if
    } // end outer if
    
    if (!pause_)
      {
	main_pnet_->tick();	
	//empty_ = false;
      } // end if else
	/* No need for this in MCI for now
	   if (empty_ == false) 
	   {tick
	   if (main_pnet_->getctrl("mrs_bool/notEmpty").toBool() == false) 
	   {
	   empty_ = true;
	   } // end if
	   } // end if
	*/
  } // end while loop
} // end function
