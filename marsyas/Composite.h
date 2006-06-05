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

class Composite: public MarSystem
{
protected:

  std::vector<realvec*>   slices_;

  natural marsystemsSize_;
  std::vector<MarSystem*> marsystems_;			
  natural i;
  virtual void addControls();
  
  

public:
  Composite();
  Composite(const Composite& a);
  
  ~Composite();
  void updctrl(std::string cname, MarControlValue value);
  void updControl(std::string cname, MarControlValue value);
  void setControl(std::string cname, MarControlValue value);
  void setControl(std::string cname, real value);
  void setControl(std::string cname, natural value);
  
  virtual void update() = 0;
  
  bool hasControlState(std::string cname);
  bool hasControl(std::string cname);
  bool hasControlLocal(std::string cname);
  
  
  
  MarControlValue getctrl(std::string cname);
  MarControlValue getControl(std::string cname);
  void addMarSystem(MarSystem *marsystem);


  
  std::ostream& put(std::ostream& o);	  
  virtual void process(realvec& in, realvec& out) = 0;
};



#endif

	

	
