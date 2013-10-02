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

#include <marsyas/sched/TmControlValue.h>
#include <marsyas/system/MarControlValue.h>

using namespace std;
using namespace Marsyas;

TmControlValue::TmControlValue()
{
  type_ = tmcv_null;
}

TmControlValue::TmControlValue(const TmControlValue& val)
{
  type_=val.type_;
  r_=val.r_;
  n_=val.n_;
  b_=val.b_;
  s_=val.s_;
  ms_=val.ms_;
}

TmControlValue::TmControlValue(double re)
{
  r_ = (mrs_real)re;
  type_ = tmcv_real;
}

TmControlValue::TmControlValue(float re)
{
  r_ = (mrs_real)re;
  type_ = tmcv_real;
}

TmControlValue::TmControlValue(int ne)
{
  n_ = ne;
  type_ = tmcv_natural;
}

TmControlValue::TmControlValue(long int ne)
{
  n_ = ne;
  type_ = tmcv_natural;
}

TmControlValue::TmControlValue(std::string st)
{
  s_ = st;
  type_ = tmcv_string;
}

TmControlValue::TmControlValue(const char *c)
{
  s_ = c;
  type_ = tmcv_string;
}

TmControlValue::TmControlValue(bool be)
{
  b_ = be;
  type_ = tmcv_bool;
}

TmControlValue::TmControlValue(MarSystem* me)
{
  ms_ = me;
  type_ = tmcv_marsystem;
}

mrs_real
TmControlValue::toReal()
{
  if(type_ != tmcv_real) {
    MRSWARN("MarControlValue::toReal Incorrect type");
    return 0.0;
  }
  else
    return r_;
}

bool
TmControlValue::toBool()
{
  if(type_ != tmcv_bool) {
    MRSWARN("MarControlValue::toBool Incorrect type");
    return false;
  }
  else
    return b_;
}

mrs_natural
TmControlValue::toNatural()
{
  if(type_ != tmcv_natural) {
    MRSWARN("MarControlValue::toNatural Incorrect type");
    return 0;
  }
  else
    return n_;
}

std::string
TmControlValue::toString()
{
  if(type_ != tmcv_string) {
    MRSWARN("MarControlValue::toString Incorrect type");
    return "";
  }
  else
    return s_;
}

MarSystem*
TmControlValue::toMarSystem()
{
  if(type_ != tmcv_marsystem) {
    MRSWARN("MarControlValue::toMarSystem Incorrect type");
    return NULL;
  }
  else
    return ms_;
}

int
TmControlValue::getType()
{
  return type_;
}

mrs_string
TmControlValue::getSType()
{
  mrs_string res;

  if(getType() == tmcv_string) res = "mrs_string";
  else if(getType() == tmcv_real) res = "mrs_real";
  else if(getType() == tmcv_vec) res = "mrs_realvec";
  else if(getType() == tmcv_natural) res = "mrs_natural";
  else if(getType() == tmcv_bool) res = "mrs_bool";
  else if(getType() == tmcv_marsystem) res = "mrs_marsystem";
  return res;
}

namespace Marsyas {
ostream&
operator<<(ostream& o, const TmControlValue& m)
{
  if(m.type_ == tmcv_string) o << m.s_;
  if(m.type_ == tmcv_real) o << m.r_;
  if(m.type_ == tmcv_natural) o << m.n_;
  if(m.type_ == tmcv_bool) o << m.b_;
  if(m.type_ == tmcv_marsystem) o << "MarSystem";
  if(m.type_ == tmcv_vec) o << "realvec";
  return o;
}
}
