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
	READ_LOCKER(rwLock_);
	return new MarControl(*this);
}

void
MarControl::setMarSystem(MarSystem* msys) 
{
	WRITE_LOCKER(rwLock_);
  msys_ = msys;
}

MarSystem*
MarControl::getMarSystem() 
{
	READ_LOCKER(rwLock_); 
  return msys_;
}

void
MarControl::setName(std::string cname)
{
	WRITE_LOCKER(rwLock_);
  cname_ = cname; 
}

std::string 
MarControl::getName() const
{
	READ_LOCKER(rwLock_);
  return cname_;
}

void 
MarControl::setState(bool state) 
{
	WRITE_LOCKER(rwLock_);  
  state_ = state;
}

bool 
MarControl::hasState() const
{
	READ_LOCKER(rwLock_);
  return state_;
}

string
MarControl::getType() const
{ 
	READ_LOCKER(rwLock_);
	READ_LOCKER(value_->valuerwLock_);
	return value_->getType(); 
}

void 
MarControl::callMarSystemUpdate()
{
	LOCK_FOR_READ(rwLock_);
	if (state_ && msys_)
	{
		MarSystem* msys = msys_;
		UNLOCK(rwLock_);
		msys->update(this);
		return;
	}
	UNLOCK(rwLock_);
}

bool
MarControl::linkTo(MarControlPtr ctrl, bool update)
{
	lockAllLinkedControls(ctrl());
	lockAllLinkedControls(this);

	if (ctrl.isInvalid())
	{
		ostringstream oss;
		oss << "MarControl::linkTo() - Linking to an invalid control ";
		oss << "(" << ctrl->cname_ << " with " << cname_ << ").";
		MRSWARN(oss.str());

		unlockAllLinkedControls(ctrl());
		unlockAllLinkedControls(this);
		return false;
	}

	//check if these controls are already linked
	//(i.e. they own the same MarControlValue) 
	if(value_ == ctrl->value_)
	{
		unlockAllLinkedControls(ctrl());
		unlockAllLinkedControls(this);
		return true;//already linked! :-)
	}
	
	if (ctrl->value_->type_ != value_->type_)
	{
		ostringstream oss;
		oss << "MarControl::linkTo() - Linking controls of different types ";
		oss << "(" << ctrl->cname_ << " with " << cname_ << ").";
		MRSWARN(oss.str());

		unlockAllLinkedControls(ctrl());
		unlockAllLinkedControls(this);
		return false;
	}

	//unlink this control (but keeping all links to it) 
	//before linking it again to the passed control
	this->unlinkFromTargetNonReentrant();

	//store a pointer to the (soon to be old) MarControlValue object,
	//so we can later delete it from memory
	MarControlValue* oldvalue = value_;
	//and get a pointer to the new value
	MarControlValue* newvalue = ctrl->value_;

	//get all the links of our current MarControlValue so we can also
	//re-link them to the passed ctrl
	vector<pair<MarControl*, MarControl*> >::iterator lit;
	for(lit=oldvalue->links_.begin(); lit!=oldvalue->links_.end(); ++lit)
	{
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
	}

	//unlockAllLinkedControls(ctrl());
	//unLockAllLinkedControls(this);

	//unlock the old MarControlValue object before deleting it
	UNLOCK(oldvalue->linksrwLock_);
	//old MarControlValue can and should now be safely deleted from memory
	delete oldvalue;

	//unlock all the linked controls, so we can call update without deadlocks
	//(since the links table is still locked, no changes can happen to value_ - these
	//would only happen in case of an unlink/link operation, which is never possible
	//to happen with the links table locked)
	for(lit=value_->links_.begin(); lit!=value_->links_.end(); ++lit)
	{
		UNLOCK(lit->first->rwLock_);
	}

	//check if it's needed to call update()
	if(update)
		value_->callMarSystemsUpdate();//newvalue->callMarSystemsUpdate();

	//finally unlock the links table of the new anc current value and we are done
	UNLOCK(value_->linksrwLock_);

	#ifdef MARSYAS_QT
	emitControlChanged(this);
	#endif

	return true;
}

