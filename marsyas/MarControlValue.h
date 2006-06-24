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
    \class MarControlValue
    \brief MarControlValue 
    
 Created by Ari Lazier on Aug 2003.
 Modified by George Tzanetakis 

*/

#ifndef __MARCONTROLVALUE__
#define __MARCONTROLVALUE__

#include <string> 
#include <iostream>

#include "realvec.h"
#include "common.h"

namespace Marsyas
{

enum types_
{
    mar_null = 0,
    mar_real,
    mar_bool,
    mar_natural,
    mar_string,
    mar_vec
};
  

class MarControlValue
{
protected:
  int type_;
  
  mrs_real r;
  mrs_natural n;
  bool b;
  std::string s;
  realvec v;
  
  std::string desc_;
  
public:
  MarControlValue();
  MarControlValue(const MarControlValue&);
  MarControlValue& operator=(const MarControlValue& a);
  MarControlValue(float re);
  MarControlValue(double re);
  MarControlValue(int ne);
  MarControlValue(long int ne);
  MarControlValue(std::string st);
  MarControlValue(const char *);
  MarControlValue(bool be);
  
  
  // MarControlValue(mrs_natural i);
  MarControlValue(realvec& ve);

  
  bool update(mrs_real re);
  bool update(mrs_natural ne);
  bool update(bool be);
  bool update(realvec ve);
  bool update(std::string st);
  bool update(MarControlValue &val);
  
  mrs_real toReal();
  mrs_natural toNatural();
  bool toBool();
  std::string toString();
  realvec toVec(); 
  
  int getType();
  std::string getSType();
  
  friend std::ostream& operator<<(std::ostream&, const MarControlValue&);
  friend bool operator!=(MarControlValue& v1, MarControlValue& v2);
};

}//namespace Marsyas

#endif
 
 
