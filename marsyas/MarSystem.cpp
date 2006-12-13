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
\class MarSystem
\brief MarSystem transforms a realvec

Abstract base class for any type of system. Basically a MarSystem
takes as input a vector of float numbers (realvec) and produces a new
vector (possibly with different dimensionality). Different types of
computation can be used. MarSystems are the core processing blocks of
Marsyas including transformations like FFT, Filter as well as feature
extractors like Spectral Centroid.
*/

#include "MarSystem.h"
#include "MarControlManager.h"

#ifdef MARSYAS_QT
#include "MarGUIManager.h"
#include "MarSystemNetworkGUI.h"
#include "MarSystemControlsGUI.h"
//#include "MarSystemDataGUI.h"
#include "MATLABeditorGUI.h"
#endif

using namespace std;
using namespace Marsyas;

MarSystem::MarSystem(string type, string name)
{
#ifdef MARSYAS_QT
	processMutex_ = new QMutex(QMutex::Recursive);
	msysNetGUI_ = NULL;
	MATLABeditorGUI_ = NULL;
#endif

	parent_ = NULL;
	name_ = name;
	type_ = type;
	prefix_ = "/" + type_ + "/" + name_ + "/";
	absPath_ = prefix_;

	isComposite_ = false;
	marsystemsSize_ = 0;

	MATLABscript_ = "";

	scheduler_.removeAll();
	TmTimer* t = new TmSampleCount(NULL, this, "mrs_natural/inSamples");
	scheduler_.addTimer(t);
	delete t;

	//add default controls that 
	//all MarSystems should have
	addControls();
}

// copy constructor 
MarSystem::MarSystem(const MarSystem& a)
{
#ifdef MARSYAS_QT
	processMutex_ = new QMutex(QMutex::Recursive);
	msysNetGUI_ = NULL;
	MATLABeditorGUI_ = NULL;
#endif

	parent_ = NULL;
	type_ = a.type_;
	name_ = a.name_;
	prefix_ = a.prefix_;
	absPath_ = a.absPath_;

	MATLABscript_ = a.MATLABscript_;

	//clone controls
	{
		#ifdef MARSYAS_QT
		QWriteLocker locker_w(&rwLock_);
		QReadLocker locker_r(&(a.rwLock_));
		#endif

		controls_.clear();
		for(ctrlIter_ = a.controls_.begin(); ctrlIter_ != a.controls_.end(); ++ctrlIter_)
		{
			//clone all controls
			controls_[ctrlIter_->first] = ctrlIter_->second->clone();		
			//set new MarSystem parent
			controls_[ctrlIter_->first]->setMarSystem(this);
		}
	}

	//update the "references" to the controls
	ctrl_inSamples_ = getctrl("mrs_natural/inSamples");
	ctrl_inObservations_ = getctrl("mrs_natural/inObservations");
	ctrl_israte_ = getctrl("mrs_real/israte");
	ctrl_inObsNames_ = getctrl("mrs_string/inObsNames");
	ctrl_onSamples_ = getctrl("mrs_natural/onSamples");
	ctrl_onObservations_ = getctrl("mrs_natural/onObservations");
	ctrl_osrate_ = getctrl("mrs_real/osrate");
	ctrl_onObsNames_ = getctrl("mrs_string/onObsNames");
	ctrl_debug_ = getctrl("mrs_bool/debug"); 
	ctrl_mute_ = getctrl("mrs_bool/mute");
	ctrl_active_ = getctrl("mrs_bool/active");

	//clone children (if any)
	isComposite_ = a.isComposite_;
	marsystemsSize_ = 0;
	if(isComposite_)
	{
		for (mrs_natural i=0; i< a.marsystemsSize_; i++)
		{
			addMarSystem((*a.marsystems_[i]).clone());
		}
	}

	// "re-link" controls
	for(ctrlIter_ = a.controls_.begin(); ctrlIter_ != a.controls_.end(); ++ctrlIter_)
	{
		// get original links...
		vector<MarControlPtr> protolinks = ctrlIter_->second->getLinks();
		//... and re-establish links between the new cloned controls
		vector<MarControlPtr>::const_iterator ci;
		for (ci = protolinks.begin(); ci != protolinks.end(); ++ci)
		{
			controls_[ctrlIter_->first]->getLinks().clear(); // clear clone's links table
			MarControlPtr ctrl = this->getControl((*ci)->getMarSystem()->getAbsPath() + (*ci)->getName(), true);
			if (!ctrl.isInvalid())
			{
				controls_[ctrlIter_->first]->linkTo(ctrl);
			}
		}
	}

	//recreate schedule objects
	scheduler_.removeAll();
	TmTimer* t = new TmSampleCount(NULL, this, "mrs_natural/inSamples");
	scheduler_.addTimer(t);
	delete t;
}

MarSystem::~MarSystem()
{
	//delete children (if any)
	for (mrs_natural i=0; i< marsystemsSize_; i++)  
	{
		delete marsystems_[i];
	} 

#ifdef MARSYAS_QT
	delete processMutex_;
	delete msysNetGUI_;
	delete MATLABeditorGUI_;
	
	//this closes all opened GUIs when the MarSystem is destroyed
	//and clears the internal hash table used to store its pointers
	QList<MarSystemControlsGUI*> controlsGUIs = activeControlsGUIs_.values();
	for(int i = 0; i < controlsGUIs.size(); ++i) 
	{
		delete controlsGUIs[i];
	}
	QList<QWidget*> dataGUIs = activeDataGUIs_.values();
	for(int i = 0; i < dataGUIs.size(); ++i) 
	{
		delete dataGUIs[i];
	}
#endif
}

