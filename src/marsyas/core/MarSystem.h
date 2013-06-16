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

#ifndef MARSYAS_MARSYSTEM_H
#define MARSYAS_MARSYSTEM_H

#include "common_header.h"
#include "MarControl.h"
#include "MarControlAccessor.h"
#include "core/realvec.h"
#include "Conversions.h"

#include "EvEvent.h"
#include "TmTime.h"
#include "Scheduler.h"
#include "TmControlValue.h"
#include "marostring.h"

#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <cassert>
#include <utility>

namespace Marsyas
{
/**
\class MarSystem
\ingroup Composites
\brief MarSystem transforms a realvec

Abstract base class for any type of system. Basically a MarSystem
takes as input a vector of float numbers (realvec) and produces a new
vector (possibly with different dimensionality). Different types of
computation can be used. MarSystems are the core processing blocks of
Marsyas including transformations like FFT, Filter as well as feature
extractors like Spectral Centroid.

Controls:  (these are inherited by all MarSystems)
- \b mrs_natural/inSamples [r] : number of input samples
- \b mrs_natural/inObservations [r] : number of input observations
- \b mrs_real/israte [rw] : rate (number of samples per second) of input
- \b mrs_string/inObsNames [rw] : stores the names of observations.  Used
  for graphs, WEKA output, etc.

- \b mrs_natural/onSamples [w] : number of output samples
- \b mrs_natural/onObservations [w] : number of output observations
- \b mrs_real/osrate [rw] : rate (number of samples per second) of output
- \b mrs_string/onObsNames [rw] : stores the names of observations.  Used
  for graphs, WEKA output, etc.

- \b mrs_realvec/processedData [r] : output realvec; useful for viewing
  the output of a MarSystem directly, for a GUI plot for example.

- \b mrs_bool/debug	[rw] : for debugging purposes...
- \b mrs_bool/mute [rw] : in some MarSystems, this disables the myProcess().
  Not support on all MarSystems (yet).
- \b mrs_bool/active [rw] : used to put all internal threads to sleep.
  (experimental)

- \b mrs_natural/inStabilizingDelay [rw] : number of input ticks
  before output is stable.  Constant value for each network; any
  MarSystem wanting to track "stabilization" is responsible for
  counting its own ticks.
- \b mrs_natural/onStabilizingDelay [rw] : number of ticks before
  output is stable.
*/


class marsyas_EXPORT MarSystem
{

//friend classes
	friend class MarSystemManager;
	friend class MarControl;

private:
	void addControls();//add MarSystem default controls
	virtual void activate(bool state);

protected:

	/// Parent MarSystem (if in a composite, otherwise it's NULL)
	MarSystem* parent_;

	/// Type of MarSystem
	std::string type_;

	/// Name of instance
	std::string name_;

	/// /type_/name_/
	std::string prefix_;

	/// /parent0Type/parent0Name/.../parentNType/parentNName/type_/name_/
	/// in case this MarSystem is part of a composite
	/// this is the absolute path to it
	std::string absPath_;

	/// observation and time index //[!]
	mrs_natural c,o,t;

	bool isComposite_;
	mrs_natural marsystemsSize_;

	/// children
	std::vector<MarSystem*> marsystems_;

	std::vector<realvec*> slices_;

	std::vector<MarControlPtr> slPtrs_;

	/// Controls
	mutable std::map<std::string, MarControlPtr> controls_;
	std::map<std::string, MarControlPtr>::iterator ctrlIter_;

	//in flow member vars
	mrs_natural inObservations_;
	mrs_natural inSamples_;
	mrs_real israte_;
	mrs_string inObsNames_;
	mrs_natural inStabilizingDelay_;

	//out flow member vars
	mrs_natural onObservations_;
	mrs_natural onSamples_;
	mrs_real osrate_;
	mrs_string onObsNames_;
	mrs_natural onStabilizingDelay_;

	//temporary in flow vars
	mrs_natural tinObservations_;
	mrs_natural tinSamples_;
	mrs_real		tisrate_;
	mrs_string	tinObsNames_;
	mrs_natural tinStabilizingDelay_;

	//temporary out flow vars
	mrs_natural tonObservations_;
	mrs_natural tonSamples_;
	mrs_real		tosrate_;
	mrs_string	tonObsNames_;
	mrs_natural tonStabilizingDelay_;

	mrs_natural irows_;
	mrs_natural icols_;
	mrs_natural orows_;
	mrs_natural ocols_;

	realvec inTick_;
	realvec outTick_;

	Scheduler scheduler_;

	/// scheduling purposes
	mrs_natural count_;

	bool active_;

	mrs_natural addToStabilizingDelay_; /// value which is added to inStabilizingDelay to produce onStabilizingDelay.  Defaults to 0.

	 /// is true while inside ::update() -> used for children to check if an update came from their parent
	bool isUpdating_;

	std::string MATLABscript_;

