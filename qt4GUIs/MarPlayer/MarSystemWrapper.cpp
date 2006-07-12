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

using namespace std;
using namespace Marsyas;

MarSystemWrapper::MarSystemWrapper(MarSystem* msys)
{
  msys_ = msys;
  running_ = false;
  pause_ = true;
  empty_ = false;
}


MarControlValue 
MarSystemWrapper::getctrl(string cname)
{
  MarControlValue v;
  v = msys_->getctrl(cname);
  return v;
}


void 
MarSystemWrapper::updctrl(QString cname, MarControlValue value) 
{
  // controls can not be updated at any point in Marsyas 
  // so if the thread is running they are stored 
  // and then the actual updates happen in between 
  // calls to tick 
  /* cur_cname = cname;
  cur_value = value;
  
  cnames_.push_back(cname);
  cvalues_.push_back(value);
  if (!running_) 
    {
      msys_->updctrl(cname.toStdString(), value);
      emit ctrlChanged(cname, value);
    }
  else 
    {
      guard_ = true;
      emit ctrlChanged(cname, value);
    }
  */ 

  // controls can not be updated at any point in Marsyas  
  // so if the thread is running they are stored  
  // and then the actual updates happen in between  // calls to tick  
  //lmartins: cur_cname and cur_value do not comply with  
  //the usual member variable naming convention used in marsyas
  //(i.e. it should be cur_cname_ and cur_value_)    

  cur_cname_ = cname;
  cur_value_ = value;
  
  //cnames_.push_back(cname); //lmartins
  //cvalues_.push_back(value); //lmartins
  
  if (!running_)    
    {
      msys_->updctrl(cname.toStdString(), value);
      emit ctrlChanged(cname, value);
    }
  else    
    {
      cnames_.push_back(cname);          //lmartins
      cvalues_.push_back(value);         //lmartins  
      emit ctrlChanged(cname, value);
    }
}

void MarSystemWrapper::pause()
{
  pause_ = true;
}


void MarSystemWrapper::play()
{
  pause_ = false;
}



void MarSystemWrapper::run() 
{
  while(1)
    {
      running_ = true;

      //  udpate stored controls atomically 

      mutex_.tryLock();
      vector<QString>::iterator  vsi;
      vector<MarControlValue>::iterator vvi;
      
      for (vsi = cnames_.begin(), vvi = cvalues_.begin(); 
	   vsi != cnames_.end();
	   ++vsi, ++vvi)
	{
	  
	  msys_->updctrl(vsi->toStdString(), *vvi);
	}
      
      cnames_.clear();
      cvalues_.clear();
      mutex_.unlock();
      
      // Now play the samples by ticking the MarSystem 
      if (!pause_)
	{
	  msys_->tick();	
	  empty_ = false;
	}
      
      if (empty_ == false) 
	{	 
	  if (msys_->getctrl("mrs_bool/notEmpty").toBool() == false) 
	    {
	      empty_ = true;
	      pause();
	    }
	}
    }
}