void
MarSystem::addControls()
{
	//input pin controls (with state)
	addctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES, ctrl_inSamples_);
	setctrlState(ctrl_inSamples_, true);
	addctrl("mrs_natural/inObservations", MRS_DEFAULT_SLICE_NOBSERVATIONS, ctrl_inObservations_);
	setctrlState(ctrl_inObservations_, true);
	addctrl("mrs_real/israte", MRS_DEFAULT_SLICE_SRATE, ctrl_israte_);
	setctrlState(ctrl_israte_, true);
	addctrl("mrs_string/inObsNames", ",", ctrl_inObsNames_);
	setctrlState(ctrl_inObsNames_, true);

	//output pin controls (stateless)
	addctrl("mrs_natural/onSamples", MRS_DEFAULT_SLICE_NSAMPLES, ctrl_onSamples_);
	addctrl("mrs_natural/onObservations", MRS_DEFAULT_SLICE_NOBSERVATIONS, ctrl_onObservations_);
	addctrl("mrs_real/osrate", MRS_DEFAULT_SLICE_SRATE, ctrl_osrate_);
	addctrl("mrs_string/onObsNames", ",", ctrl_onObsNames_);

	inObservations_ = ctrl_inObservations_->to<mrs_natural>();
	inSamples_ = ctrl_inSamples_->to<mrs_natural>();
	onObservations_ = ctrl_onObservations_->to<mrs_natural>();
	onSamples_ = ctrl_onSamples_->to<mrs_natural>();

	//other controls:
	addctrl("mrs_bool/debug", false, ctrl_debug_);		//no debug by default
	addctrl("mrs_bool/mute", false, ctrl_mute_);			//unmuted by default
	addctrl("mrs_bool/active",true, ctrl_active_);		//active by default
	ctrl_active_->setState(true);

	active_ = ctrl_active_->to<bool>();
}

bool 
MarSystem::addMarSystem(MarSystem *marsystem)
{
	//idiot proof 1
	if(this == marsystem)
	{
		MRSWARN("MarSystem::addMarSystem - Trying to add MarSystem to itself - failing...");
		return false;
	}

	//idiot proof 2 
	MarSystem* msys = parent_;
	while(msys)
	{
		if(msys == marsystem)
		{
			MRSWARN("MarSystem::addMarSystem - Trying to add an ancestor MarSystem as a child - failing...");
			return false;
		}
		msys = msys->parent_;
	}

	//it's only possible to add MarSystems to Composites
	if(isComposite_)
	{
		vector<MarSystem*>::iterator it;
		bool replaced = false;
		//check if a child MarSystem with the same type/name
		//exists. If it does, replace it with the new one.
		for(it = marsystems_.begin(); it != marsystems_.end(); ++it)
		{
			if((*it)->getName() == marsystem->getName() &&
				(*it)->getType() == marsystem->getType())
			{
				//delete current child MarSystem
				delete (*it);
				//and replace it with the new one
				(*it) = marsystem;
				replaced = true;
				break;
			}
		}
		//if no replacement took place, then add the
		//new MarSystem as a new child
		if (!replaced)
		{
			marsystems_.push_back(marsystem);
			marsystemsSize_ = (mrs_natural)marsystems_.size();
		}
		//set parent for the new child MarSystem
		marsystem->setParent(this);
		//update child MarSystem
		marsystem->update();
		//update parent MarSystem
		update();
		return true;
	}
	else
	{
		MRSWARN("MarSystem::addMarSystem -Trying to add MarSystem to a non-Composite - failing...");
		return false;
	}
}

MarSystem*
MarSystem::getMarSystem(std::string absPath)
{
	if(absPath == prefix_)
		return this;
	else if(isComposite_)
	{
		string childPath;
		if(absPath.length() > prefix_.length())
			childPath = absPath.substr(prefix_.length()-1, absPath.length()); //includes leading "/" [!]
		else
		{
			MRSWARN("MarSystem::getMarsystem(): " + absPath + " not found!");
			return NULL;
		}

		vector<MarSystem*>::const_iterator iter;
		for(iter = marsystems_.begin(); iter != marsystems_.end(); ++iter)
		{
			MarSystem* msys = (*iter)->getMarSystem(childPath);
			if (msys)
				return msys;
		}

		MRSWARN("MarSystem::getMarsystem(): " + absPath + " not found!");
		return NULL;
	}
	else
	{
		MRSWARN("MarSystem::getMarsystem(): " + absPath + " not found!");
		return NULL;
	}
}

void
MarSystem::setParent(const MarSystem* parent)
{
	parent_ = const_cast<MarSystem*>(parent);

	if(parent)
	{
		//if absPath_ was "/Gain/g/" it will become 
		//"/Series/s" + "/Gain/g/" = "/Series/s/Gain/g/"
		absPath_ = parent_->absPath_.substr(0, parent_->absPath_.length()-1) + prefix_;

		if(isComposite_)
		{
			//...and re-set parent and propagate the new path 
			// to all children
			for (mrs_natural i=0; i< marsystemsSize_; i++)
			{
				marsystems_[i]->setParent(this);
			}
		}
	}
}