void
MarControl::unlinkFromAll()
{
	lockAllLinkedControls(this);
		
	//first unlink this control from all the controls to which
	//it links to
	this->unlinkFromTargetNonReentrant();
	
	//get a list of all the controls that target this controls...
	vector<pair<MarControl*, MarControl*> >::iterator lit;
	vector<MarControl*> linkedControls;
	for(lit=value_->links_.begin(); lit!=value_->links_.end(); ++lit)
	{
		if(lit->second == this && lit->first != lit->second)
			linkedControls.push_back(lit->first);
	}
	//... and now unlink them all from this
	for(mrs_natural i=0; i < (mrs_natural)linkedControls.size(); ++i)
		linkedControls[i]->unlinkFromTargetNonReentrant();

	unlockAllLinkedControls(this);
}

void
MarControl::unlinkFromTarget()
{
	lockAllLinkedControls(this);

	unlinkFromTargetNonReentrant();

	unlockAllLinkedControls(this);
}

void
MarControl::lockAllLinkedControls(MarControl* ctrl)
{
#ifdef MARSYAS_MT
	lockedMutexes_.clear()
	vector<pair<MarControl*, MarControl*> >::iterator lit;
	LOCK_FOR_WRITE(ctrl->rwLock_);
	while(TRY_LOCK_FOR_READ(ctrl->value_->linksrwLock_, 500) == false)
	{
		//if this lock fails, we should unlock momentarily rwLock_ 
		//so we allow other blocked threads to continue, avoiding potential
		//deadlocks
		UNLOCK(ctrl->rwLock_);
		LOCK_FOR_WRITE(ctrl->rwLock_);
	}
	
	lockedMutexes_.push_back(&(ctrl_->rwLock_));

	//lock all linked controls for write
	for(lit=ctrl->value_->links_.begin(); lit!=ctrl->value_->links_.end(); ++lit)
	{
		//avoid locking again this control, which is already locked!
		if(lit->first != this)
		{
			LOCK_FOR_WRITE(lit->first->rwLock_);
			lockedMutexes_.push_back(&(lit->first->rwLock_));
		}
	}

	lockedMutexes_.push_back(&(ctrl->value_->linksrwLock_));
#endif
}

void
MarControl::unlockAllLinkedControls(MarControl* ctrl)
{
#ifdef MARSYAS_MT 
	for(mrs_natural i=0; i<ctrl->lockedMutexes.size(); ++i)
	{
		UNLOCK(*(ctrl->lockedMutexes_[i]));
	}
	ctrl->lockedMutexes_.clear();
#endif
}

void
MarControl::unlinkFromTargetNonReentrant()
{
	vector<pair<MarControl*, MarControl*> >::iterator lit;
	
	//check if this MarControl is linked
	//(i.e. more than one MarControl linking
	//to the MarControlValue).
	//if not, no point doing unlink - just return.
	if(value_->links_.size() <= 1)
		return;

	MarControlValue* oldvalue = value_;
	MarControlValue* newvalue = oldvalue->clone();

	//WRITE_LOCKER(newvalue->linksrwLock_);
	//WRITE_LOCKER(newvalue->valuerwLock_);

	vector<pair<MarControl*, MarControl*> >* inSet = new vector<pair<MarControl*, MarControl*> >;
	vector<pair<MarControl*, MarControl*> >* outSet = new vector<pair<MarControl*, MarControl*> >;

	mrs_natural toProcess = oldvalue->links_.size();
	bool* processed = new bool[oldvalue->links_.size()];
	for(mrs_natural i=0; i < (mrs_natural)oldvalue->links_.size(); ++i)
		processed[i] = false;

	//iterate over all the links
	MarControl* oldRootLink = NULL;
	//vector<pair<MarControl*, MarControl*> >::iterator lit;
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
				lit->first->value_ = newvalue; //"this" is already locked for writing
				inSet->push_back(pair<MarControl*, MarControl*>(lit->first, lit->first));
				toProcess--;
				processed[idx]=true;
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
				lit->first->value_ = newvalue;
				inSet->push_back(*lit);
				toProcess--;
				processed[idx]=true;
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
						lit->first->value_ = newvalue;
						inSet->push_back(*lit);
						toProcess--;
						processed[idx]=true;
						found = true;
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
}

bool
MarControl::isLinked() const
{
	READ_LOCKER(rwLock_);
	READ_LOCKER(value_->linksrwLock_);
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
	READ_LOCKER(rwLock_);
	READ_LOCKER(value_->linksrwLock_);

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
