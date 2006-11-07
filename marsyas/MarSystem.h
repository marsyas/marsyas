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

//forward declarations
class MarSystemControlsGUI;
class MATLABeditorGUI;
class MarSystemNetworkGUI;

#ifdef MARSYAS_QT
	class MarSystem : public QObject
#else
class MarSystem
#endif
{

//friend classes
friend class MarSystemManager;

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
	MATLABeditorGUI* MATLABeditorGUI_;
	QHash<QString, MarSystemControlsGUI*> activeControlsGUIs_;
	QHash<QString, QWidget*> activeDataGUIs_; //[!]
#endif

protected:
	std::string type_;		// Type of MarSystem
	std::string name_;		// Name of instance
	std::string prefix_;	// /type_/name_/
	std::string path_;		// /fatherType/fatherName/type_/name_/
												// in case this MarSystem is part of a composite
												// this is the complete path to it

	mrs_natural c,o,t;    // observation and time index //[!]

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

	//Controls 
	mutable std::map<std::string, MarControlPtr> controls_;
	std::map<std::string, MarControlPtr>::iterator ctrlIter_;
	std::map<std::string,std::vector<std::string> > synonyms_;

	void addctrl(std::string cname, MarControlPtr v);
	void addctrl(std::string cname, MarControlPtr v, MarControlPtr& ptr);
	void addControl(std::string cname, MarControlPtr v);
	void addControl(std::string cname, MarControlPtr v, MarControlPtr& ptr);

	virtual void myUpdate();

	virtual void localActivate(bool state);

	virtual void myProcess(realvec& in, realvec& out) = 0;

	MarSystem& operator=(const MarSystem& a) { assert(0); } // copy assignment

public:
	virtual void controlUpdate(MarControlPtr ctrl);


	MarSystem(std::string type, std::string name);
  MarSystem(const MarSystem& a);	// copy constructor
  virtual ~MarSystem();

	virtual MarSystem* clone() const = 0;
  
  // Naming methods 
  virtual void setName(std::string name);
  std::string getType() const;
  std::string getName() const;
  std::string getPrefix() const;
	std::string getPath() const;
	virtual void addFatherPath(std::string fpath);

  void update(MarControlPtr sender = MarControlPtr());
  
  const std::map<std::string, MarControlPtr>& getControls();
	virtual std::vector<MarSystem*> getChildren();
  
#ifdef MARSYAS_QT
public slots: //[!]
#endif
  
	virtual void updControl(std::string cname, MarControlPtr control);
	void updctrl(std::string cname, MarControlPtr control);
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

	virtual bool setControl(std::string cname, MarControlPtr control);
	bool setctrl(std::string cname, MarControlPtr control);

  virtual MarControlPtr getControl(std::string cname);
	virtual MarControlPtr getctrl(std::string cname);

	void setMATLABscript(std::string script);
	std::string getMATLABscript();

public: //[!]
	virtual bool hasControl(std::string cname);

	void setControlState(std::string cname, bool val);
	void setctrlState(std::string cname, bool val);
	
	virtual bool hasControlState(std::string cname);
	bool hasctrlState(std::string cname);
  
	virtual void linkControl(std::string visible, std::string inside);
	virtual void linkctrl(std::string visible, std::string inside);

	// method to receive controls from a network connection
	virtual mrs_real* const recvControls();

  mrs_natural inObservations() const;
  mrs_natural inSamples() const;
 
	void addTimer(TmTimer* t);
	void removeTimer(std::string name);

	void checkFlow(realvec&in, realvec& out);
  
  virtual void addMarSystem(MarSystem *marsystem);
	virtual MarSystem* getMarSystem(std::string path);
  
  // methods that actually do something 
  void tick();
  void process(realvec& in, realvec& out);   
 
  // derived class such as Composite can override put 
  // essentially overriding operator<< 
  virtual std::ostream& put(std::ostream& o);

  // the usual stream IO 
  friend std::ostream& operator<<(std::ostream&, MarSystem&);

	// controls serialization methods
  friend std::istream& operator>>(std::istream&, MarSystem&); //[!]
	friend std::ostream& operator<<(std::ostream&, const std::map<std::string,MarControlPtr>&); 
	
#ifdef MARSYAS_QT

protected slots:
	void GUIdestroyed(QObject* obj);
	
public slots:
	virtual QMainWindow* getMarSystemNetworkGUI(QWidget* parent = 0, Qt::WFlags f = 0);
	virtual QMainWindow* getControlsGUI(QWidget* parent = 0, Qt::WFlags f = 0);
	virtual QMainWindow* getDataGUI(QWidget* parent = 0, Qt::WFlags f = 0);
	virtual QMainWindow* getMATLABeditorGUI(QWidget* parent = 0, Qt::WFlags f = 0);

signals:
	void controlChanged(MarControlPtr control);
	//void processed();

#endif //MARSYAS_QT

};

}//namespace Marsyas

#endif