void
MarSystem::setName(string name)
{
	if (name == name_)
		return;

	string oldPrefix = prefix_;
	prefix_ = "/" + type_ + "/" + name + "/";
	name_ = name;

#ifdef MARSYAS_QT
	rwLock_.unlock();//unlock writing mutex
#endif

	//update path accordingly
	string::size_type pos = absPath_.find(oldPrefix, 0);
	string uppath = absPath_.substr(0, pos);
	string downpath = absPath_.substr(oldPrefix.length()+pos, absPath_.length()-(oldPrefix.length()+pos));
	absPath_ = uppath + prefix_ + downpath;

	if (isComposite_)
	{
		for (int i=0; i<marsystemsSize_; i++)
		{
			marsystems_[i]->updatePath();
		}
	}
}

void
MarSystem::setType(string type)
{
	if (type == type_)
		return;

	string oldPrefix = prefix_;
	prefix_ = "/" + type + "/" + name_ + "/";
	type_ = type;

	//update path accordingly
	string::size_type pos = absPath_.find_last_of(oldPrefix, 0);
	string uppath = absPath_.substr(0, pos);
	string downpath = absPath_.substr(oldPrefix.length()+pos, absPath_.length()-(oldPrefix.length()+pos));
	absPath_ = uppath + prefix_ + downpath;

	if (isComposite_)
	{
		for (int i=0; i<marsystemsSize_; i++)
		{
			marsystems_[i]->updatePath();
		}
	}
}

string 
MarSystem::getType() const
{
	return type_;
}

string 
MarSystem::getName() const
{
	return name_;
}

string
MarSystem::getPrefix() const
{
	return prefix_;
}

string
MarSystem::getAbsPath() const
{
	return absPath_;
}

void
MarSystem::updatePath()
{
	if (parent_)
	{
		absPath_ = parent_->getAbsPath() + type_ + '/' + name_ + '/';
	}
	else
	{
		absPath_ = prefix_;
	}
}

void 
MarSystem::checkFlow(realvec& in, realvec& out)
{
	mrs_natural irows = in.getRows();
	mrs_natural icols = in.getCols();
	mrs_natural orows = out.getRows();
	mrs_natural ocols = out.getCols();

	if(getctrl("mrs_bool/debug")->isTrue())
	{
		MRSWARN("Debug CheckFlow Information");
		MRSWARN("MarSystem Type    = " << type_); 
		MRSWARN("MarSystem Name    = " << name_);
		MRSWARN("inObservations_ = " << inObservations_);
		MRSWARN("inSamples_ = " << inSamples_);
		MRSWARN("onObservations_ = " << onObservations_);
		MRSWARN("onSamples_ = " << onSamples_);      
		MRSWARN("Input  Slice Rows = " << irows ); 
		MRSWARN("Input  Slice Cols = " << icols ); 
		MRSWARN("Output Slice Rows = " << orows );
		MRSWARN("Output Slice Cols = " << ocols );      
	}

	MRSASSERT(irows == inObservations_);
	MRSASSERT(icols == inSamples_);
	MRSASSERT(orows == onObservations_);
	MRSASSERT(ocols == onSamples_);
}

void
MarSystem::process(realvec& in, realvec& out)
{
#ifdef MARSYAS_QT
	processMutex_->lock();
#endif

	checkFlow(in, out);

	myProcess(in, out);

#ifdef MARSYAS_MATLAB
	if(!MATLABscript_.empty())
	{
		MATLAB_PUT(in, name_ + "_in");
		MATLAB_PUT(out, name_ + "_out");
		MATLAB_EVAL(MATLABscript_);
		MATLAB_GET(name_+"_out", out);

		//check if out realvec was shortened by MATLAB script... //[!]
		if((out.getRows() < onObservations_)||(out.getCols() < onSamples_))
			out.stretch(onObservations_, onSamples_);
	}
#endif

#ifdef MARSYAS_QT
	processMutex_->unlock();
	//emit processed();
#endif
}

void 
MarSystem::tick()
{
	//if MarSystem is not active, ignore ticks
	if(ctrl_active_->isTrue())
	{
		scheduler_.tick();
		process(inTick_,outTick_);
	}
	else
		MRSDIAG("MarSystem::tick() : MarSystem is not active! Ignoring tick command.");
}

void 
MarSystem::myUpdate()
{
	MRSDIAG("MarSystem.cpp - MarSystem:myUpdate");

	//lmartins:
	//By default, a MarSystem does not modify the input data stream format.
	//Override this method on a derived MarSystem if data format changes
	//should take place...
	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
	ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
	ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
	ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
}

void 
MarSystem::update(MarControlPtr sender)
{
#ifdef MARSYAS_QT
	processMutex_->lock();
#endif

	MRSDIAG("MarSystem.cpp - MarSystem:Update");

	//sync input member variables
	inObservations_ = ctrl_inObservations_->to<mrs_natural>();
	inSamples_ = ctrl_inSamples_->to<mrs_natural>();
	israte_ = ctrl_israte_->to<mrs_real>();
	//sync output member variables
	onObservations_ = ctrl_onObservations_->to<mrs_natural>();
	onSamples_ = ctrl_onSamples_->to<mrs_natural>();
	osrate_ = ctrl_osrate_->to<mrs_real>();

	//call derived class specific update
	myUpdate();

	//sync input member variables
	inObservations_ = ctrl_inObservations_->to<mrs_natural>();
	inSamples_ = ctrl_inSamples_->to<mrs_natural>();
	israte_ = ctrl_israte_->to<mrs_real>();
	//sync output member variables
	onObservations_ = ctrl_onObservations_->to<mrs_natural>();
	onSamples_ = ctrl_onSamples_->to<mrs_natural>();
	osrate_ = ctrl_osrate_->to<mrs_real>();

	//check active status
	bool active = ctrl_active_->isTrue();
	//if active status changed...
	if(active_ !=  active)
	{
		active_ = active;
		activate(active);
	}

	//resize input and output realvec if necessary
	if ((inObservations_ != inTick_.getRows()) ||
		(inSamples_ != inTick_.getCols())      ||
		(onObservations_ != outTick_.getRows()) ||
		(onSamples_ != outTick_.getCols()))
	{
		inTick_.create(inObservations_, inSamples_);
		outTick_.create(onObservations_, onSamples_);
	}

#ifdef MARSYAS_QT
	processMutex_->unlock();
#endif
}

