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
\class MarControl
\brief 
	MarControlPtr is a smart pointer wrapper for MarControl.
	MarControl is the control class containing a name, type and value.
	Created by lfpt@inescporto.pt and lmartins@inescporto.pt

*/

/************************************************************************/
/* MarControlPtr implementation                                         */
/************************************************************************/

#include "MarControl.h"
#include "MarControlValue.h"
#include "MarSystem.h"

using namespace std;
using namespace Marsyas;

MarControlPtr::MarControlPtr()
{
	control_ = NULL;
}

MarControlPtr::~MarControlPtr()
{
	if (control_)
	{
		control_->unref();
	}
}

/************************************************************************/
/* MarControl implementation                                            */
/************************************************************************/

MarControl*
MarControl::clone()
{
	return new MarControl(*this);
}

void
MarControl::setMarSystem(MarSystem* msys) 
{
#ifdef MARSYAS_QT
  QWriteLocker locker(&rwLock_);
#endif
  
  if(msys_) //[!]
    {
      //if this is a change in the parent MarSystem, 
      //then the control name must be updated accordingly
      string oldPrefix = msys_->getPrefix();
      string shortcname = cname_.substr(oldPrefix.length(), cname_.length());
      cname_ = msys->getPrefix() + shortcname;
    }
  msys_ = msys;
}

MarSystem*
MarControl::getMarSystem() 
{
#ifdef MARSYAS_QT
  QReadLocker locker(&rwLock_);
#endif
  
  return msys_;
}

void
MarControl::setName(std::string cname) //[!] should check if is in sync with current msys_ prefix...
{
#ifdef MARSYAS_QT
  QWriteLocker locker(&rwLock_);
#endif

  cname_ = cname; 
}

std::string 
MarControl::getName() const
{
#ifdef MARSYAS_QT
  QReadLocker locker(&rwLock_);
#endif

  return cname_;
}

void 
MarControl::setState(bool state) 
{
#ifdef MARSYAS_QT
  QWriteLocker locker(&rwLock_);
#endif
  
  state_ = state;
}

bool 
MarControl::hasState() const
{
#ifdef MARSYAS_QT
  QReadLocker locker(&rwLock_);
#endif
  
  return state_;
}

void MarControl::callMarSystemUpdate()
{
  if (state_ && msys_)
    msys_->controlUpdate(this); //thread-safe? [!]
}


#ifdef MARSYAS_QT
void
MarControlValue::emitControlChanged(MarControlValue* cvalue)
{
  //only bother calling MarSystem's controlChanged signal
  //if there is a GUI currently active(i.e. being displayed)
  //=> more efficient! [!]
  if(msys_)
    {
      if(msys_->activeControlsGUIs_.size() != 0 ||
	 msys_->activeDataGUIs_.size() != 0)//this class is friend of MarSystem //[!]
	{
	  QMetaObject::invokeMethod(msys_, "controlChanged", Qt::AutoConnection,
				    Q_ARG(MarControlValue*, cvalue));
	}
      
    }
}
#endif //MARSYAS_QT
