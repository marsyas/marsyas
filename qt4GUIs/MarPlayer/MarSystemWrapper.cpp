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
    \class MarSystemWrapper
    \brief Wraps a MarSystem network into a Qt-like object with signals/slots

    The MarSystemWrapper is the generic way of interfacing a GUI written 
in Qt 4 with Marsyas. It creates a separate thread for running sound 
through the MarSystem dataflow network. The network is communicates 
with the Qt code through the standard signal/slot mechanism 
adjusted to reflect Marsyas controls. 
*/

#include "MarSystemWrapper.h"

#include <QMutexLocker>

using namespace std;
using namespace Marsyas;

MarSystemWrapper::MarSystemWrapper(MarSystem* msys)
{
  msys_ = msys;
	stopped_ = false;
	updctrl("mrs_bool/active",false);
}

MarControlValue 
MarSystemWrapper::getctrl(string cname)
{
  MarControlValue v;
  
	ctrlMutex_.lock();
	v = msys_->getctrl(cname);
  ctrlMutex_.unlock();
	
	return v;
}

void 
MarSystemWrapper::updctrl(QString cname, MarControlValue value) 
{
  // controls can not be updated at any point in Marsyas 
  // so if the thread is running they are stored 
  // and then the actual updates happen in between 
  // calls to tick 
	if (!isRunning()) 
  {
    msys_->updctrl(cname.toStdString(), value);
    emit ctrlChanged(cname, value);
  }
  else    
  {
    ctrlMutex_.lock();
		cnames_.push_back(cname);
    cvalues_.push_back(value);
		ctrlMutex_.unlock();
    emit ctrlChanged(cname, value);
  }
}

void MarSystemWrapper::stopThread()
{
	stopMutex_.lock();
	stopped_ = true;
	stopMutex_.unlock();
}

void MarSystemWrapper::pause()
{
	updctrl("mrs_bool/active", false);
}


void MarSystemWrapper::play()
{
	updctrl("mrs_bool/active", true);
}

void MarSystemWrapper::run() 
{
	while(1)//forever
  {
		{
			QMutexLocker stopLocker(&stopMutex_);
				if(stopped_)
				{
					stopped_ = false;
					break; //break from while(1) and exit run() => stops thread!
				}
		}
				
		//update stored controls atomically 
    if(ctrlMutex_.tryLock())
		{
			vector<QString>::iterator  vsi;
			vector<MarControlValue>::iterator vvi;
	    
			for (vsi = cnames_.begin(), vvi = cvalues_.begin(); 
					vsi != cnames_.end(); ++vsi, ++vvi)
			{
				msys_->updctrl(vsi->toStdString(), *vvi);
			}
			cnames_.clear();
			cvalues_.clear();
	    
			ctrlMutex_.unlock();
		}
    
		//if MarSystem is not active (i.e. paused)
		//this tick is ignored
		msys_->tick();
    
    //check for EOF
		if (msys_->getctrl("mrs_bool/notEmpty").toBool() == false) 
		{
			pause();
		}

	}
}