void
MarSystem::activate(bool state) //non-thread-safe, but this method is only supposed to be called from update(), which is thread-safe
{
	//since this method must be public (so it can be called in Composite::activate())
	//we must guarantee that the "mrs_bool/active" control is in sync with any eventual 
	//direct calls to MarSystem::activate() from client code
	if(ctrl_active_->to<bool>() != state)
	{
		ctrl_active_->setValue(state, NOUPDATE);
		active_ = state;
	}

	//execute any code needed to run when activating/deactivating
	//the derived MarSystem
	localActivate(state);
}

void
MarSystem::localActivate(bool state)
{
	//call activate for all Composite's components
	if(isComposite_)
	{
		for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			//marsystems_[i]->activate(state);
			marsystems_[i]->updctrl("mrs_bool/active", state); //thread-safe
		}
	}
}


string
MarSystem::getControlRelativePath(string cname) const
{
	//check for an absolute path 
	if(cname[0] == '/')
	{
		//is this absolute path pointing to this MarSystem?
		if(cname.substr(0, absPath_.length()) == absPath_)
		{
			//just remove the absolute path
			cname = cname.substr(0, absPath_.length());
		}
		else
			return "";
	}
	
	//if cname is already a relative path
	//just return it (do nothing)
	return cname;
}

string
MarSystem::getControlLocalPath(string cname) const
{
	cname = getControlRelativePath(cname);

	//a local path should only have one '/' (e.g. type/name), 
	//otherwise it's probably a control from a child MarSystem
	if(cname.find_first_of('/') == cname.find_last_of('/') && 
		cname.find_first_of('/') != -1)
	{
		return cname;
	}
	else
	{
		string prefix = type_ + "/" + name_;
		string::size_type pos = cname.find(prefix, 0);
		if(pos != -1)
		{
			cname = cname.substr(prefix_.length()-1, cname.length());
			return cname;
		}
		else
			return cname;

	}

	//if not a local control path, return empty string
	//return "";
}

bool
MarSystem::linkControl(string cname1, string cname2) 
{
	//first control has to be a local control
	string localPath = this->getControlLocalPath(cname1);
	if(localPath == "")
	{
		MRSWARN("MarSystem::linkControl first control has to be a local control (" + cname1 + ")");
		return false;
	}
	
	//a control is inherently connected to itself!
	if(localPath == getControlLocalPath(cname2))
	{
		return true;
	}

	//try to get the controls
	MarControlPtr ctrl1 = this->getControlLocal(localPath);//search only local controls
	MarControlPtr ctrl2 = this->getControl(cname2, true);//search everywhere in the network

	//make sure 2nd control exists somewhere in the network
	if(ctrl2.isInvalid())
	{
		//MRSWARN("MarSystem::linkControl - control does not exist anywhere: " + cname2);
		return false;
	}
	
	//now check if the first control already exists or if we have
	// to add it to this MarSystem
	if(ctrl1.isInvalid())
	{
		if(!addControl(localPath, ctrl2->clone(), ctrl1))
			return false; //some error occurred...
	}

	//now both controls exist 
	//just check their types, and link them
	if(ctrl1->getType() == ctrl2->getType())
		return ctrl1->linkTo(ctrl2);
	else
	{
		MRSWARN("MarSystem::linkControl control type mismatch (" + ctrl1->getName() + "!=" + ctrl2->getName() + ")");
		return false;
	}
}

MarControlPtr
MarSystem::getControl(string cname, bool searchParent, MarSystem* excludedFromSearch)
{
	//USE A CACHE FOR MORE EFFICIENT LOOK-UP?? [!]

	//check if this is an absolute control pathname
	if(cname[0] == '/')
	{
		//check for this MarSystem's own path
		if(cname.substr(0, absPath_.length()) == absPath_)
		{
			//just remove the path and look for it internally
			//or among children
			cname = cname.substr(absPath_.length(), cname.length());
		}
		//not a control from this MarSystem or its children
		//If allowed, ask parent for searching for it on the 
		//remaining of the network
		else
		{
			//exclude this MarSystem when asking parent to search for the control
			//since it's known that it does not belong to here or any of the children
			if(searchParent && parent_)
				return parent_->getControl(cname, true, this);
			else
			{
				//MRSWARN("MarSystem::getControl - Unsupported control name: " + cname);
				return MarControlPtr();
			}
		}
	}
	
	// This is a relative path, so now get the local path
	string localcname = getControlLocalPath(cname);

	// start by checking if it is a local control
	//(i.e. owned by the MarSystem itself and not its children)
	if(controls_.find(localcname) != controls_.end())
	{
		#ifdef MARSYAS_QT
		QReadLocker locker(&rwLock_); //reading controls_ [!]
		#endif
		return controls_[localcname];
	}
	else if(isComposite_) //if not a local control, then search it among children
	{
		MarControlPtr childCtrl;
		//iterate recursively over children and just
		//return the first control that matches the requested one
		for (mrs_natural i=0; i< marsystemsSize_; i++)
		{
			//skip the child MarSystem where this search originated at
			//(already looked for controls at that child, so no point
			//doing it all over again)
			if(marsystems_[i] == excludedFromSearch)
				continue;//skip this child!

			//search for controls in children

			 /***** THIS WAS THE PORTUGESE VERSION *****/ 
			//childCtrl = marsystems_[i]->getControl(cname);
			// changed by gtzan to 

			childCtrl = marsystems_[i]->getControl(localcname);


			if(!childCtrl.isInvalid())
				return childCtrl;
		}
	}

	//if no control found anywhere, this is an unsupported control!
	//MRSWARN("MarSystem::getControl - Unsupported control name = " + cname);
	return MarControlPtr();
}

