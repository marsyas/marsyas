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

	name_ = name;
	type_ = type;
	prefix_ = "/" + type_ + "/" + name_ + "/";
	path_ = prefix_;
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

	type_ = a.type_;
	name_ = a.name_;
	prefix_ = a.prefix_;
	path_ = a.path_;
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
			controls_[ctrlIter_->first] = ctrlIter_->second->clone();
			controls_[ctrlIter_->first]->setMarSystem(this);
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
	}

	synonyms_ = a.synonyms_;

	//recreate schedule objects
	scheduler_.removeAll();
	TmTimer* t = new TmSampleCount(NULL, this, "mrs_natural/inSamples");
	scheduler_.addTimer(t);
	delete t;
}

MarSystem::~MarSystem()
{
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

// MarSystem& 
// MarSystem::operator=(const MarSystem& a)
// {
// 	if (this != &a)
// 	{
// 		type_ = a.type_;
// 		name_ = a.name_;
// 		prefix_ = a.prefix_;
// 		path_ = a.path_;
// 		MATLABscript_ = a.MATLABscript_;
// 		
// 		//clone controls
// 		{
// 			#ifdef MARSYAS_QT
// 			QWriteLocker locker_w(&rwLock_);
// 			QReadLocker locker_r(&(a.rwLock_));
// 			#endif
// 			controls_.clear();
// 			for(ctrlIter_ = a.controls_.begin(); ctrlIter_ != a.controls_.end(); ++ctrlIter_)
// 			{
// 				controls_[ctrlIter_->first] = ctrlIter_->second->clone();
// 				ctrlIter_->second->setMarSystem(this);
// 			}
// 		}
// 	}
// 	return *this;
// }

void
MarSystem::addControls()
{
	//input pin controls (with state)
	addctrl("mrs_natural/inSamples", MRS_DEFAULT_SLICE_NSAMPLES, ctrl_inSamples_);
	ctrl_inSamples_->setState(true);
	addctrl("mrs_natural/inObservations", MRS_DEFAULT_SLICE_NOBSERVATIONS, ctrl_inObservations_);
	ctrl_inObservations_->setState(true);
	addctrl("mrs_real/israte", MRS_DEFAULT_SLICE_SRATE, ctrl_israte_);
	ctrl_israte_->setState(true);
	addctrl("mrs_string/inObsNames", ",", ctrl_inObsNames_);
	ctrl_inObsNames_->setState(true);

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

void 
MarSystem::addMarSystem(MarSystem *marsystem)
{
	MRSWARN("Trying to add MarSystem to a non-Composite - Ignoring");
}

MarSystem*
MarSystem::getMarSystem(std::string path)
{
	if(path == path_) //use path_ () instead?! [?] see Composite::getMarSystem()
		return this;
	else
	{
		MRSWARN("MarSystem::getMarsystem(): " + prefix_ + " not found!");
		return NULL;
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

	//renames all controls accordingly
	map<std::string, MarControlPtr> renamedControls;

#ifdef MARSYAS_QT
	rwLock_.lockForRead(); //reading controls_
#endif

	for(ctrlIter_ = controls_.begin(); ctrlIter_ != controls_.end(); ++ctrlIter_)
	{
		string key = ctrlIter_->first;
		key = key.substr(oldPrefix.length(),key.length()-oldPrefix.length());
		key = prefix_ + key;

		ctrlIter_->second->setName(key);
		renamedControls[key] = ctrlIter_->second;
	}
#ifdef MARSYAS_QT
	rwLock_.unlock();//unlock reading mutex
	rwLock_.lockForWrite();//lock controls_ for writing
#endif

	controls_ = renamedControls;

#ifdef MARSYAS_QT
	rwLock_.unlock();//unlock writing mutex
#endif

	//update path accordingly
	string::size_type pos = path_.find(oldPrefix, 0);
	string uppath = path_.substr(0, pos);
	string downpath = path_.substr(oldPrefix.length()+pos, path_.length()-(oldPrefix.length()+pos));
	path_ = uppath + prefix_ + downpath;
}

void
MarSystem::setType(string type)
{
	if (type == type_)
		return;

	string oldPrefix = prefix_;
	prefix_ = "/" + type + "/" + name_ + "/";
	type_ = type;

	//renames all controls accordingly
	map<std::string, MarControlPtr> renamedControls;

#ifdef MARSYAS_QT
	rwLock_.lockForRead(); //reading controls_
#endif

	for(ctrlIter_ = controls_.begin(); ctrlIter_ != controls_.end(); ++ctrlIter_)
	{
		string key = ctrlIter_->first;
		key = key.substr(oldPrefix.length(),key.length()-oldPrefix.length());
		key = prefix_ + key;

		ctrlIter_->second->setName(key);
		renamedControls[key] = ctrlIter_->second;
	}
#ifdef MARSYAS_QT
	rwLock_.unlock();//unlock reading mutex
	rwLock_.lockForWrite();//lock controls_ for writing
#endif

	controls_ = renamedControls;

#ifdef MARSYAS_QT
	rwLock_.unlock();//unlock writing mutex
#endif

	//update path accordingly
	string::size_type pos = path_.find(oldPrefix, 0);
	string uppath = path_.substr(0, pos);
	string downpath = path_.substr(oldPrefix.length()+pos, path_.length()-(oldPrefix.length()+pos));
	path_ = uppath + prefix_ + downpath;
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
MarSystem::getPath() const
{
	return path_;
}

void
MarSystem::addFatherPath(std::string fpath)
{
	//e.g. if path_ was "/Gain/g/" it will become 
	//"/Series/s" + "/Gain/g/" = "/Series/s/Gain/g/"
	path_ = fpath.substr(0, fpath.length()-1) + path_; //[!]
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

	//set input member variables
	inObservations_ = ctrl_inObservations_->to<mrs_natural>();
	inSamples_ = ctrl_inSamples_->to<mrs_natural>();
	israte_ = ctrl_israte_->to<mrs_real>();

	//call derived class specific update
	myUpdate();

	//set output member variables
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
	// override this method if something needs to be done when 
	// activating/deactivating the MarSystem (e.g. start/stop threads, etc)
	// default: do nothing
}

void
MarSystem::linkctrl(string visible, string inside)
{
	linkControl(visible, inside);   
}

void
MarSystem::linkControl(string visible, string inside) //mutexes?!? [?]
{
	map<string, vector<string> >::iterator iter; //[!]
	iter = synonyms_.find(visible); //[!]

	vector<string> synonymList; 

	// extend list of synonyms
	synonymList = synonyms_[visible];
	synonymList.push_back(inside);
	synonyms_[visible] = synonymList;
}

MarControlPtr
MarSystem::getControl(string cname)
{
	MRSDIAG("MarSystem::getControl");

	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;

	// remove prefix for synonyms
	string::size_type pos = cname.find(prefix_, 0);
	string shortcname;

	if (pos == 0) 
		shortcname = cname.substr(prefix_.length(), cname.length());

	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			return getControl(prefix_ + *si);
		}
	}

#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_); //reading controls_ [!]
#endif

	if (controls_.find(cname) == controls_.end())
	{
		MRSWARN("MarSystem::getControl Unsupported control name = " + cname);
		return MarControlPtr();
	}
	return controls_[cname];
}

MarControlPtr
MarSystem::getctrl(string cname)
{
	MRSDIAG("MarSystem::getctrl");
	return getControl(prefix_ + cname);
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

bool 
MarSystem::hasctrlState(string cname)
{
	return hasControlState(prefix_ + cname);
}

bool 
MarSystem::hasControlState(string cname)
{
	MarControlPtr control = getControl(cname);
	if(control.isInvalid())
	{
		MRSWARN("MarControls::hasState Unsupported control name = " + cname);
		return false;
	}
	else
		return control->hasState();
}

void 
MarSystem::setctrlState(string cname, bool state)
{
	setControlState(prefix_ + cname, state);
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

bool MarSystem::setControl(string cname, MarControlPtr control)
{
#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_);//reading controls_ [!]
#endif

	ctrlIter_ = controls_.find(cname);
	if (ctrlIter_ == controls_.end())
	{
		MRSWARN("MarSystem::setControl Unsupported control name = " + cname);
		return false;
	}
	ctrlIter_->second->setValue(control, NOUPDATE);//should not call MarSystem::update()

#ifdef MARSYAS_QT
	//emitControlChanged(cname, control);//[!]
#endif

	return true;
}

bool MarSystem::setctrl(string cname, MarControlPtr control)
{
	return setControl(prefix_ + cname, control);
}

mrs_natural 
MarSystem::inObservations() const
{
	return inObservations_;
}

mrs_natural 
MarSystem::inSamples() const
{
	return inSamples_;
}

bool 
MarSystem::hasControl(string cname) 
{
	//this block seems to be doing nothing here!!! [!][?]
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// remove prefix for synonyms
	string::size_type pos = cname.find(prefix_, 0);
	string shortcname;
	if (pos == 0) 
		shortcname = cname.substr(prefix_.length(), cname.length());
	// check for synonyms - call recursively to resolve them
	map<string, vector<string> >::iterator ei;
	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			hasControl(prefix_ + *si);//[?]
		}
	}
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_);//reading controls_ [!]
#endif

	ctrlIter_ = controls_.find(cname);
	return (ctrlIter_ != controls_.end());
}

void 
MarSystem::updControl(string cname, MarControlPtr newcontrol)
{


	// remove prefix for synonyms
	string::size_type pos = cname.find(prefix_, 0);
	string shortcname;
	if (pos == 0) 
		shortcname = cname.substr(prefix_.length(), cname.length());
	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;
	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			updControl(prefix_ + *si, newcontrol);
		}
	}
	//if no links found, look for controls (if any)
	else
	{
		MarControlPtr control = getControl(cname);
		// if the control exists, and if its value is in fact different, 
		// update it!
		if(!control.isInvalid() && control != newcontrol)
		{
			//calls MarSystem::update() if the control has state
			control->setValue(newcontrol);
		}
	}
}