	/// control paths
	std::string getControlRelativePath(std::string cname) const;
	std::string getControlLocalPath(std::string cname) const;

	virtual void myUpdate(MarControlPtr sender);

	virtual void localActivate(bool state);

	virtual void myProcess(realvec& in, realvec& out) = 0;

   /// Assignment operator (should never be called!) [!]
	MarSystem& operator=(const MarSystem&)
	{
		assert(0);
		return *this;
	}

	virtual std::ostream& put_html_worker(std::ostream& o);

public:
	MarSystem(std::string type, std::string name);
	MarSystem(const MarSystem& a);	// copy constructor
	virtual ~MarSystem();

	virtual MarSystem* clone() const = 0;
	void relinkControls(const MarSystem& a);

	/// Naming methods
	virtual void setName(std::string name);
	virtual void setType(std::string type);
	std::string getType() const;
	std::string getName() const;
	std::string getPrefix() const;
	std::string getAbsPath() const;
	void updatePath();

	/// Link controls
	bool linkControl(std::string cname1, std::string cname2, bool update = true);
	DEPRECATED(bool linkctrl(std::string cname1, std::string cname2, bool update = true))
	{
		return linkControl(cname1, cname2, update);
	}

	/// Update controls
	bool updControl(MarControlPtr control, MarControlPtr newcontrol, bool upd = true);
	bool updControl(const char* cname, MarControlPtr newcontrol, bool upd = true);
	bool updControl(std::string cname, MarControlPtr newcontrol, bool upd = true);
	void updControl(EvEvent* me);
	void updControl(TmTime t, EvEvent* ev);
	void updControl(TmTime t, Repeat r, EvEvent* ev);
	void updControl(TmTime t, std::string cname, MarControlPtr control);
	void updControl(TmTime t, Repeat r, std::string cname, MarControlPtr control);

	DEPRECATED(bool updctrl(MarControlPtr control, MarControlPtr newcontrol, bool upd = true))
	{
		return updControl(control, newcontrol, upd);
	}

	DEPRECATED(bool updctrl(const char *cname, MarControlPtr newcontrol, bool upd = true))
	{
		MarControlPtr control = getControl(cname);
		if (control.isInvalid())
		{
			MRSWARN("MarSystem::updctrl - " + std::string(cname) + " is an invalid control @ " + getAbsPath());
			return false;
		}
		return updControl(control, newcontrol, upd);
	}

	DEPRECATED(bool updctrl(std::string cname, MarControlPtr newcontrol, bool upd = true))
	{
		MarControlPtr control = getControl(cname);
		if (control.isInvalid())
		{
			MRSWARN("MarSystem::updctrl - " + cname + " is an invalid control @ " + getAbsPath());
			return false;
		}
		return updControl(control, newcontrol, upd);
	}

	/// set controls (does not call update())
	bool setControl(std::string cname, MarControlPtr newcontrol)
	{
		return updControl(cname, newcontrol, NOUPDATE);
	}
	bool setctrl(const char *cname, MarControlPtr newcontrol)
	{
		return updControl(std::string(cname), newcontrol, NOUPDATE);
	}
	bool setctrl(std::string cname, MarControlPtr newcontrol)
	{
		return updControl(cname, newcontrol, NOUPDATE);
	}
	bool setctrl(MarControlPtr control, MarControlPtr newcontrol)
	{
		return updControl(control, newcontrol, NOUPDATE);
	}

	// query controls
	bool hasControl(MarControlPtr control, bool searchChildren = true);
	bool hasControlLocal(MarControlPtr control)
	{
		return hasControl(control, false);
	}
	bool hasControl(std::string cname, bool searchChildren = true);
	bool hasControlLocal(std::string cname)
	{
		return hasControl(cname, false);
	}
	
	bool isComposite()
	{
		return isComposite_;
	}

	// get controls
	MarControlPtr getControl(std::string cname, bool searchParent = false, bool searchChildren = true);
	MarControlPtr getControlLocal(std::string cname)
	{
		return getControl(cname, false, false);
	}
	MarControlPtr getctrl(std::string cname)
	{
		return getControl(cname);
	}

	//add controls
	bool addControl(std::string cname, MarControlPtr v);
	bool addControl(std::string cname, MarControlPtr v, MarControlPtr& ptr);
	bool addctrl(std::string cname, MarControlPtr v)
	{
		return addControl(cname, v);
	}
	bool addctrl(std::string cname, MarControlPtr v, MarControlPtr& ptr)
	{
		return addControl(cname, v, ptr);
	}

	std::map<std::string, MarControlPtr> getControls(std::map<std::string, MarControlPtr>* cmap = NULL);
	const std::map<std::string, MarControlPtr>& getLocalControls();