MarControlPtr
MarSystem::getControlLocal(string cname)
{
	if(controls_.find(cname) != controls_.end())
	{
		#ifdef MARSYAS_QT
		QReadLocker locker(&rwLock_); //reading controls_ [!]
		#endif
		return controls_[cname];
	}
	else
		return MarControlPtr();
}

bool 
MarSystem::hasControlState(string cname)
{
	MarControlPtr control = getControl(cname);
	if(control.isInvalid())
	{
		MRSWARN("MarSystem::hasControlState Unsupported control name = " + cname);
		return false;
	}
	else
		return control->hasState();
}

void
MarSystem::setControlState(string cname, bool state)
{
	MarControlPtr control = getControl(cname);
	if(control.isInvalid())
	{
		MRSWARN("MarControls::setState Unsupported control name = " + cname);
	}
	else
		control->setState(state);
}

bool 
MarSystem::hasControl(string cname) 
{
#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_);//reading controls_ [!]
#endif

	//look for local and child controls only!
	MarControlPtr control = this->getControl(cname);
	return !control.isInvalid();
}

bool
MarSystem::hasControlLocal(string cname)
{
	//get the local control pathname (if it exists!)
	cname = getControlLocalPath(cname);
	
	//if the local path is inexistent, then the control does not exist
	//in this MarSystem!
	if(cname == "")
		return false;

	#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_); //reading controls_ [!]
	#endif
	
	//if the local path exists, then see if the control itself really exists
	return(controls_.find(cname)!=controls_.end());

}

bool 
MarSystem::updControl(MarControlPtr control, MarControlPtr newcontrol, bool upd)
{
	// check if the control really exists locally or among children
	if(control.isInvalid())
	{
	  MRSWARN("MarSystem::updControl - Invalid control ptr");
		MRSWARN("MarSystem::updControl - Composite name = " + name_);
		return false;
	}

	// since the control exists somewhere, set its value...
	if(!control->setValue(newcontrol, upd))
		return false; //some error occurred in setValue()

	//in case this is a composite Marsystem,
	if(isComposite_)
	{
		// get the control name
		string cname = control->getName();

		// call update (only if the control has state,
		// upd is true, and if it's not a local control (otherwise update 
		// was already called by control->setValue())).
		if(upd && control->hasState() && !hasControlLocal(cname))
			update();

		// certain controls must also be propagated to its children
		// (must find a way to avoid this hard-coded control list, though! [!] )
		if ((cname == "mrs_natural/inSamples")|| 
			(cname == "mrs_natural/inObservations")||
			(cname == "mrs_real/israte")||
			(cname == "mrs_string/inObsNames"))
		{
			//if there is at least a child MarSystem in this composite...
			if (marsystemsSize_ > 0)
			{
				if(!marsystems_[0]->updctrl(cname, newcontrol, upd))
					return false;//some error occurred in updctrl()
				if(upd && marsystems_[0]->hasControlState(cname))
					update();
			}
		}
	}

	//success!
	return true;
}

void
MarSystem::controlUpdate(MarControlPtr ctrl)
{
	//this method is called by MarControl each time the value of
	//the control (if it has state) is modified.
	//It is possible to define specialized "update" functions for
	//different controls, if needed...

	// TODO: simple fix, but this method needs to be recoded
	MarControlPtr ctrlcpy = ctrl->clone();

	// check local controls 
	for (ctrlIter_ = controls_.begin(); ctrlIter_ != controls_.end(); ++ctrlIter_)
	{
		//compare MarControl* (the actual pointer and not its value)
		if((ctrlIter_->second)() == ctrl())
		{
			update();
			break;
		}
	}
	
	if(isComposite_ && marsystemsSize_ > 0)
	{
		string cname = ctrl->getName();
		// (should find a way to avoid this hard-coded check...) [!]
		// default controls - semantics of composites 
		if ((cname == "mrs_natural/inSamples")||
			(cname == "mrs_natural/inObservations")||
			(cname == "mrs_real/israte")||
			(cname == "mrs_string/inObsNames"))
		{
			//marsystems_[0]->updctrl(cname, ctrl);
			marsystems_[0]->getctrl(cname)->setValue(ctrlcpy, true);
			update();
			return;
		}
	}
}

const map<string, MarControlPtr>&
MarSystem::getControls()
{
#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_); //reading controls_ [!] 
#endif

	return controls_;
}

