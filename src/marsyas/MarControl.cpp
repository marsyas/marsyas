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

void 
MarControl::callMarSystemUpdate()
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
MarControl::linkTo(MarControlPtr ctrl, bool update)
{
	if (ctrl.isInvalid())
	{
		ostringstream oss;
		oss << "MarControl::linkTo() - Linking to an invalid control ";
		oss << "(" << ctrl->cname_ << " with " << cname_ << ").";
		MRSWARN(oss.str());
		return false;
	}

	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	ctrl->rwLock_.lockForRead();
	#endif

	//check if these controls are already linked
	//(i.e. they own the same MarControlValue) 
	if(value_ == ctrl->value_)
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		ctrl->rwLock_.unlock();
		#endif
		return true;//already linked! :-)
	}
	
	if (ctrl->value_->type_ != value_->type_)
	{
		ostringstream oss;
		oss << "MarControl::linkTo() - Linking controls of different types ";
		oss << "(" << ctrl->cname_ << " with " << cname_ << ").";
		MRSWARN(oss.str());
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		ctrl->rwLock_.unlock();
		#endif
		return false;
	}

	//unlink this control (but keeping all links to it) 
	//before linking it again to the passed control
	this->unlinkFromTarget();

	//store a pointer to the (soon to be old) MarControlValue object
	MarControlValue* oldvalue = value_;
	//and a pointer to the new value
	MarControlValue* newvalue = ctrl->value_;

	#ifdef MARSYAS_QT
	newvalue->rwLock_.lockForWrite();
	oldvalue->rwLock_.lockForWrite();
	#endif

	//get all the links of our current MarControlValue so we can also
	//re-link them to the passed ctrl
	vector<pair<MarControl*, MarControl*> >::iterator lit;
	for(lit=oldvalue->links_.begin(); lit!=oldvalue->links_.end(); ++lit)
	{
		#ifdef MARSYAS_QT
		//we must lock each linked MarControl, so we can
		//safely make it point to the new cloned MarControlValue
		//without interferences from other threads
		if(MarControlPtr(this) != lit->first) //this MarControl is already locked, so we must avoid a deadlock!
			lit->first->rwLock_.lockForWrite(); 
		#endif

		//make each linked control now point to the "passed" MarControlValue
		lit->first->value_ = newvalue;

		// check if this is the root link
		if(lit->first == lit->second)
		{
			//make it "link to" the passed control
			newvalue->links_.push_back(pair<MarControl*, MarControl*>(lit->first, ctrl())); 
		}
		else //if not a root link, just copy the table entry unchanged into the new MarControlValue
			newvalue->links_.push_back(*lit); 

		#ifdef MARSYAS_QT
		lit->first->rwLock_.unlock();
		#endif
	}
	//old MarControlValue can and should now be deleted from memory
	#ifdef MARSYAS_QT
	oldvalue->rwLock_.unlock();
	#endif
	delete oldvalue;

	#ifdef MARSYAS_QT
	//all controls now linked to the new value, so we may release the locks
	newvalue->rwLock_.unlock();
	ctrl->rwLock_.unlock();
	#endif

	//check if it's needed to call update()
	if(update)
		newvalue->callMarSystemsUpdate();

	#ifdef MARSYAS_QT
	emitControlChanged(this);
	#endif

	return true;
}

void
MarControl::unlinkFromAll()
{
	//first unlink this control from all the controls to which
	//it links to
	this->unlinkFromTarget();

	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	value_->rwLock_.lockForWrite(); //this will deadlock!!!! [!][?]
	#endif

	//now unlink all controls that link directly to this control
	vector<pair<MarControl*, MarControl*> >::iterator lit;
	vector<MarControl*> linkedControls;
	for(lit=value_->links_.begin(); lit!=value_->links_.end(); ++lit)
	{
		if(lit->second == this && lit->first != lit->second)
			linkedControls.push_back(lit->first);
	}

	for(mrs_natural i=0; i < (mrs_natural)linkedControls.size(); ++i)
		linkedControls[i]->unlinkFromTarget();

	#ifdef MARSYAS_QT
	value_->rwLock_.unlock();
	rwLock_.unlock();
	#endif
}

