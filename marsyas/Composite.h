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
    \class Composite
    \brief Composite of MarSystem objects
    
    Combines a set MarSystem objects into a single MarSystem 
Different composites such as Fanout, Fanin and Series have 
different semantics of operations. One of the main advantages 
of composites is that control update propagate through 
all objects in the composite. 

*/

#ifndef MARSYAS_COMPOSITE_H
#define MARSYAS_COMPOSITE_H

#include "MarSystem.h"

namespace Marsyas
{

class Composite: public MarSystem
{
private:
	//if in the future all Composites should have
	//some default controls (added to the default ones
	//already created by MarSystem::addControls()), then
	//implement this local addControls() method and 
	//remember to call it at the Composite constructor
	//
	//void addControls();

	void localActivate(bool state);

protected:
  std::vector<realvec*> slices_;
  mrs_natural marsystemsSize_;
  std::vector<MarSystem*> marsystems_;

	void controlUpdate(MarControlPtr cvalue);

	virtual void myProcess(realvec& in, realvec& out) = 0;

public:
  //Composite();
  Composite(std::string type, std::string name);
  Composite(const Composite& a);
  virtual ~Composite();

	void addFatherPath(std::string fpath);

  //void updctrl(std::string cname, MarControlPtr value); //clash with inherited method from MarSystem[!][?]
  virtual void updControl(std::string cname, MarControlPtr value);
  
	bool setControl(std::string cname, MarControlPtr value);
  bool setControl(std::string cname, mrs_real value);
  bool setControl(std::string cname, mrs_natural value);
  
  bool hasControlState(std::string cname);
  bool hasControl(std::string cname);
  bool hasControlLocal(std::string cname);
   
  MarControlPtr getctrl(std::string cname);
  MarControlPtr getControl(std::string cname);
  
  void addMarSystem(MarSystem *marsystem);
  MarSystem* getMarSystem(std::string path);
  
  virtual std::vector<MarSystem*> getChildren();
  
  std::ostream& put(std::ostream& o);	  

};

}//namespace Marsyas

#endif

	

	
