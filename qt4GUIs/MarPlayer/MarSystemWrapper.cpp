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

/* Function: getctrl
   Parameters: name of the control
   Returns: control value
   Wrapper around MarSystem getctrl.
*/
MarControlValue MarSystemWrapper::getctrl(string cname)
{
	MarControlValue value;
	value = main_pnet_->getctrl(cname);
	return value;
} // end function

/* Function: updctrl
   Parameters: name of the control
	       control value
   Returns: nothing
   Checks flags then updates the control in pnet or
   pushes them into a vector to process when pnet is
   not ticking.
*/
void MarSystemWrapper::updctrl(string cname, MarControlValue cvalue)
{
	//cout << "Attempting to lock ... ";

	// if tryAcquire returns true, the semaphore is acquired
	if (pnet_sema_->tryAcquire(1)) {
		//cout << "locked with " << pnet_sema_->available() << " avail." << endl;

		if ( !running_ )
		{
			main_pnet_->updctrl(cname, cvalue);

 			//emit ctrlChanged(cname, cvalue);
		} else {
			// tr(cname.c_str()) means convert string to a cstring
			// tr converts the cstring to a QString
			// using the tr in QObject (not the other one)
			control_names_.push_back(tr(cname.c_str()));
			control_values_.push_back(cvalue);
			guard_ = true;

 			//emit ctrlChanged(cname, cvalue);
		} // end if else

		// check to make sure available is 0
		// otherwise release will create 1 more semaphore
		if ( pnet_sema_->available() == 0) {
			pnet_sema_->release(1);
		} // end if
	} // end if
} // end function

/* Function: pause
   Parameters: nothing
   Returns: nothing
   Handy function for changing pause variable.
*/
void MarSystemWrapper::pause()
{
	pause_ = true;
} // end function

/* Function: play
   Parameters: nothing
   Returns: nothing
   Handy function for changing pause variable.
*/
void MarSystemWrapper::play()
{
	pause_ = false;
} // end function

/* Function: run
   Parameters: none
   Returns: nothing
   One of the required thread methods to implement.
   Note: it runs forever.
*/
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
			//cout << "Locked update control with ";
			//cout << pnet_sema_->available() << " avail." << endl;
			
			// udpate stored controls
			vector<QString>::iterator vsi;
			vector<MarControlValue>::iterator vvi;

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
		{
		 if (main_pnet_->getctrl("mrs_bool/notEmpty").toBool() == false) 
		 {
			empty_ = true;
		 } // end if
		} // end if
	*/
	} // end while loop
} // end function