void
MarControl::unlinkFromTarget()
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	value_->rwLock_.lockForWrite();
	#endif

	//check if this MarControl is linked
	//(i.e. more than one MarControl linking
	//to the MarControlValue).
	//if not, no point doing unlink - just return.
	if(value_->links_.size() <= 1)
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		value_->rwLock_.unlock();
		#endif
		return;
	}

	MarControlValue* oldvalue = value_;
	MarControlValue* newvalue = oldvalue->clone();

	vector<pair<MarControl*, MarControl*> >* inSet = new vector<pair<MarControl*, MarControl*> >;
	vector<pair<MarControl*, MarControl*> >* outSet = new vector<pair<MarControl*, MarControl*> >;

	mrs_natural toProcess = oldvalue->links_.size();
	bool* processed = new bool[oldvalue->links_.size()];
	for(mrs_natural i=0; i < (mrs_natural)oldvalue->links_.size(); ++i)
		processed[i] = false;

	//iterate over all the links
	MarControl* oldRootLink = NULL;
	vector<pair<MarControl*, MarControl*> >::iterator lit;
	lit = oldvalue->links_.begin();
	mrs_natural idx = 0;
	while(toProcess > 0)
	{
		//avoid processing processed links
		if(!processed[idx])
		{
			//check if this is the old root link and send it to the outSet (also as a root link)
			if(lit->first == lit->second)
			{
				oldRootLink = lit->first;
				outSet->push_back(*lit); 
				toProcess --;
				processed[idx]=true;
			}
			//check if this is this same control (i.e. the control to unlink at)
			//and set it as the root link of the inSet
			else if(lit->first == this)
			{
				#ifdef MARSYAS_QT
				//we must lock each linked MarControl, so we can
				//safely make it point to the new cloned MarControlValue
				//without interferences from other threads
				if(MarControlPtr(this) != lit->first) //this MarControl is already locked, so we must avoid a deadlock!
					lit->first->rwLock_.lockForWrite(); 
				#endif

				lit->first->value_ = newvalue;
				inSet->push_back(pair<MarControl*, MarControl*>(lit->first, lit->first));
				toProcess--;
				processed[idx]=true;

				#ifdef MARSYAS_QT
				lit->first->rwLock_.unlock();
				#endif
			}
			//check if this is directly linked to the old root link and if so,
			//send it to the outSet
			else if(lit->second == oldRootLink)
			{
				outSet->push_back(*lit); 
				toProcess --;
				processed[idx]=true;
			}
			//if this control links directly to the control we want to unlink, send it to the inSet
			else if(lit->second == this)
			{
				#ifdef MARSYAS_QT
				//we must lock each linked MarControl, so we can
				//safely make it point to the new cloned MarControlValue
				//without interferences from other threads
				if(MarControlPtr(this) != lit->first) //this MarControl is already locked, so we must avoid a deadlock!
					lit->first->rwLock_.lockForWrite(); 
				#endif
				
				lit->first->value_ = newvalue;
				inSet->push_back(*lit);
				toProcess--;
				processed[idx]=true;

				#ifdef MARSYAS_QT
				lit->first->rwLock_.unlock();
				#endif
			}
			//This control is not directly connected to any root link, so we have
			//to check to which set belongs the control it links to and assign it to
			//the same set
			else
			{
				bool found = false;
				vector<pair<MarControl*, MarControl*> >::iterator sit;
				//do a copy of the inSet because we are using iterators
				//and they become invalid if we change the size of the inSet object
				//inside the for loop below
				vector<pair<MarControl*, MarControl*> > inSet2 = *inSet;
				//start by searching in the inSet...
				for(sit = inSet2.begin(); sit != inSet2.end(); ++sit)
				{
					if(lit->second == sit->first)
					{
						#ifdef MARSYAS_QT
						//we must lock each linked MarControl, so we can
						//safely make it point to the new cloned MarControlValue
						//without interferences from other threads
						if(MarControlPtr(this) != lit->first) //this MarControl is already locked, so we must avoid a deadlock!
							lit->first->rwLock_.lockForWrite(); 
						#endif

						lit->first->value_ = newvalue;
						inSet->push_back(*lit);
						toProcess--;
						processed[idx]=true;
						found = true;

						#ifdef MARSYAS_QT
						lit->first->rwLock_.unlock();
						#endif
					}
				}
				//if not found there, look for it in the outSet...
				if(!found)
				{
					vector<pair<MarControl*, MarControl*> > outSet2 = *outSet;
					for(sit = outSet2.begin(); sit != outSet2.end(); ++sit)
					{
						if(lit->second == sit->first)
						{
							outSet->push_back(*lit);
							toProcess--;
							processed[idx]=true;
						}
					}
				}
			}
		}

		//iterate until all links were processed!
		if(lit!=oldvalue->links_.end())
		{
			lit++;
			idx++;
		}
		else
		{
			lit=oldvalue->links_.begin();
			idx = 0;
		}
	}

	delete [] processed;

	//set the two unlinked tables
	oldvalue->links_ = *outSet;
	newvalue->links_ = *inSet;

	delete inSet;
	delete outSet;

	//check if newValue has in fact any control
	//linking to it
	if(newvalue->links_.size() == 0)
		delete newvalue;

	#ifdef MARSYAS_QT
	oldvalue->rwLock_.unlock();
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
	if(value_->links_.size()-1 == 0)
		return false;
	else
		return true;
}

vector<pair<MarControlPtr, MarControlPtr> >
MarControl::getLinks()
{
#ifdef MARSYAS_QT
	QReadLocker r_locker(&rwLock_);
	QReadLocker (&(value_->rwLock_));
#endif

	vector<pair<MarControlPtr, MarControlPtr> > res;
	vector<pair<MarControl*, MarControl*> >::const_iterator lit;
	for(lit=value_->links_.begin(); lit != value_->links_.end(); ++lit)
	{
		res.push_back(pair<MarControlPtr, MarControlPtr>(MarControlPtr(lit->first),MarControlPtr(lit->second)));
	}
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