vector<MarSystem*>
MarSystem::getChildren()
{
	return marsystems_;
}

bool
MarSystem::addControl(string cname, MarControlPtr v, MarControlPtr& ptr)
{
	if(addControl(cname, v))
	{
		ptr = controls_[cname];
		return true;
	}
	else
	{
		ptr = MarControlPtr();//return invalid control
		return false;
	}
}

bool
MarSystem::addControl(string cname, MarControlPtr v)
{
	//check for type mismatch between cname string (which include type information)
	//and the actual control type passed as an argument
	string::size_type pos = cname.find("/", 0);
	string ctype = cname.substr(0,pos);
	if(ctype!= v->getType())
	{
		MRSWARN("MarSystem::addControl control type mismatch (" + ctype + "!=" + v->getType() + ")");
		return false;
	}

#ifdef MARSYAS_QT
	QWriteLocker locker(&rwLock_); //writing controls_
#endif
	controls_[cname] = v;
	controls_[cname]->setMarSystem(this);
	controls_[cname]->setName(cname);

	//success!
	return true;
}

void 
MarSystem::updctrl(MarEvent* me) 
{
	if (me != NULL) 
	{
		me->dispatch();
		delete(me);
	}
}

/* this method clashes with updctrl(string cname, MarControlValue val)
when val=0, which can be interpreted as ev=NULL
void MarSystem::updctrl(string  time, MarEvent* ev) {
scheduler_.post(time, Repeat("",0), ev);
}
*/

void 
MarSystem::updctrl(string  time, Repeat rep, MarEvent* ev) 
{
	scheduler_.post(time, rep, ev);
}

/*
void
MarSystem::updctrl(Repeat rep, MarEvent* ev)
{
scheduler_.post("0", rep, ev);
}
*/

void
MarSystem::updctrl(string time, string cname, MarControlPtr control)
{
	scheduler_.post(time, Repeat(), new EvValUpd(this,cname,control));
}

void
MarSystem::updctrl(string time, Repeat rep, string cname, MarControlPtr control)
{
	scheduler_.post(time, rep, new EvValUpd(this,cname,control));
}

/*
void
MarSystem::updctrl(Repeat rep, string cname, MarControlPtr control)
{
scheduler_.post("0", rep, new EvValUpd(this,cname,control));
}
*/

void
MarSystem::updctrl(TmTime t, MarEvent* ev)
{
	scheduler_.post(t,Repeat(),ev);
}

void
MarSystem::updctrl(TmTime t, Repeat r, MarEvent* ev)
{
	scheduler_.post(t,r,ev);
}

void
MarSystem::updctrl(TmTime t, string cname, MarControlPtr control)
{
	scheduler_.post(t,Repeat(),new EvValUpd(this,cname,control));
}

void
MarSystem::updctrl(TmTime t, Repeat r, string cname, MarControlPtr control)
{
	scheduler_.post(t,r,new EvValUpd(this,cname,control));
}

void
MarSystem::addTimer(TmTimer* t)
{
	scheduler_.addTimer(t);
}

void
MarSystem::removeTimer(string name)
{
	scheduler_.removeTimer(name);
}

void
MarSystem::addTimer(std::string class_name, std::string identifier)
{
	scheduler_.addTimer(class_name,identifier);
}
void
MarSystem::updtimer(std::string cname, TmControlValue value)
{
	scheduler_.updtimer(cname,value);
}
mrs_natural
MarSystem::getTime(string timer_name) {
	return scheduler_.getTime(timer_name);
}

void
MarSystem::setMATLABscript(std::string script) 
{
#ifdef MARSYAS_QT
	processMutex_->lock();
#endif

	MATLABscript_ = script;

#ifdef MARSYAS_QT
	processMutex_->unlock();
#endif
}

string
MarSystem::getMATLABscript()
{
#ifdef MARSYAS_QT
	processMutex_->lock();
#endif

	return MATLABscript_;

#ifdef MARSYAS_QT
	processMutex_->unlock();
#endif
}

mrs_real* 
const MarSystem::recvControls() 
{
	return NULL;
}

// write *this to s 
ostream&
MarSystem::put(ostream &o) 
{
	if(isComposite_)
	{
		o << "# MarSystemComposite" << endl;
	}
	else
	{
		o << "# MarSystem" << endl;
	}
	o << "# Type = " << type_ << endl;
	o << "# Name = " << name_ << endl;

	o << endl;
	o << "# MarControls = " << controls_.size() << endl;
	for (ctrlIter_=controls_.begin(); ctrlIter_ != controls_.end(); ++ctrlIter_)
	{
		o << "# " << ctrlIter_->first << " = " << ctrlIter_->second << endl;
		std::vector<MarControlPtr> links = ctrlIter_->second->getLinks();
		o << "# Links = " << links.size() << endl;
		for (size_t i=0; i<links.size(); i++)
		{
			o << "# " << links[i]->getMarSystem()->getAbsPath() << links[i]->getName() << endl;
		}
	}

	if(isComposite_)
	{
		o << endl;
		o << "# nComponents = " << marsystemsSize_ << endl;
		o << endl;

		for (mrs_natural i=0; i < marsystemsSize_; i++)
			o << *(marsystems_[i]) << endl;
	}

	return o;	
}

ostream& 
Marsyas::operator<< (ostream& o, MarSystem& sys)
{
	sys.put(o);
	return o;
}