	// set control state
	void setControlState(std::string cname, bool state);
	void setctrlState(std::string cname, bool state)
	{
		setControlState(cname, state);
	}
	void setctrlState(const char * cname, bool state)
	{
		setControlState(std::string(cname), state);
	}
	void setctrlState(MarControlPtr control, bool state)
	{
		control->setState(state);
	}

	// get control state
	bool hasControlState(std::string cname);
	bool hasctrlState(std::string cname)
	{
		return hasControlState(cname);
	}
	bool hasctrlState(char* cname)
	{
		return hasControlState(std::string(cname));
	}
	bool hasctrlState(MarControlPtr control)
	{
		return control->hasState();
	}

	// Composite interface
	virtual bool addMarSystem(MarSystem *marsystem);
	virtual MarSystem* getChildMarSystem(std::string childPath);
	virtual void setParent(const MarSystem* parent);
	MarSystem* getParent() const
	{
		return parent_;
	}
	virtual std::vector<MarSystem*> getChildren();

	// Processing and update methods
	bool isUpdating();
	void checkFlow(realvec&in, realvec& out);
	void update(MarControlPtr sender = MarControlPtr());
	void process(realvec& in, realvec& out);
	void tick();

	std::string toString();
	std::string toStringShort();
	std::string toStringGraphViz();
	void toStringGraphViz(std::ostringstream& os_defs, std::ostringstream& os_links);

	virtual marostring& toString(marostring& m);
	
	// derived class such as Composite can override put
	// essentially overriding operator<<
	virtual std::ostream& put(std::ostream& o, bool verbose);

	/// The opposite of toString() and put() above, read in the parameters for a system.
	virtual std::istream& put(std::istream& is);

	/// Output the MarSystem as an HTML document with nested lists
	virtual std::ostream& put_html(std::ostream& o);

	/// the usual stream IO
	marsyas_EXPORT friend std::ostream& operator<<(std::ostream&, MarSystem&);

	/// controls serialization methods
	marsyas_EXPORT friend std::istream& operator>>(std::istream&, MarSystem&); //[!]
	marsyas_EXPORT friend std::ostream& operator<<(std::ostream&, const std::map<std::string,MarControlPtr>&);

	/// MATLAB scripting
	void setMATLABscript(std::string script);
	std::string getMATLABscript();

	//////////////////////////////////////////////////////////////////////////
	// EvEvent methods
	//////////////////////////////////////////////////////////////////////////
	mrs_natural getTime(std::string timer_name);
	void updtimer(std::string tmr_ctrl_path, TmControlValue value);
	void updtimer(std::string tmr_path, TmParam param);
	void updtimer(std::string tmr_path, std::vector<TmParam> params);
	void addTimer(std::string tmr_class, std::string tmr_ident);
	void addTimer(std::string tmr_class, std::string tmr_ident, std::vector<TmParam> params);
	//void addTimer(TmTimer* t);
	void removeTimer(std::string name);

	void updctrl(EvEvent* me);
	//void updctrl(std::string time, EvEvent* ev); //clashes with void upctrl(std::string cname, 0);
	//void updctrl(std::string time, Repeat rep, EvEvent* ev);
	//void updctrl(Repeat rep, EvEvent* ev);
	//void updctrl(std::string time, std::string cname, MarControlPtr control);
	//void updctrl(std::string time, Repeat rep, std::string cname, MarControlPtr control);
	//void updctrl(Repeat rep, std::string cname, MarControlPtr control);
	void updctrl(TmTime t, EvEvent* ev);
	void updctrl(TmTime t, Repeat rep, EvEvent* ev);
	void updctrl(TmTime t, std::string cname, MarControlPtr control);
	void updctrl(TmTime t, Repeat rep, std::string cname, MarControlPtr control);
	//////////////////////////////////////////////////////////////////////////

	//control pointers [!] should these be public?
	MarControlPtr ctrl_inSamples_;
	MarControlPtr ctrl_inObservations_;
	MarControlPtr ctrl_israte_;
	MarControlPtr ctrl_inObsNames_;
	MarControlPtr ctrl_inStabilizingDelay_;
	MarControlPtr ctrl_onSamples_;
	MarControlPtr ctrl_onObservations_;
	MarControlPtr ctrl_osrate_;
	MarControlPtr ctrl_onObsNames_;
	MarControlPtr ctrl_onStabilizingDelay_;
	MarControlPtr ctrl_debug_;
	MarControlPtr ctrl_verbose_;
	MarControlPtr ctrl_mute_;
	MarControlPtr ctrl_active_;
	MarControlPtr ctrl_processedData_;
};

/// Helper function for adding a prefix to each of the observation names.
mrs_string obsNamesAddPrefix(mrs_string observationNames, mrs_string prefix);

/// Helper function for splitting a string.
std::vector<mrs_string> stringSplit(mrs_string input, mrs_string delimiter);

/// Helper function for splitting an observation name string.
std::vector<mrs_string> obsNamesSplit(mrs_string observationNames);

}//namespace Marsyas

#endif
