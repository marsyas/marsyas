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

#include "common.h"
#include "MarControl.h"
#include "realvec.h"
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
#include <cassert>

#ifdef MARSYAS_QT
#include <QtCore>
#include <QtGui>
#endif

namespace Marsyas
{

#ifdef MARSYAS_QT
//forward declarations
class MarSystemControlsGUI;
class MATLABeditorGUI;
class MarSystemNetworkGUI;

	class MarSystem : public QObject
#else
class MarSystem
#endif
{

//friend classes
friend class MarSystemManager;
friend class MarControl;

#ifdef MARSYAS_QT
	Q_OBJECT
#endif

private:
	void addControls();//add MarSystem default controls
	virtual void activate(bool state);

#ifdef MARSYAS_QT
	//Qt Mutexes
	QMutex* processMutex_;
	//Qt GUIs
	MarSystemNetworkGUI* msysNetGUI_;
	QMainWindow* MATLABeditorGUI_;

	QHash<QString, MarSystemControlsGUI*> activeControlsGUIs_;
	QHash<QString, QWidget*> activeDataGUIs_; //[!]
#endif

protected:

#ifdef MARSYAS_QT
	mutable QReadWriteLock rwLock_;
#endif

	MarSystem* parent_;		// Parent MarSystem (if in a composite, otherwise it's NULL)
	std::string type_;		// Type of MarSystem
	std::string name_;		// Name of instance
	std::string prefix_;	// /type_/name_/
	std::string absPath_;	// parent0Type/parent0Name/.../parentNType/parentNName/type_/name_/
												// in case this MarSystem is part of a composite
												// this is the absolute path to it

	mrs_natural c,o,t;    // observation and time index //[!]

	//children
	bool isComposite_;
	mrs_natural marsystemsSize_;
	std::vector<MarSystem*> marsystems_;
	std::vector<realvec*> slices_;

	//Controls 
	mutable std::map<std::string, MarControlPtr> controls_;
	std::map<std::string, MarControlPtr>::iterator ctrlIter_;

	//control pointers
	MarControlPtr ctrl_inSamples_;
	MarControlPtr ctrl_inObservations_; 
	MarControlPtr ctrl_israte_;
	MarControlPtr ctrl_inObsNames_;
	MarControlPtr ctrl_onSamples_;
	MarControlPtr ctrl_onObservations_;
	MarControlPtr ctrl_osrate_;
	MarControlPtr ctrl_onObsNames_;
	MarControlPtr ctrl_debug_; 
	MarControlPtr ctrl_mute_;
	MarControlPtr ctrl_active_;

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
  
  realvec inTick_;
  realvec outTick_;

  VScheduler scheduler_;
	mrs_natural count_; // scheduling purposes

	bool active_;

	std::string MATLABscript_;

	//add controls
	bool addControl(std::string cname, MarControlPtr v);
	bool addControl(std::string cname, MarControlPtr v, MarControlPtr& ptr);
	bool addctrl(std::string cname, MarControlPtr v) {return addControl(cname, v);}
	bool addctrl(std::string cname, MarControlPtr v, MarControlPtr& ptr) {return addControl(cname, v, ptr);}

	//control paths
	std::string getControlRelativePath(std::string cname) const;
	std::string getControlLocalPath(std::string cname) const;

	virtual void controlUpdate(MarControlPtr ctrl);

	virtual void myUpdate();
	virtual void localActivate(bool state);
	virtual void myProcess(realvec& in, realvec& out) = 0;

	MarSystem& operator=(const MarSystem&) { assert(0); } // copy assignment (should never be called!) [!]

public:
	MarSystem(std::string type, std::string name);
  MarSystem(const MarSystem& a);	// copy constructor
  virtual ~MarSystem();

	virtual MarSystem* clone() const = 0;
	
  // Naming methods 
  virtual void setName(std::string name);
	virtual void setType(std::string type);
  std::string getType() const;
  std::string getName() const;
  std::string getPrefix() const;
	std::string getAbsPath() const;
	void updatePath();

	// link controls
	bool linkControl(std::string cname1, std::string cname2);
	bool linkctrl(std::string cname1, std::string cname2) {return linkControl(cname1, cname2);}
  
	// update controls
	virtual bool updControl(std::string cname, MarControlPtr newcontrol, bool upd = true);
	bool updctrl(char *cname, MarControlPtr newcontrol, bool upd = true) {return updControl(std::string(cname), newcontrol, upd);}
	bool updctrl(std::string cname, MarControlPtr newcontrol, bool upd = true) {return updControl(cname, newcontrol, upd);}
	bool updctrl(MarControlPtr control, MarControlPtr newcontrol, bool upd = true) {return control->setValue(newcontrol, upd);}

	// set controls
	bool setControl(std::string cname, MarControlPtr newcontrol) {return updControl(cname, newcontrol, NOUPDATE);}
	bool setctrl(char *cname, MarControlPtr newcontrol) {return updControl(std::string(cname), newcontrol, NOUPDATE);}
	bool setctrl(std::string cname, MarControlPtr newcontrol) {return updControl(cname, newcontrol, NOUPDATE);}
	bool setctrl(MarControlPtr control, MarControlPtr newcontrol) {return updctrl(control, newcontrol, NOUPDATE);}
  
	// get controls
	bool hasControl(std::string cname);
	bool hasControlLocal(std::string cname);
  MarControlPtr getControl(std::string cname, bool searchParent = false, MarSystem* excluded = NULL);
	MarControlPtr getControlLocal(std::string cname);
	MarControlPtr getctrl(std::string cname) {return getControl(cname);}
	const std::map<std::string, MarControlPtr>& getControls();

	// set control state
	void setControlState(std::string cname, bool state); //should this be virtual? [?]
	void setctrlState(std::string cname, bool state) {setControlState(cname, state);}
	void setctrlState(char * cname, bool state){setControlState(std::string(cname), state);}
	void setctrlState(MarControlPtr control, bool state) {control->setState(state);}

	// get control state
	bool hasControlState(std::string cname);
	bool hasctrlState(std::string cname) {return hasControlState(cname);}
	bool hasctrlState(char* cname) {return hasControlState(std::string(cname));}
	bool hasctrlState(MarControlPtr control) {return control->hasState();}
  
	// Composite interface
  virtual bool addMarSystem(MarSystem *marsystem);
	virtual MarSystem* getMarSystem(std::string absPath);
	virtual void setParent(const MarSystem* parent);
	MarSystem* getParent() const {return parent_;}
	virtual std::vector<MarSystem*> getChildren();
  
  // Processing and update methods 
  void checkFlow(realvec&in, realvec& out);
	void update(MarControlPtr sender = MarControlPtr());
  void process(realvec& in, realvec& out);   
	void tick();

  // derived class such as Composite can override put 
  // essentially overriding operator<< 
  virtual std::ostream& put(std::ostream& o);

  // the usual stream IO 
  friend std::ostream& operator<<(std::ostream&, MarSystem&);

	// controls serialization methods
  friend std::istream& operator>>(std::istream&, MarSystem&); //[!]
	friend std::ostream& operator<<(std::ostream&, const std::map<std::string,MarControlPtr>&); 

	// method to receive controls from a network connection
	virtual mrs_real* const recvControls();

	// MATLAB scripting
	void setMATLABscript(std::string script);
	std::string getMATLABscript();

	//////////////////////////////////////////////////////////////////////////
	// MarEvent methods
	//////////////////////////////////////////////////////////////////////////
	void updctrl(MarEvent* me);
	//void updctrl(std::string time, MarEvent* ev); //clashes with void upctrl(std::string cname, 0);
	void updctrl(std::string time, Repeat rep, MarEvent* ev);
	//void updctrl(Repeat rep, MarEvent* ev);
	void updctrl(std::string time, std::string cname, MarControlPtr control);
	void updctrl(std::string time, Repeat rep, std::string cname, MarControlPtr control);
	//void updctrl(Repeat rep, std::string cname, MarControlPtr control);
	void updctrl(TmTime t, MarEvent* ev);
	void updctrl(TmTime t, Repeat rep, MarEvent* ev);
	void updctrl(TmTime t, std::string cname, MarControlPtr control);
	void updctrl(TmTime t, Repeat rep, std::string cname, MarControlPtr control);

	void addTimer(TmTimer* t);
	void removeTimer(std::string name);
	//////////////////////////////////////////////////////////////////////////
	
#ifdef MARSYAS_QT

protected slots:
	void GUIdestroyed(QObject* obj);
	
public slots:
	virtual QMainWindow* getMarSystemNetworkGUI(QWidget* parent = 0, Qt::WFlags f = 0);
	virtual QMainWindow* getControlsGUI(QWidget* parent = 0, Qt::WFlags f = 0);
	virtual QMainWindow* getDataGUI(QWidget* parent = 0, Qt::WFlags f = 0);
	virtual QMainWindow* getMATLABeditorGUI(QWidget* parent = 0, Qt::WFlags f = 0);

signals:
	void controlChanged(MarControl* control);
	//void processed();

#endif //MARSYAS_QT

};

}//namespace Marsyas

#endif
