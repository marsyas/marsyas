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

/************************************************************************/
/* MarControlPtr implementation                                         */
/************************************************************************/

#include "MarControl.h"
#include "MarControlValue.h"
#include "MarSystem.h"

using namespace std;
using namespace Marsyas;

#ifdef TRACECONTROLS
std::set<MarControl*> MarControlPtr::controlTracer;

void
MarControlPtr::printControlTracer()
{
	std::set<MarControl*>::iterator it;
	if (MarControlPtr::controlTracer.size() > 0)
	{
		cout << "#############################################################" << endl;
		cout << "++ Existing MarControls: " << MarControlPtr::controlTracer.size() << endl;
		for (it=MarControlPtr::controlTracer.begin(); it!=MarControlPtr::controlTracer.end(); it++)
		{
			cout << (*it)->getMarSystem()->getPrefix() << (*it)->getName() 
				<< " | ref.count: " << (*it)->getRefCount() << endl;
		}
		cout << "#############################################################" << endl;
	}
}
#endif

MarControlPtr::MarControlPtr()
{
  control_ = NULL;
}

MarControlPtr::~MarControlPtr()
{
	if (control_)
	{
		TRACE_REMCONTROL;
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
  
//   if(msys_) //[!]
//     {
//       //if this is a change in the parent MarSystem, 
//       //then the control name must be updated accordingly
//       string oldPrefix = msys_->getPrefix();
//       string shortcname = cname_.substr(oldPrefix.length(), cname_.length());
//       cname_ = msys->getPrefix() + shortcname;
//     }
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
MarControl::setName(std::string cname)
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

string
MarControl::getType() const
{ 
#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_);
#endif

	return value_->getType(); 
}

void MarControl::callMarSystemUpdate()
{
	#ifdef MARSYAS_QT
	rwLock_.lockForRead(); //too many lockers?!?
	#endif
	if (state_ && msys_)
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		//msys_->controlUpdate(this);
		msys_->update(this);
		return;
	}
	#ifdef MARSYAS_QT
	rwLock_.unlock();
	#endif
}

bool
MarControl::linkTo(MarControlPtr ctrl)
{
	if (ctrl.isInvalid())
	{
		ostringstream oss;
		oss << "[MarControl::linkTo] Linking to an invalid control ";
		oss << "(" << ctrl->getName() << " with " << this->getName() << ").";
		MRSWARN(oss.str());
		return false;
	}

#ifdef MARSYAS_QT
	rwLock_.lockForRead();
#endif

	//check if these controls are already linked
	vector<MarControlPtr>::const_iterator ci;
	for(ci = linkedTo_.begin(); ci != linkedTo_.end(); ++ci) //iterator may become invalid without a lock!! [!][?] 
	{
		//compare MarControl* (the actual pointer and not its value)
		if((*ci)() == ctrl())
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock();
			#endif
			return true;//already linked! :-)
		}
	}
	
	if (ctrl->getType() != value_->getType())
	{
		ostringstream oss;
		oss << "[MarControl::linkTo] Linking two controls of different types ";
		oss << "(" << ctrl->getName() << " with " << this->getName() << ").";
		MRSWARN(oss.str());
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		return false;
	}

#ifdef MARSYAS_QT
	rwLock_.unlock();
	rwLock_.lockForWrite();
	ctrl->rwLock_.lockForWrite();
#endif

	linkedTo_.push_back(ctrl);
	isLinked_ = true;

	//the linked control should also be linked to this one!
	ctrl->linkedTo_.push_back(this);
	ctrl->isLinked_ = true;

#ifdef MARSYAS_QT
	ctrl->rwLock_.unlock();
	rwLock_.unlock();
#endif
	
	//sync control values (and sizes in case of mrs_vector controls)
	//operator= is protected by mutexes
	*this = *ctrl;
	
	return true;
}

void
MarControl::clearLinks()
{
#ifdef MARSYAS_QT
	rwLock_.lockForRead();
#endif

	for (size_t i=0; i<linkedTo_.size(); i++)
	{
		linkedTo_[i]->removeLink(this);
	}

#ifdef MARSYAS_QT
	rwLock_.unlock();
	rwLock_.lockForWrite();
#endif

	linkedTo_.clear();

#ifdef MARSYAS_QT
	rwLock_.unlock();
#endif
}

void
MarControl::removeLink(MarControlPtr link)
{
#ifdef MARSYAS_QT
	rwLock_.lockForRead();
	link->rwLock_.lockForRead();
#endif

	std::vector<MarControlPtr> temp;
	for (size_t i=0; i<linkedTo_.size(); i++)
	{
		if (linkedTo_[i]() != link()) // same pointer?
		{
			temp.push_back(linkedTo_[i]);
		}
	}

#ifdef MARSYAS_QT
	link->rwLock_.unlock();
	rwLock_.unlock();
	rwLock_.lockForWrite();
#endif

	linkedTo_=temp;

#ifdef MARSYAS_QT
	rwLock_.unlock();
#endif
}

#ifdef MARSYAS_QT
void
MarControl::emitControlChanged(MarControl* control)
{
	//only bother calling MarSystem's controlChanged signal
	//if there is a GUI currently active(i.e. being displayed)
	//=> more efficient! [!]
	if(msys_)
	{
		if(msys_->activeControlsGUIs_.size() != 0 ||
			msys_->activeDataGUIs_.size() != 0)//this class is friend of MarSystem //[!]
		{
			bool registered = QMetaType::isRegistered(QMetaType::type ("MarControl*"));
			QMetaObject::invokeMethod(msys_, "controlChanged", 
				Qt::AutoConnection,
				Q_ARG(MarControl*, control));
		}
	}
}
#endif //MARSYAS_QT