void
MarSystem::controlUpdate(MarControlPtr ctrl)
{
	//this method is called by MarControl each time the value of
	//the control (if it has state) is modified

	//check if this object owns the control //is this really needed? [!]
	//if(ctrl.getMarSystem() != this || ctrl.isInvalid())
	//	return;

	update(ctrl);

	//it is possible to define specialized "update" functions for
	//different controls, if needed...
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
	//Only composites have children, so
	//return an empty children vector.
	//Composites should override this method.
	vector<MarSystem*> emptyChildVec;
	return emptyChildVec;
}

//  MarSystem* 
//  MarSystem::clone() const
//  {
//    MRSWARN("MARSYSTEM::CLONE CALLED");
//    MRSWARN("CLONING SHOULD BE IMPLEMENTED FOR DERIVED CLASS");
//    return 0;
//  }

void
MarSystem::addctrl(string cname, MarControlPtr control)
{
	addControl(prefix_ + cname, control); 
}

void
MarSystem::addctrl(string cname, MarControlPtr control, MarControlPtr& ptr)
{
	addControl(prefix_ + cname, control, ptr);
}

void
MarSystem::addControl(string cname, MarControlPtr control)
{
#ifdef MRSDEBUGG
	string shortcname = cname.substr(prefix_.length(), cname.length());
	string::size_type pos = shortcname.find("/", 0);
	string ctype = shortcname.substr(0,pos);
	MRSASSERT(ctype!= control->getSType());
#endif 

#ifdef MARSYAS_QT
	QWriteLocker locker(&rwLock_); //writting controls_
#endif
	controls_[cname] = control;
	controls_[cname]->setMarSystem(this);
	controls_[cname]->setName(cname);
}