istream& 
Marsyas::operator>> (istream& is, MarSystem& msys)  
{
	// #ifdef MARSYAS_QT
	// 	QWriteLocker locker(&(c.rwLock_)); // this would create deadlocks with the code below...
	// #endif 

	string skipstr;

	is >> skipstr >> skipstr >> skipstr;

	mrs_natural nControls;
	is >> nControls;

	mrs_natural i;
	string type;
	string rstr = "mrs_real";
	string nstr = "mrs_natural";
	string bstr = "mrs_bool";
	string sstr = "mrs_string";
	string vstr = "mrs_realvec";
	mrs_real   rcvalue;
	string scvalue;
	mrs_natural ncvalue;
	bool bcvalue;
	string cname;
	map<string, MarControlPtr>::iterator iter;

	// if composite, clear all children to avoid bad links to prototype children
	if (msys.isComposite_)
	{
		for (mrs_natural i=0; i< msys.marsystemsSize_; i++)  
		{
			delete msys.marsystems_[i];
		} 
		msys.marsystems_.clear();
		msys.marsystemsSize_ = 0;
	}

	for (i=0; i < nControls; i++)
	{
		is >> skipstr;
		is >> cname;

		string ctype;
		ctype = cname.substr(0, cname.rfind("/", cname.length()));

		iter = msys.controls_.find(cname);

		if (ctype == rstr)
		{
			is >> skipstr >> rcvalue;
			if (iter == msys.controls_.end())
				msys.addControl(cname, rcvalue);//deadlocks if using mutexes for object "c"![!]
			else
				msys.updControl(cname, rcvalue);//deadlocks if using mutexes for object "c"![!]
		}
		else if (ctype == sstr)
		{
			is >> skipstr >> scvalue;

			if (iter == msys.controls_.end())
				msys.addControl(cname, scvalue);//deadlocks if using mutexes for object "c"![!]
			else
				msys.updControl(cname, scvalue);//deadlocks if using mutexes for object "c"![!]
		}
		else if (ctype == nstr)
		{
			is >> skipstr >> ncvalue;
			if (iter == msys.controls_.end())
				msys.addControl(cname, ncvalue);//deadlocks if using mutexes for object "c"![!]
			else
				msys.updControl(cname, ncvalue);//deadlocks if using mutexes for object "c"![!]
		}
		else if (ctype == bstr)
		{
			is >> skipstr >> bcvalue;

			if (iter == msys.controls_.end())
				msys.addControl(cname, bcvalue);//deadlocks if using mutexes for object "c"![!]
			else
				msys.updControl(cname, bcvalue);//deadlocks if using mutexes for object "c"![!]
		}
		else if (ctype == vstr)
		{
			realvec vcvalue;
			is >> skipstr >> vcvalue;

			if (iter == msys.controls_.end())
				msys.addControl(cname, vcvalue);//deadlocks if using mutexes for object "c"![!]
			else
				msys.updControl(cname, vcvalue);//deadlocks if using mutexes for object "c"![!]
		}
		else
		{
			is >> skipstr;
			MarControlPtr ctrl = MarControlManager::getManager()->createFromStream(ctype, is);
			if (iter == msys.controls_.end())
				msys.addControl(cname, ctrl);//deadlocks if using mutexes for object "c"![!]
			else
				msys.updControl(cname, ctrl);//deadlocks if using mutexes for object "c"![!]
		}

		// read links
		int nLinks;
		string linkto;
		is >> skipstr >> skipstr >> skipstr;
		is >> nLinks;
		//clean all links for current control
		MarControlPtr curCtrl = msys.getControlLocal(cname);
		curCtrl->getLinks().clear();
		//remake all links
		for (int i=0; i<nLinks; i++)
		{
			is >> skipstr >> linkto;
			msys.linkControl(cname, linkto);
		}
	}
	return is;
}

ostream&
Marsyas::operator<< (ostream& o, const map<string,MarControlPtr>& c) 
{
	//lock map<string,MarControlPtr>& c for read? [?]
	o << "# MarControls = " << c.size() << endl;
	map<string, MarControlPtr>::const_iterator iter;
	for (iter=c.begin(); iter != c.end(); ++iter)
	{
		o << "# " << iter->first << " = " << iter->second << endl;
	}
	return o; 
}


//**************************************************************************
//	MARSYAS_QT only methods
//**************************************************************************
#ifdef MARSYAS_QT

QMainWindow*
MarSystem::getMarSystemNetworkGUI(QWidget* parent, Qt::WFlags f)
{
	//if a network viewer already exists (i.e. is being displayed)
	//close it first in order to allow creating a new one
	//(this avoids any container window from being "empty")
	if(msysNetGUI_)
		return NULL;

	//create the Dialog
	msysNetGUI_ = new MarSystemNetworkGUI(this, parent, f);
	msysNetGUI_->setObjectName("MarSystemNetworkGUI");
	//string prefix = prefix_.substr(0, prefix_.length()-1);//remove trailing "/"
	msysNetGUI_->setWindowTitle(QString::fromStdString(prefix_) + " network");

	connect(msysNetGUI_, SIGNAL(destroyed(QObject*)),
		this, SLOT(GUIdestroyed(QObject*)));

	//if no parent widget is specified, open the controls dialog
	//as an independent window and return a NULL pointer so it can
	//not be deleted by mistake. The MarSystemControlsGUI class, when
	//created without a parent deletes itself on close.
	if(!parent)
	{
		msysNetGUI_->setAttribute(Qt::WA_DeleteOnClose, true);
		msysNetGUI_->show();
		return NULL;
	}
	else
		return msysNetGUI_;
}

