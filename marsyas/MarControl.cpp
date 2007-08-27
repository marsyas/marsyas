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
	rwLock_.lockForRead(); //too many lockers?!? [!]
	#endif
	if (state_ && msys_)
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
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
		oss << "MarControl::linkTo() - Linking to an invalid control ";
		oss << "(" << ctrl->getName() << " with " << this->getName() << ").";
		MRSWARN(oss.str());
		return false;
	}

#ifdef MARSYAS_QT
	rwLock_.lockForRead();
#endif

	//check if these controls are already linked
	//(i.e. they own the same MarControlValue) 
	if(value_ == ctrl->value_)
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		return true;//already linked! :-)
	}
	
	if (ctrl->getType() != value_->getType())
	{
		ostringstream oss;
		oss << "MarControl::linkTo() - Linking two controls of different types ";
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
	ctrl->rwLock_.lockForRead();
#endif

	//get all the links of our current MarControlValue so we can also
	//re-link them to the passed ctrl
	vector<MarControl*> links = value_->getLinks();
	vector<MarControl*>::iterator lit;
	for(lit=links.begin(); lit!=links.end(); ++lit)
	{
		//linked controls will now point to the same MarControlValue
		//pointed by ctrl (i.e. they will take the same value as ctrl)
		(*lit)->value_->removeLink(*lit);
		(*lit)->value_ = ctrl->value_;
		(*lit)->value_->addLink(*lit);
		//(*lit)->callMarSystemUpdate();
	}
	links = value_->getLinks();
	for(lit=links.begin(); lit!=links.end(); ++lit)
	{
		(*lit)->callMarSystemUpdate();
	}

#ifdef MARSYAS_QT
	ctrl->rwLock_.unlock();
	rwLock_.unlock();
#endif
		
	return true;
}

void
MarControl::unlink()
{
#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
#endif

	//check if this MarControl is linked
	//(i.e. more than one MarControl linking
	//to the MarControlValue).
	//if not, no point doing unlink - just return.
	if(value_->getNumLinks() <= 1)
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		return;
	}

	MarControlValue* clonedvalue = value_->clone();
	value_->removeLink(this);
	value_ = clonedvalue;
	value_->addLink(this);

#ifdef MARSYAS_QT
	rwLock_.unlock();
#endif
}

bool
MarControl::isLinked() const
{
	#ifdef MARSYAS_QT
	QReadLocker r_locker(&rwLock_);
	#endif
	
	//if there is only one link (i.e. this control itself),
	//it means that there are no other linked controls
	// => return false (i.e. 0)
	if(value_->getNumLinks()-1 == 0)
		return false;
	else
		return true;
}

vector<MarControlPtr>
MarControl::getLinks()
{
	vector<MarControlPtr> res;
	vector<MarControl*> links = value_->getLinks();
	vector<MarControl*>::const_iterator lit;
	for(lit=links.begin(); lit != links.end(); ++lit)
		res.push_back(MarControlPtr(*lit));
	return res;
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
