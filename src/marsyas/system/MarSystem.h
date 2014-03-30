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

#include <marsyas/common_header.h>
#include <marsyas/system/MarControl.h>
#include <marsyas/system/MarControlAccessor.h>
#include <marsyas/realvec.h>
#include <marsyas/marostring.h>
#include <marsyas/Conversions.h>
#include <marsyas/sched/EvEvent.h>
#include <marsyas/sched/TmTime.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/sched/TmControlValue.h>

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

class marsyas_EXPORT MarSystemObserver
{
public:
  virtual ~MarSystemObserver() {}
  virtual void preProcess( const realvec & input ) = 0;
  virtual void postProcess( const realvec & output ) = 0;
};

class marsyas_EXPORT MarSystem
{

//friend classes
  friend class MarSystemManager;
  friend class MarControl;

public:
  typedef std::vector<MarSystem*>::size_type child_count_t;
  typedef std::vector<MarSystemObserver*>::size_type observer_count_t;

private:
  void addControls();//add MarSystem default controls
  virtual void activate(bool state);

  std::vector<MarSystemObserver*> observers_;

  MarSystem * parent_scope_;
  std::map<std::string, MarSystem*> scope_;

protected:

  // Parent MarSystem (if in a composite, otherwise it's NULL)
  MarSystem* parent_;

  // Type of MarSystem
  std::string type_;

  // Name of instance
  std::string name_;

  // /type_/name_/
  std::string prefix_;

  // /parent0Type/parent0Name/.../parentNType/parentNName/type_/name_/
  // in case this MarSystem is part of a composite
  // this is the absolute path to it
  std::string absPath_;

  bool isComposite_;

  // children
  std::vector<MarSystem*> marsystems_;
  std::vector<MarSystem*> attached_marsystems_;

  // Controls
  mutable std::map<std::string, MarControlPtr> controls_;
  typedef std::map<std::string, MarControlPtr>::iterator ControlItr;

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

  // scheduling purposes
  mrs_natural count_;

  bool active_;

  mrs_natural addToStabilizingDelay_; // value which is added to inStabilizingDelay to produce onStabilizingDelay.  Defaults to 0.

  // is true while inside update() -> used for children to check if an update came from their parent
  bool isUpdating_;

  std::string MATLABscript_;

  // control paths
  std::string getControlRelativePath(std::string cname) const;
  std::string getControlLocalPath(std::string cname) const;

  /**
   * @brief Updates internal state due to a control change.
   * @param sender The control that triggered the update; more precisely,
   * any control passed to the update() method - could be an invalid one.
   * @protected
   *
   * Implement this method in subclass to define specific response
   * to control changes.
   */
  virtual void myUpdate(MarControlPtr sender);

  virtual void localActivate(bool state);

  /**
   * @brief Processes data.
   * @param in Input data to read.
   * @param out Output data to write.
   * @protected
   *
   * Implement this method in subclass to define specific data processing.
   */
  virtual void myProcess(realvec& in, realvec& out) = 0;

  // Assignment operator (should never be called!) [!]
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

  // Naming methods
  virtual void setName(std::string name);
  virtual void setType(std::string type);
  const std::string & getType() const;
  const std::string & getName() const;
  const std::string & getPrefix() const;
  const std::string & getAbsPath() const;
  void updatePath();

  const std::map<std::string, MarControlPtr> & controls() { return controls_; }

  // Link controls
  bool linkControl(std::string cname1, std::string cname2, bool update = true);
  DEPRECATED(bool linkctrl(std::string cname1, std::string cname2, bool update = true))
  {
    return linkControl(cname1, cname2, update);
  }