QMainWindow*
MarSystem::getMATLABeditorGUI(QWidget* parent, Qt::WFlags f)
{
#ifdef MARSYAS_MATLAB

	//if a MATLAB editor already exists (i.e. is being displayed)
	//close it first in order to allow creating a new one
	//(this avoids any container window from being "empty")
	if(MATLABeditorGUI_)
		return NULL;

	//create the Dialog
	MATLABeditorGUI_ = new MATLABeditorGUI(MATLABscript_,parent, f);
	//string path = prefix_.substr(0, absPath_.length()-1);//remove trailing "/"
	MATLABeditorGUI_->setWindowTitle(QString::fromStdString(absPath_));
	MATLABeditorGUI_->setObjectName("MATLABeditorGUI");

	connect(MATLABeditorGUI_, SIGNAL(scriptChanged(std::string)),
		this, SLOT(setMATLABscript(std::string)));

	// 	Q_ASSERT(
	// 		connect(this, SIGNAL(processed()),
	// 						MATLABeditorGUI_, SLOT(updateOutputDisplay()))
	// 		);

	connect(MATLABeditorGUI_, SIGNAL(destroyed(QObject*)),
		this, SLOT(GUIdestroyed(QObject*)));

	//if no parent widget is specified, open the controls dialog
	//as an independent window and return a NULL pointer so it can
	//not be deleted by mistake. The MarSystemControlsGUI class, when
	//created without a parent deletes itself on close.
	if(!parent)
	{
		MATLABeditorGUI_->setAttribute(Qt::WA_DeleteOnClose, true);
		MATLABeditorGUI_->show();
		return NULL;
	}
	else
		return MATLABeditorGUI_;

#else //MARSYAS_MATLAB
	MRSWARN("MarSystem::getMATLABeditor(): Marsyas not built with MATLAB engine support!");
	return NULL;
#endif //MARSYAS_MATLAB
}

QMainWindow*
MarSystem::getControlsGUI(QWidget* parent, Qt::WFlags f)
{
	//create a MarControls editor GUI for this MarSystem
	MarSystemControlsGUI* controlsGUI = MarGUIManager::getControlsGUI(this, parent, f);
	controlsGUI->setWindowTitle(QString::fromStdString(absPath_));
	controlsGUI->setObjectName("controlsGUI_" + QDateTime::currentDateTime().toString(Qt::ISODate));

	//store this control in the active GUIs list
	activeControlsGUIs_[controlsGUI->objectName()] = controlsGUI;

	//connect signal sent by controlsGUI whenever a control value is changed
	//by the user
	// 	Q_ASSERT(
	// 		connect(controlsGUI_, SIGNAL(controlChanged(std::string, MarControlPtr)),
	// 		this, SLOT(updControl(std::string, MarControlPtr)))
	// 		);

	//connect a signal to the controlsGUI to update any control
	//whose value was modified elsewhere.
	//This would in theory create an infinite loop anytime a control was changed
	//using the GUI, but the MarSystemControlsGUI class provides a mechanism for
	//avoiding that (See MarSystemControlsGUI.cpp).
	connect(this, SIGNAL(controlChanged(MarControl*)),
		controlsGUI, SLOT(updControl(MarControl*)));

	//connect the controlsGUI destroyed signal to a slot so we can detect
	//when the controls editor was closed/destroyed
	connect(controlsGUI, SIGNAL(destroyed(QObject*)),
		this, SLOT(GUIdestroyed(QObject*)));

	//if no parent widget is specified, open the controls dialog
	//as an independent window and return a NULL pointer so it can
	//not be deleted by mistake. The MarSystemControlsGUI class, when
	//created without a parent deletes itself on close.
	if(!parent)
	{
		controlsGUI->setAttribute(Qt::WA_DeleteOnClose, true);
		controlsGUI->show();
		return NULL;
	}
	else
		return controlsGUI;
}

QMainWindow*
MarSystem::getDataGUI(QWidget* parent, Qt::WFlags f)
{
	//to be further defined...
	//...
	return NULL;
}

void
MarSystem::GUIdestroyed(QObject *obj)
{
	//check if the Qt object destroyed was the network viewer...
	if(obj->objectName() == "MarSystemNetworkGUI")
	{
		msysNetGUI_ = NULL;
		return;
	}
	//check if the Qt object destroyed was the MATLAB editor...
	if(obj->objectName() == "MATLABeditorGUI")
	{
		MATLABeditorGUI_ = NULL;
		return;
	}
	//remove destroyed GUI objects from the active lists
	//so they are not deleted again at the destructor 
	activeControlsGUIs_.remove(obj->objectName());
	activeDataGUIs_.remove(obj->objectName());
}

// #ifdef MARSYAS_QT
// void
// MarControls::emitControlChanged(MarControlPtr* control)
// {
// 	//only bother calling MarSystem's controlChanged signal
// 	//if there is a GUI currently active(i.e. being displayed)
// 	//=> more efficient! [!]
// 	if(msys_->controlsGUI_ || msys_->dataGUI_)//possible because this class is friend of MarSystem //[!]
// 	{
// 		Q_ASSERT(QMetaObject::invokeMethod(msys_, "controlChanged", Qt::AutoConnection,
// 			Q_ARG(MarControlPtr*, control)));
// 	}
// }
// #endif //MARSYAS_QT

#endif //MARSYAS_QT
