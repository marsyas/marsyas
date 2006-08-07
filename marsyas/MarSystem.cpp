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

using namespace std;
using namespace Marsyas;

// MarSystem::MarSystem()//lmartins: this should be deprecated... [!]
// {
//   name_ = "MarSystemPrototype";
//   type_ = "MarSystem";
// 
// 	scheduler_.removeAll();
// 	TmTimer* t = new TmSampleCount(NULL, this, "mrs_natural/inSamples");
// 	scheduler_.addTimer(t);
// 	delete t;
// 
// 	//add default controls that 
// 	//all MarSystems should have
// 	addControls();
// }

MarSystem::MarSystem(string type, string name)
{
  name_ = name;
  type_ = type;

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
  type_ = a.type_;
  name_ = a.name_;
  ncontrols_ = a.ncontrols_;
  synonyms_ = a.synonyms_;
  
  inSamples_ = a.inSamples_;
  inObservations_ = a.inObservations_;
  onSamples_ = a.onSamples_;
  onObservations_ = a.onObservations_;
  //dbg_ = a.dbg_;
  //mute_ = a.mute_;
  active_ = a.active_;
  
  //[!]
  scheduler_.removeAll();
  TmTimer* t = new TmSampleCount(NULL, this, "mrs_natural/inSamples");
  scheduler_.addTimer(t);
  delete t;
}

MarSystem::~MarSystem()
{
  
}

void
MarSystem::addControls()
{
  //input pin controls (with state)
	addctrl("mrs_natural/inSamples", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES);
  setctrlState("mrs_natural/inSamples", true);
  addctrl("mrs_natural/inObservations", (mrs_natural)MRS_DEFAULT_SLICE_NOBSERVATIONS);
  setctrlState("mrs_natural/inObservations", true);
  addctrl("mrs_real/israte", MRS_DEFAULT_SLICE_SRATE);
  setctrlState("mrs_real/israte", true);
	addctrl("mrs_string/inObsNames", ",");
	setctrlState("mrs_string/inObsNames", true);

  //output pin controls (stateless)
	addctrl("mrs_natural/onSamples", (mrs_natural)MRS_DEFAULT_SLICE_NSAMPLES);
  addctrl("mrs_natural/onObservations", (mrs_natural)MRS_DEFAULT_SLICE_NOBSERVATIONS);
  addctrl("mrs_real/osrate", MRS_DEFAULT_SLICE_SRATE);
	addctrl("mrs_string/onObsNames", ",");

	inObservations_ = getctrl("mrs_natural/inObservations").toNatural();
	inSamples_ = getctrl("mrs_natural/inSamples").toNatural();
	onObservations_ = getctrl("mrs_natural/onObservations").toNatural();
	onSamples_ = getctrl("mrs_natural/onSamples").toNatural();

	//other controls:
  addctrl("mrs_bool/debug", false);			//no debug by default
  //setctrlState("mrs_bool/debug", true);  
  addctrl("mrs_bool/mute", false);			//unmuted by default
  //setctrlState("mrs_bool/mute", true);  
  addctrl("mrs_bool/active",true);			//active by default
  setctrlState("mrs_bool/active", true);
 
  //dbg_ = getctrl("mrs_bool/debug").toBool();//false;
  //mute_ = getctrl("mrs_bool/mute").toBool();//false;
  active_ = getctrl("mrs_bool/active").toBool();//true;
}

// assignment operator [?]
MarSystem& 
MarSystem::operator=(const MarSystem& a)
{
  if (this != &a)
  {
    type_ = a.type_;
    name_ = a.name_;
    ncontrols_ = a.ncontrols_;
   // lmartins: shouldn't these also be copied?! [!][?]
    /*
      synonyms_ = a.synonyms_;
      
      inSamples_ = a.inSamples_;
      inObservations_ = a.inObservations_;
      onSamples_ = a.onSamples_;
      onObservations_ = a.onObservations_;
      dbg_ = a.dbg_;
      mute_ = a.mute_;
      active_ = a.active_;
    */
  }
  return *this;
}

void 
MarSystem::addMarSystem(MarSystem *marsystem)
{
  MRSWARN("Trying to add MarSystem to a non-Composite - Ignoring");
}

void
MarSystem::setName(string name)
{
  //name_ = name;
  //ncontrols_.clear();		
  //addControls();

	if (name == name_)
		return;

	string oldPrefix = "/" + type_ + "/" + name_;
	string newPrefix = "/" + type_ + "/" + name;

	name_ = name;

	ncontrols_.renamePrefix(oldPrefix, newPrefix);
}
string 
MarSystem::getType()
{
  return type_;
}

string 
MarSystem::getName()
{
  return name_;
}

string
MarSystem::getPrefix()
{
  string prefix = type_ + "/" + name_ + "/";
  return prefix;
}

