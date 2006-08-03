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

#ifndef MARSYAS_MARSYSTEM_H
#define MARSYAS_MARSYSTEM_H

#include "realvec.h"
#include "common.h"
#include "MarControls.h"
#include "MarControlValue.h"
#include "TmSampleCount.h" 
#include "EvValUpd.h" 
#include "EvExpUpd.h"
#include "MarEvent.h"
#include "TmTime.h"

#include "Conversions.h"
//#include "Scheduler.h"
#include "VScheduler.h"

#include <vector>
#include <iostream>
#include <map>
#include <string>

namespace Marsyas
{

class MarSystem
{
private:
	void addControls();//add MarSystem default controls

protected:
  mrs_natural inObservations_;
  mrs_natural inSamples_;
  mrs_real israte_;
  mrs_natural onObservations_;
  mrs_natural onSamples_;
  mrs_real osrate_;

  mrs_natural tinObservations_;
  mrs_natural tinSamples_;
  mrs_natural tonObservations_;
  mrs_natural tonSamples_;
  
  mrs_natural count_; // scheduling purposes

  realvec inTick_;
  realvec outTick_;
  std::string ocname_;
  std::string prefix_;

  MarControlValue oldval_;
  
  std::string type_;		// Type of MarSystem
  std::string name_;		// Name of instance 
  mrs_natural c,o,t;        // observation and time index

  //bool dbg_;
  //bool mute_;
  bool active_;

  VScheduler scheduler_;

	void addctrl(std::string cname, MarControlValue v);
  void addControl(std::string cname, MarControlValue v);
  
	virtual void localUpdate();
	virtual void localActivate(bool state);
  
public:
  std::map<std::string,std::vector<std::string> > synonyms_;//make it protected? [!]
  MarControls ncontrols_;	// New controls class from Ari  //make it protected? [!] 

  // Constructors 
  //MarSystem();
  MarSystem(std::string type, std::string name);
  MarSystem(const MarSystem& a);	// copy constructor
  virtual ~MarSystem();

	MarSystem& operator=(const MarSystem& a); // copy assignment

	virtual MarSystem* clone() const = 0;
  
  // Naming methods 
  virtual void setName(std::string name);
  std::string getType();
  std::string getName();
  std::string getPrefix();

  virtual void update();
  
  std::map<std::string, MarControlValue> getControls();
  
  // new control methods 
  void updctrl(std::string cname, MarControlValue value);
  void updctrl(MarEvent* me);
	//void updctrl(std::string time, MarEvent* ev); //  clashes with void upctrl(std::string cname, 0);
  void updctrl(std::string time, Repeat rep, MarEvent* ev);
	virtual void updControl(std::string cname, MarControlValue value);  
	/****** NEIL START *******/
	//void updctrl(Repeat rep, MarEvent* ev);
  void updctrl(std::string time, std::string cname, MarControlValue value);
  void updctrl(std::string time, Repeat rep, std::string cname, MarControlValue value);
	//void updctrl(Repeat rep, std::string cname, MarControlValue value);

  void addTimer(TmTimer* t);
  void removeTimer(std::string name);

  void updctrl(TmTime t, MarEvent* ev);
  void updctrl(TmTime t, Repeat rep, MarEvent* ev);
  void updctrl(TmTime t, std::string cname, MarControlValue value);
  void updctrl(TmTime t, Repeat rep, std::string cname, MarControlValue value);
	/****** NEIL END *******/

  virtual void setControl(std::string cname, MarControlValue value);
  virtual void setControl(std::string cname, mrs_real value);
  virtual void setControl(std::string cname, mrs_natural value);
  void setctrl(std::string cname, MarControlValue value);
  void setctrl(std::string cname, mrs_real value);
  void setctrl(std::string cname, mrs_natural value);
  
  virtual MarControlValue getctrl(std::string cname);
  virtual MarControlValue getControl(std::string cname);

	virtual bool hasControl(std::string cname);

  void setControlState(std::string cname, bool val);
	virtual bool hasControlState(std::string cname);
  void setctrlState(std::string cname, bool val);
  bool hasctrlState(std::string cname);

	virtual void linkctrl(std::string visible, std::string inside);
	virtual void linkControl(std::string visible, std::string inside);

  mrs_natural inObservations() const;
  mrs_natural inSamples() const;
 
  void checkFlow(realvec&in, realvec& out);
  
  virtual void addMarSystem(MarSystem *marsystem);
  
  // methods that actually do something 
  void tick();
  virtual void process(realvec& in, realvec& out)=0;   
 
  // method to receive controls from a network connection
  virtual mrs_real* const recvControls();

  // derived class such as Composite can overide put 
  // essentially overiding operator<< 
  virtual std::ostream& put(std::ostream& o);	  

  // the usual stream IO 
  friend std::ostream& operator<<(std::ostream&, MarSystem&);
  friend std::istream& operator>>(std::istream&, MarSystem&);

	virtual void activate(bool state);
};

}//namespace Marsyas

#endif