void
MarSystem::addControl(string cname, MarControlPtr control, MarControlPtr& ptr)
{
	addControl(cname, control);
	ptr = controls_[cname];
}

void
MarSystem::updctrl(string cname, MarControlPtr control)
{
	MRSDIAG("MarSystem::upctrl");
	updControl(prefix_ + cname, control); 
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

mrs_real* 
const MarSystem::recvControls() 
{
	return NULL;
}

// write *this to s 
ostream&
MarSystem::put(ostream &o) 
{
	o << "# MarSystem" << endl;
	o << "# Type = " << type_ << endl;
	o << "# Name = " << name_ << endl;

	o << endl;
	o << controls_ << endl; //[!]

	map<string,vector<string> >::iterator mi;
	o << "# Number of links = " << synonyms_.size() << endl;

	for (mi = synonyms_.begin(); mi != synonyms_.end(); ++mi)
	{
		vector<string> syns = mi->second;
		vector<string>::iterator vi;
		o << "# Synonyms of " << prefix_ + mi->first << " = " << endl;
		o << "# Number of synonyms = " << syns.size() << endl;

		for (vi = syns.begin(); vi != syns.end(); ++vi) 
			o << "# " << prefix_ + (*vi) << endl;
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
Marsyas::operator>> (istream& is, MarSystem& msys) //[!] "c" object is not locked during this operator call!!!! 
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

	for (i=0; i < nControls; i++)
	{
		is >> skipstr;
		is >> cname;
		string ctype1;
		string ctype;

		// string::size_type pos = cname.rfind("/");
		ctype1 = cname.substr(0,cname.rfind("/", cname.length()));
		ctype = ctype1.substr(ctype1.rfind("/", ctype1.length())+1, ctype1.length());

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
	}
	return is;
}

ostream&
Marsyas::operator<< (ostream& o, const map<string,MarControlPtr>& c) 
{
#ifdef MARSYAS_QT
	QReadLocker locker(&(c.rwLock_));
#endif 

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
	//string path = prefix_.substr(0, path_.length()-1);//remove trailing "/"
	MATLABeditorGUI_->setWindowTitle(QString::fromStdString(path_));
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
	controlsGUI->setWindowTitle(QString::fromStdString(path_));
	controlsGUI->setObjectName("controlsGUI_" + QDateTime::currentDateTime()->toString(Qt::ISODate));

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
	connect(this, SIGNAL(controlChanged(MarControlPtr)),
		controlsGUI, SLOT(updControl(MarControlPtr)));

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
