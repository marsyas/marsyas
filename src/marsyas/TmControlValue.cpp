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

#include "TmControlValue.h"
#include "MarControlValue.h"

using namespace std;
using namespace Marsyas;

TmControlValue::TmControlValue()
{
  type_ = tmcv_null;
}
TmControlValue::TmControlValue(const TmControlValue& val)
{
  type_=val.type_;
  r=val.r;
  n=val.n;
  b=val.b;
  s=val.s;
  ms=val.ms;
}

TmControlValue::TmControlValue(double re)
{
  r = (mrs_real)re;
  type_ = tmcv_real;
}

TmControlValue::TmControlValue(float re)
{
  r = (mrs_real)re;
  type_ = tmcv_real;
}

TmControlValue::TmControlValue(int ne)
{
  n = ne;
  type_ = tmcv_natural;
}

TmControlValue::TmControlValue(long int ne)
{
  n = ne;
  type_ = tmcv_natural;
}

TmControlValue::TmControlValue(string st)
{
  s = st;
  type_ = tmcv_string;
}


TmControlValue::TmControlValue(const char *c)
{
  s = c;
  type_ = tmcv_string;
}


TmControlValue::TmControlValue(bool be)
{
  b = be;
  type_ = tmcv_bool;
}

TmControlValue::TmControlValue(MarSystem* me)
{
  ms = me;
  type_ = tmcv_marsystem;
}

mrs_real
TmControlValue::toReal()
{
    if(type_ != tmcv_real)
        MRSWARN("MarControlValue::toReal Incorrect type");
	else 
		return r;
}

bool
TmControlValue::toBool()
{
    if(type_ != tmcv_bool)
        MRSWARN("MarControlValue::toBool Incorrect type");
	else
		return b;
}

mrs_natural
TmControlValue::toNatural()
{
    if(type_ != tmcv_natural)
        MRSWARN("MarControlValue::toNatural Incorrect type");
	else 
       return n;
}

string
TmControlValue::toString()
{
    if(type_ != tmcv_string)
        MRSWARN("MarControlValue::toString Incorrect type");
	else 
		return s;
}

MarSystem*
TmControlValue::toMarSystem()
{
  if(type_ != tmcv_marsystem)
      MRSWARN("MarControlValue::toMarSystem Incorrect type");
  else
   return ms;
}

int
TmControlValue::getType()
{
    return type_;
}

string 
TmControlValue::getSType()
{
  string res;
  
  if(getType() == tmcv_string) res = "mrs_string";
  else if(getType() == tmcv_real) res = "mrs_real";
  else if(getType() == tmcv_vec) res = "mrs_realvec";
  else if(getType() == tmcv_natural) res = "mrs_natural";
  else if(getType() == tmcv_bool) res = "mrs_bool";
  else if(getType() == tmcv_marsystem) res = "mrs_marsystem";
  return res;
}

ostream&
Marsyas::operator<<(ostream& o, const TmControlValue& m)
{
  if(m.type_ == tmcv_string) o << m.s;
  if(m.type_ == tmcv_real) o << m.r;
  if(m.type_ == tmcv_natural) o << m.n;
  if(m.type_ == tmcv_bool) o << m.b;
  return o;
}

 
