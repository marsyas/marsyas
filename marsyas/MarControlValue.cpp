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


#include "MarControlValue.h"
using namespace std;


MarControlValue::MarControlValue()
{
  type_ = mar_null;
}

MarControlValue::MarControlValue(const MarControlValue& val)
{
  if (val.type_ == mar_vec)
    {
      v.create(val.v.getSize());
      v = val.v;
      desc_ = val.desc_;
      type_ = val.type_;
    }
  else
    {
      r = val.r;
      n = val.n;
      
      b = val.b;
      if (val.type_ == mar_string)
	s = val.s;
      desc_ = val.desc_;
      type_ = val.type_;
    }
  
}


MarControlValue& 
MarControlValue::operator=(const MarControlValue& a)
{
  if (this != &a)
    {
      if (a.type_ == mar_vec)
	{
	  v.create(a.v.getSize());
	  v = a.v;
	}
      r = a.r;
      n = a.n;

      
      b = a.b;
      if (a.type_ == mar_string)
	s = a.s;
      desc_ = a.desc_;
      type_ = a.type_;
    }


  return *this;
}

  

MarControlValue::MarControlValue(double re)
{
  r = (real)re;
  type_ = mar_real;
}
 

MarControlValue::MarControlValue(float re)
{
  r = (real)re;
  type_ = mar_real;
}





MarControlValue::MarControlValue(int ne)
{
  n = ne;
  type_ = mar_natural;
}

MarControlValue::MarControlValue(long int ne)
{
  n = ne;
  type_ = mar_natural;
}

MarControlValue::MarControlValue(realvec& ve)
{
  v.create(ve.getSize());
  v = ve;
  type_ = mar_vec;
}
  
MarControlValue::MarControlValue(string st)
{
  s = st;
  type_ = mar_string;
}


MarControlValue::MarControlValue(const char *c)
{
  s = c;
  type_ = mar_string;
}


MarControlValue::MarControlValue(bool be)
{
  b = be;
  type_ = mar_bool;
}


  

 
bool
MarControlValue::update(MarControlValue &val)
{
  if(val.getType() == mar_real) 
    return update(val.toReal());
  else if(val.getType() == mar_string) 
    return update(val.toString());
  else if(val.getType() == mar_bool) 
    return update(val.toBool());
  else if(val.getType() == mar_natural) 
    return update(val.toNatural());
  else if(val.getType() == mar_vec) 
    return update(val.toVec());
  else
  {
    MRSWARN("MarControlValue::update MarControlValue with invalid type was specified");
    return false;
  }
}
   

bool
MarControlValue::update(natural ne)
{
  if(type_ != mar_natural)
  {
    MRSWARN("MarControlValue::update(natural) Incorrect type");
    return false;
  }
  n = ne;
  return true;
}

bool
MarControlValue::update(real re)
{
  if(type_ != mar_real)
  {
    MRSWARN("MarControlValue::update(natural) Incorrect type");
    return false;
  }
  r = re;
  return true;
}

  
bool
MarControlValue::update(string st)
{
  if(type_ != mar_string)
  {
    MRSWARN("MarControlValue::update(string) Incorrect type");
    return false;
  }
  s = st;
  return true;
}
  
bool
MarControlValue::update(bool be)
{
  if(type_ != mar_bool)
  {
    MRSWARN("MarControlValue::update(bool) Incorrect type");
    return false;
  }
  b = be;
  return true;
}
  
bool
MarControlValue::update(realvec ve) 
{
  if(type_ != mar_vec)
  {
    MRSWARN("MarControlValue::update vec Incorrect type");
    return false;
  }
  if(v.getSize() != ve.getSize()) 
    {
      v.create(ve.getSize());
    }
  v = ve;
  return true;
}
  
/* 
bool
MarControlValue::update(natural n) 
{
  if(type_ != mar_natural)
  {
    MRSWARN("MarControlValue::update(real) Incorrect type");
    return false;
  }
  i = n;
  return true;
}
*/ 
  

  
real
MarControlValue::toReal() 
{

  
  if(type_ != mar_real) 
    {
      MRSWARN("MarControlValue::toReal Incorrect type");
    }
  
  return r;
} 


bool
MarControlValue::toBool() 
{
  if(type_ != mar_bool) 
    MRSWARN("MarControlValue::toBool Incorrect type");
  return b;
}


natural
MarControlValue::toNatural() 
{
  if(type_ != mar_natural) 
    MRSWARN("MarControlValue::toNatural Incorrect type");
  return n;
}
  
string
MarControlValue::toString() 
{
  if(type_ != mar_string) MRSWARN("MarControlValue::toString Incorrect type");
  return s;
}
  
realvec
MarControlValue::toVec() 
{
  if(type_ != mar_vec) MRSWARN("MarControlValue::toVec Incorrect type");
  return v;
}



int
MarControlValue::getType() 
{
  return type_;
}

string 
MarControlValue::getSType()
{
  string res;
  
  if(getType() == mar_string) res = "string";
  if(getType() == mar_real) res = "real";
  if(getType() == mar_vec) res = "realvec";
  if(getType() == mar_natural) res = "natural";
  if(getType() == mar_bool) res = "bool";
  return res;
  
}

bool 
operator!=(MarControlValue& v1, MarControlValue& v2)
{
  if (v1.getType() != v2.getType())
    MRSWARN("Types of MarControlValue are different");
  
  if (v1.getType() == mar_string) return (v1.toString() != v2.toString());
  if (v1.getType() == mar_real) return (v1.toReal() != v2.toReal());
  if (v1.getType() == mar_natural) return (v1.toNatural() != v2.toNatural());
  if (v1.getType() == mar_bool) return (v1.toBool() != v2.toBool());
  
  
  return true;
}


ostream&
operator<<(ostream& o, const MarControlValue& m)
{
  if(m.type_ == mar_string) o << m.s;
  if(m.type_ == mar_real) o << m.r;
  if(m.type_ == mar_natural) o << m.n;
  if(m.type_ == mar_vec) o << m.v;
  if(m.type_ == mar_bool) o << m.b;
  return o;
}
 
 