void 
MarSystem::checkFlow(realvec& in, realvec& out)
{
	mrs_natural irows = in.getRows();
	mrs_natural icols = in.getCols();
	mrs_natural orows = out.getRows();
	mrs_natural ocols = out.getCols();

	//dbg_ = false; //lmartins: [!]

	//lmartins: if (dbg_)
	if(getctrl("mrs_bool/debug").toBool())
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
MarSystem::tick()
{
  //if MarSystem is not active, ignore ticks
	if(getctrl("mrs_bool/active").toBool())
	{
		scheduler_.tick();
		process(inTick_,outTick_);
	}
	else
		MRSDIAG("MarSystem::tick() : MarSystem is not active! Ignoring tick command.");
}

void 
MarSystem::localUpdate()
{
	MRSDIAG("MarSystem.cpp - MarSystem:localUpdate");

	//lmartins:
	//By default, a MarSystem does not modify the input data stream format.
	//Override this method on a derived MarSystem if data format changes
	//should take place...
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
	setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
}

void 
MarSystem::update()
{
	MRSDIAG("MarSystem.cpp - MarSystem:update");

	localUpdate();

	inObservations_ = getctrl("mrs_natural/inObservations").toNatural();
	inSamples_ = getctrl("mrs_natural/inSamples").toNatural();
	onObservations_ = getctrl("mrs_natural/onObservations").toNatural();
	onSamples_ = getctrl("mrs_natural/onSamples").toNatural();

	//dbg_ = getctrl("mrs_bool/debug").toBool();
	//mute_ = getctrl("mrs_bool/mute").toBool();
	
	//check active status
	bool active = getctrl("mrs_bool/active").toBool();
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
}

void
MarSystem::activate(bool state)
{
	//since this method must be public (so it can be called in Composite::activate())
	//we must guarantee that the "mrs_bool/active" control is in sync with any eventual 
	//direct calls to MarSystem::activate() from client code
	if(getctrl("mrs_bool/active").toBool() != state)
	{
		setctrl("mrs_bool/active", MarControlValue(state));
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

MarControlValue
MarSystem::getctrl(string cname)
{
  MRSDIAG("MarSystem::getctrl");
  return ncontrols_.getControl("/" + type_ + "/" + name_ + "/" + cname);
}

void
MarSystem::linkctrl(string visible, string inside)
{
  linkControl(visible, inside);   
}

void
MarSystem::linkControl(string visible, string inside)
{
	map<string, vector<string> >::iterator iter;
	iter = synonyms_.find(visible);

	vector<string> synonymList; 

	// extend list of synonyms
	synonymList = synonyms_[visible];
	synonymList.push_back(inside);
	synonyms_[visible] = synonymList;
}

MarControlValue
MarSystem::getControl(string cname)
{
	MRSDIAG("MarSystem::getControl");

	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;

	// remove prefix for synonyms
	string prefix = "/" + type_ + "/" + name_ + "/";
	string::size_type pos = cname.find(prefix, 0);
	string shortcname;

	if (pos == 0) 
		shortcname = cname.substr(prefix.length(), cname.length());


	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			getControl(prefix + *si);
		}
	}

	return ncontrols_.getControl(cname);
}

bool 
MarSystem::hasctrlState(string cname)
{
	return ncontrols_.hasState("/" + type_ + "/" + name_ + "/" + cname);
}

void 
MarSystem::setctrlState(string cname, bool val)
{
	ncontrols_.setState("/" + type_ + "/" + name_ + "/" + cname, val);
}

bool 
MarSystem::hasControlState(string cname)
{
	return ncontrols_.hasState(cname);
}

void
MarSystem::setControlState(string cname, bool val)
{
	ncontrols_.setState(cname,val);
}

void MarSystem::setControl(string cname, mrs_real value)
{
	ncontrols_.updControl(cname,value);
}

void MarSystem::setControl(string cname, mrs_natural value)
{
	ncontrols_.updControl(cname,value);
}

void MarSystem::setControl(string cname, MarControlValue value)
{
	ncontrols_.updControl(cname, value);
}

void MarSystem::setctrl(string cname, mrs_natural value)
{
	 if (ocname_ != cname) 
	 {
		 ocname_ = cname;
		 prefix_ = "/";
		 prefix_ += type_;
		 prefix_ += "/";
		 prefix_ += name_;
		 prefix_ += "/";
		 prefix_ += cname;
	 }
	 setControl(prefix_, value); 
}

void MarSystem::setctrl(string cname, mrs_real value)
{
	 if (ocname_ != cname) 
	 {
		 ocname_ = cname;
		 prefix_ = "/";
		 prefix_ += type_;
		 prefix_ += "/";
		 prefix_ += name_;
		 prefix_ += "/";
		 prefix_ += cname;
	 }
	 setControl(prefix_, value); 
}

void MarSystem::setctrl(string cname, MarControlValue value)
{
	 if (ocname_ != cname) 
	 {
		 ocname_ = cname;
		 prefix_ = "/";
		 prefix_ += type_;
		 prefix_ += "/";
		 prefix_ += name_;
		 prefix_ += "/";
		 prefix_ += cname;
	 }
	 setControl(prefix_, value); 
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
	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;

	// remove prefix for synonyms
	string prefix = "/" + type_ + "/" + name_ + "/";
	string::size_type pos = cname.find(prefix, 0);
	string shortcname;

	if (pos == 0) 
	 shortcname = cname.substr(prefix.length(), cname.length());

	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			hasControl(prefix + *si);
		}
	}

	return ncontrols_.hasControl(cname);
}


