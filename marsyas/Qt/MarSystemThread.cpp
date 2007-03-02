/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/** 
    \class MarSystemThread
    \brief Wraps a MarSystem network into a QThread

    The MarSystemThread creates a separate thread for running sound 
through the MarSystem dataflow network. 
*/

#include "MarSystemThread.h"

#ifdef MARSYAS_QT

#include <QMutexLocker>

using namespace Marsyas;

MarSystemThread::MarSystemThread(MarSystem* msys)
{
  msys_ = msys;
	stopped_ = false;
	sleep_ = true;
	msys_->updctrl("mrs_bool/active",false);
}

void MarSystemThread::stop()
{
	stopMutex_.lock();
	stopped_ = true;
	stopMutex_.unlock();
	wakeup();
}

void MarSystemThread::sleep()
{
	sleepMutex_.lock();
	sleep_ = true;
	sleepMutex_.unlock();
}

void MarSystemThread::wakeup()
{
	sleepMutex_.lock();
	sleep_ = false;
	sleepCondition_.wakeAll();
	sleepMutex_.unlock();
}

void MarSystemThread::run() 
{
	forever
  {
		//first thing to check: if the thread should go to sleep 
		//(i.e. block, and avoid wasting processing cycles doing nothing...)
		sleepMutex_.lock();
		if(sleep_)
		{
			sleepCondition_.wait(&sleepMutex_);	//wait until wakeup() is called!		
			sleep_ = false;
		}
		sleepMutex_.unlock();

		//check if the thread should be stopped...
		{
			QMutexLocker stopLocker(&stopMutex_);
			if(stopped_)
			{
				stopped_ = false;
				break; //break from forever loop and exit run() => stops thread!
			}
		}
	
		//if MarSystem is not active (i.e. inactive, paused)
		//this tick is ignored
		msys_->tick();
		
    //check for EOF //[!][?]
		if (msys_->getctrl("mrs_bool/notEmpty")->toBool() == false) 
		{
			if(msys_->getctrl("mrs_bool/active")->toBool())
			{
				emit Empty();
			}
		}
	}
}

#endif //MARSYAS_QT