  // Update controls
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
    return updControl(cname, newcontrol, upd);
  }

  DEPRECATED(bool updctrl(std::string cname, MarControlPtr newcontrol, bool upd = true))
  {
    return updControl(cname, newcontrol, upd);
  }

  // set controls (does not call update())
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

  /**
   * @brief Get control of ...[grand-][grand-]child by path.
   * @param path Path composed of types and names of children
   * and a type and name of a control,
   * e.g. "child-type/child-name/child-type/child-name/.../control-type/control-name"
   * @param searchParent Whether to search for matching control among parents (recursively).
   * @param searchChildren Whether to search for mathing control among children (recursively).
   */
  MarControlPtr getControl(std::string path, bool searchParent = false, bool searchChildren = true);
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

  /**
   * @brief Get ...[grand-][grand-]child by path.
   * @param path Path composed of child types and names,
   * e.g. "child-type/child-name/child-type/child-name/..."
   */
  virtual MarSystem* getChildMarSystem(std::string path);
  virtual void setParent(const MarSystem* parent);
  MarSystem* getParent() const
  {
    return parent_;
  }
  virtual std::vector<MarSystem*> getChildren();
  bool isDescendentOf(MarSystem *parent);

  // Scope interface

  MarSystem * scope();
  MarSystem * parentScope() { return parent_scope_; }
  void addToScope( MarSystem * marsystem );
  void removeFromScope();

  // New composite interface //

  static std::string splitPathEnd( const std::string & path, std::string & remaining );

  std::string path() const;

  const std::vector<MarSystem*> & children() const { return marsystems_; }

  /**
   * @brief Get immediate child by name.
   * @param name Child's name (without type).
   */
  MarSystem *child( const std::string & name );

  /**
   * @brief Get immediate control by name.
   * @param name Control name (without type).
   */
  MarControlPtr control( const std::string & name );

  /**
   * @brief Get system in immediate scope.
   * @param name System's name (without type).
   */
  MarSystem *subSystem( const std::string & name );

  /**
   * @brief Get system in remote scope.
   * @param path Path composed of system names (without types).
   * For example: "name/name/..." or "/name/name/...".
   *
   * Each following name in the path is looked up in previous name's scope.
   * A "/" at beginning starts lookup at root system,
   * else lookup starts at this system.
   *
   */
  MarSystem *remoteSystem( const std::string & path );

  /**
   * @brief Get control of system in remote scope.
   * @param path Path composed of system names (without types) and a control name.
   * For example: "name/.../name/control-name" or "/name/.../name/control-name".
   *
   * Each following name in the path is looked up in previous name's scope.
   * A "/" at beginning starts lookup at root system,
   * else lookup starts at this system.
   */
  MarControlPtr remoteControl( const std::string & path );

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

  // Derived class such as Composite can override put,
  // essentially overriding operator<<
  virtual std::ostream& put(std::ostream& o, bool verbose);

  // The opposite of toString() and put() above, read in the parameters for a system.
  virtual std::istream& put(std::istream& is);

  // Output the MarSystem as an HTML document with nested lists
  virtual std::ostream& put_html(std::ostream& o);

  // the usual stream IO
  marsyas_EXPORT friend std::ostream& operator<<(std::ostream&, MarSystem&);

  // controls serialization methods
  marsyas_EXPORT friend std::istream& operator>>(std::istream&, MarSystem&); //[!]
  marsyas_EXPORT friend std::ostream& operator<<(std::ostream&, const std::map<std::string,MarControlPtr>&);

  // MATLAB scripting
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
  // Observing
  //////////////////////////////////////////////////////////////////////////

  void addObserver( MarSystemObserver * observer );
  void removeObserver( MarSystemObserver * observer );
  bool hasObserver( MarSystemObserver * observer ) const;

  //////////////////////////////////////////////////////////////////////////
  // Attaching
  //////////////////////////////////////////////////////////////////////////

  void attachMarSystem( MarSystem * system );
  void detachMarSystem( MarSystem * system );

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

// Helper function for adding a prefix to each of the observation names.
mrs_string obsNamesAddPrefix(mrs_string observationNames, mrs_string prefix);

// Helper function for splitting a string.
std::vector<mrs_string> stringSplit(mrs_string input, mrs_string delimiter);

// Helper function for splitting an observation name string.
std::vector<mrs_string> obsNamesSplit(mrs_string observationNames);

}//namespace Marsyas

#endif