void 
MarSystem::updControl(string cname, MarControlValue value)
{
	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;

	// remove prefix for synonyms
	string prefix = "/" + type_ + "/" + name_ + "/";
	string::size_type pos = cname.find(prefix, 0);
	string shortcname;

	if (pos == 0) 
	 shortcname = cname.substr(prefix.length(), cname.length());

	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		 vector<string> synonymList = synonyms_[shortcname];
		 vector<string>::iterator si;
		 for (si = synonymList.begin(); si != synonymList.end(); ++si)
		 {
			 updControl(prefix + *si, value);
		 }
	}
	else
	{
		oldval_ = getControl(cname);
		setControl(cname, value);
		if (hasControlState(cname) && (value != oldval_)) 
		{
			update();
			/*
			dbg_ = getctrl("mrs_bool/debug").toBool();
			mute_ = getctrl("mrs_bool/mute").toBool();

			if ((inObservations_ != inTick_.getRows()) ||
			 (inSamples_ != inTick_.getCols())      ||
			 (onObservations_ != outTick_.getRows()) ||
			 (onSamples_ != outTick_.getCols()))
			{
			 inTick_.create(inObservations_, inSamples_);
			 outTick_.create(onObservations_, onSamples_);
			}
			*/
		}
	}
}

map<string, MarControlValue>
MarSystem::getControls()
{
	return ncontrols_.getControls();
}

//  MarSystem* 
//  MarSystem::clone() const
//  {
//    MRSWARN("MARSYSTEM::CLONE CALLED");
//    MRSWARN("CLONING SHOULD BE IMPLEMENTED FOR DERIVED CLASS");
//    return 0;
//  }

void
MarSystem::addctrl(string cname, MarControlValue val)
{
	 ncontrols_.addControl("/" + type_ + "/" + name_ + "/" + cname, val);
}

void
MarSystem::addControl(string cname, MarControlValue val)
{
	ncontrols_.addControl(cname, val);
}

void
MarSystem::updctrl(string cname, MarControlValue value)
{
	MRSDIAG("MarSystem::upctrl");
	updControl("/" + type_ + "/" + name_ + "/" + cname, value); 
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
  /****** NEIL ADDED START *******/
/*
void
MarSystem::updctrl(Repeat rep, MarEvent* ev)
{
  scheduler_.post("0", rep, ev);
}
*/
void
MarSystem::updctrl(string time, string cname, MarControlValue value)
{
  scheduler_.post(time, Repeat(), new EvValUpd(this,cname,value));
}
void
MarSystem::updctrl(string time, Repeat rep, string cname, MarControlValue value)
{
  scheduler_.post(time, rep, new EvValUpd(this,cname,value));
}
/*
void
MarSystem::updctrl(Repeat rep, string cname, MarControlValue value)
{
  scheduler_.post("0", rep, new EvValUpd(this,cname,value));
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
MarSystem::updctrl(TmTime t, string cname, MarControlValue value)
{
  scheduler_.post(t,Repeat(),new EvValUpd(this,cname,value));
}
void
MarSystem::updctrl(TmTime t, Repeat r, string cname, MarControlValue value)
{
  scheduler_.post(t,r,new EvValUpd(this,cname,value));
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

  /****** NEIL ADDED END *******/

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
  o << ncontrols_ << endl;
  
  map<string,vector<string> >::iterator mi;
  o << "# Number of links = " << synonyms_.size() << endl;
  
  for (mi = synonyms_.begin(); mi != synonyms_.end(); ++mi)
    {
      vector<string> syns = mi->second;
      vector<string>::iterator vi;
      o << "# Synonyms of " << mi->first << " = " << endl;
      o << "# Number of synonyms = " << syns.size() << endl;
      
      for (vi = syns.begin(); vi != syns.end(); ++vi) 
	o << "# " << (*vi) << endl;
    }
   
  return o;
}

ostream& 
Marsyas::operator<< (ostream& o, MarSystem& sys)
{
  sys.put(o);
  return o;
}
	








	

