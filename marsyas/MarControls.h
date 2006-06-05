/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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
    \class MarControls
    \brief MarControls handle the controls for MarSystems
    
    MarControls is the class that handles the controls for 
a particular MarSystem. 

 Created by Ari Lazier on Mon Aug 11 2003.
 Modified by George Tzanetakis 

*/



#ifndef MARSYAS_CONTROLS_H
#define MARSYAS_CONTROLS_H

#include "MarControlValue.h"
#include "realvec.h"
 
#include <map>
#include <string> 



class MarControls
{
protected:
  std::map<std::string, MarControlValue> controls_;
  std::map<std::string, real> rcontrols_;
  std::map<std::string, natural> ncontrols_;
  
  std::map<std::string, bool> hasState_;
  std::map<std::string, MarControlValue>::iterator iter_;
  std::map<std::string, real>::iterator riter_;
public:
  bool hasState(std::string cname);
  void setState(std::string cname, bool st);
  void addControl(std::string cname, MarControlValue value);
  bool updControl(std::string cname, MarControlValue value);
  bool updControl(std::string cname, natural);
  bool updControl(std::string cname, real);
  bool updControl(std::string cname, bool);
  bool hasControl(std::string cname);
  

  MarControlValue getControl(std::string cname);
  std::map<std::string, MarControlValue> getControls();
  void clear();
  int size();
  
  friend std::istream& operator>>(std::istream&, MarControls& c);
  friend std::ostream& operator<<(std::ostream&, const MarControls& c);
};

#endif


