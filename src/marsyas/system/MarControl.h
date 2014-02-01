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

#ifndef __MARCONTROL__
#define __MARCONTROL__

#include <marsyas/common_header.h>
#include <marsyas/realvec.h>
#include <marsyas/system/MarControlValue.h>

#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <typeinfo>

//#define TRACECONTROLS
#ifdef TRACECONTROLS
#include <set>
#define TRACE_ADDCONTROL MarControlPtr::controlTracer.insert(control_);;
#define TRACE_REMCONTROL if(control_->getRefCount() == 1) controlTracer.erase(control_);
#else
#define TRACE_ADDCONTROL
#define TRACE_REMCONTROL
#endif

#define TRACE_ADDCONTROL
#define TRACE_REMCONTROL

#define WAS_INLINE

namespace Marsyas
{
/**
	\class MarControl
	\ingroup Special
	\brief  MarControlPtr is a smart pointer wrapper for MarControl.
	MarControl is the control class containing a name, type and value.
	Created by lfpt@inescporto.pt and lmartins@inescporto.pt

*/

static const bool NOUPDATE = false;

class MarSystem;
class MarControl;
class MarControlManager;

//////////////////////////////////////////////////////////////////////////
//	MarControlPtr declaration
//////////////////////////////////////////////////////////////////////////
class marsyas_EXPORT MarControlPtr
{
#ifdef TRACECONTROLS
public:
  static std::set<MarControl*> controlTracer;
  static void printControlTracer();
#endif

protected:
  MarControl *control_;

public:
  // default constructor
  MarControlPtr();

  // copy constructor
  WAS_INLINE MarControlPtr(const MarControlPtr& a);

  // basic types constructors / for compatibility purposes
  WAS_INLINE MarControlPtr(MarControl control);
  WAS_INLINE MarControlPtr(MarControlValue *value);
  WAS_INLINE MarControlPtr(int ne);
  WAS_INLINE MarControlPtr(float ne);
  WAS_INLINE MarControlPtr(mrs_natural ne);
  WAS_INLINE MarControlPtr(double re);
  WAS_INLINE MarControlPtr(const char *c);
  WAS_INLINE MarControlPtr(std::string st);
  WAS_INLINE MarControlPtr(bool be);
  WAS_INLINE MarControlPtr(realvec ve);
  WAS_INLINE MarControlPtr(unsigned int ne);

  // generic type constructor
  WAS_INLINE MarControlPtr(MarControl *control);

  // assignment operator
  WAS_INLINE MarControlPtr& operator=(const MarControlPtr& a);

  ~MarControlPtr();

  MarControl* operator()() const { return control_; }
  MarControl& operator*() const  { return *control_; }
  MarControl* operator->() const { return control_; }

  WAS_INLINE bool isInvalid() const;
  WAS_INLINE bool isEqual(const MarControlPtr& v1);

  marsyas_EXPORT
  friend WAS_INLINE std::ostream& operator<<(std::ostream& os, const MarControlPtr& ctrl);
  marsyas_EXPORT
  friend WAS_INLINE bool operator==(const MarControlPtr& v1, const MarControlPtr& v2);
  marsyas_EXPORT
  friend WAS_INLINE bool operator!=(const MarControlPtr& v1, const MarControlPtr& v2);

  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator+(const MarControlPtr& v1, const mrs_real& v2);
  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator+(const mrs_real& v1, const MarControlPtr& v2);
  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator-(const MarControlPtr& v1, const mrs_real& v2);
  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator-(const mrs_real& v1, const MarControlPtr& v2);
  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator*(const MarControlPtr& v1, const mrs_real& v2);
  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator*(const mrs_real& v1, const MarControlPtr& v2);
  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator/(const MarControlPtr& v1, const mrs_real& v2);
  marsyas_EXPORT
  friend WAS_INLINE mrs_real operator/(const mrs_real& v1, const MarControlPtr& v2);

  marsyas_EXPORT
  friend WAS_INLINE MarControlPtr operator+(const MarControlPtr& v1, const MarControlPtr& v2);
  marsyas_EXPORT
  friend WAS_INLINE MarControlPtr operator-(const MarControlPtr& v1, const MarControlPtr& v2);
  marsyas_EXPORT
  friend WAS_INLINE MarControlPtr operator*(const MarControlPtr& v1, const MarControlPtr& v2);
  marsyas_EXPORT
  friend WAS_INLINE MarControlPtr operator/(const MarControlPtr& v1, const MarControlPtr& v2);

  marsyas_EXPORT friend bool operator<(const MarControlPtr& v1, const MarControlPtr& v2);
};

//////////////////////////////////////////////////////////////////////////
//	MarControl declaration
//////////////////////////////////////////////////////////////////////////
class marsyas_EXPORT MarControl
{
public:
  friend class MarControlManager;
  friend class MarControlAccessor;

private:
  int refCount_;
  MarControlValue *value_;
  MarSystem* msys_;
  std::string cname_;
  std::string id_;
  std::string desc_;
  bool state_;
  bool is_public_;

private:
  // default constructor
  explicit MarControl(); // not allowed (yes but what about the friend classes then?)

public:

  // copy constructor
  WAS_INLINE MarControl(const MarControl& a);

  // generic type constructor
  WAS_INLINE MarControl(MarControlValue *value, std::string cname = "", MarSystem* msys = 0, bool state = false);

  // basic types constructors / for compatibility purposes
  WAS_INLINE MarControl(double re, std::string cname = "", MarSystem* msys = 0, bool state = false);
  WAS_INLINE MarControl(float  re, std::string cname = "", MarSystem* msys = 0, bool state = false);
  WAS_INLINE MarControl(mrs_natural ne, std::string cname = "", MarSystem* msys = 0, bool state = false);
  WAS_INLINE MarControl(std::string st, std::string cname = "", MarSystem* msys = 0, bool state = false);
  WAS_INLINE MarControl(bool be, std::string cname = "", MarSystem* msys = 0, bool state = false);
  WAS_INLINE MarControl(realvec& ve, std::string cname = "", MarSystem* msys = 0, bool state = false);

  // destructor
  ~MarControl();

  MarControl& operator=(const MarControl& a);

  MarControl* clone();

  WAS_INLINE void ref();
  WAS_INLINE void unref();
  int getRefCount() const;

  void setMarSystem(MarSystem* msys);
  MarSystem* getMarSystem() const;
  void setName(const std::string & cname);
  const std::string & getName() const { return cname_; }
  const std::string & id() const { return id_; }
  void setState(bool state);
  bool hasState() const;
  std::string getType() const;
  std::string path() const;
  bool isPublic() const { return is_public_; }
  void setPublic(bool flag) { is_public_ = flag; }


  template<typename T>
  bool hasType()
  {
    return value_ && value_->hasType<T>();
  }

  // for link controls
  bool linkTo(MarControlPtr ctrl, bool update = true);
  void unlinkFromAll();
  void unlinkFromTarget();
  bool isLinked() const;
  std::vector<std::pair<MarControlPtr, MarControlPtr> > getLinks();

  // setters
  template<class T> WAS_INLINE bool setValue(const T& t, bool update = true);
  WAS_INLINE bool setValue(MarControlPtr mc, bool update = true);
  WAS_INLINE bool setValue(MarControlValue *mcv, bool update = true);
  WAS_INLINE bool setValue(const char *t, bool update = true);
  WAS_INLINE bool setValue(int t, bool update = true);

  // to avoid circular dependencies
  void callMarSystemUpdate();

  // getters by return (user must know the parameter's type)
  template<class T> WAS_INLINE const T& to() const;

  // type specific getters useful for SWIG bindings
  mrs_string to_string() const;
  mrs_real   to_real() const;
  mrs_natural to_natural() const;
  mrs_realvec to_realvec() const;
  mrs_bool to_bool() const;


  // bool-specific helper
  bool isTrue();

  friend WAS_INLINE std::ostream& operator<<(std::ostream& os, const MarControl& ctrl);
  friend WAS_INLINE bool operator==(const MarControl& v1, const MarControl& v2);
  friend WAS_INLINE bool operator!=(const MarControl& v1, const MarControl& v2);

  //////////////////////////////////////////////////////////////////////////
  // helper operators
  friend WAS_INLINE mrs_real operator+(const MarControl& v1, const mrs_real& v2);
  friend WAS_INLINE mrs_real operator+(const mrs_real& v1, const MarControl& v2);
  friend WAS_INLINE mrs_real operator-(const MarControl& v1, const mrs_real& v2);
  friend WAS_INLINE mrs_real operator-(const mrs_real& v1, const MarControl& v2);
  friend WAS_INLINE mrs_real operator*(const MarControl& v1, const mrs_real& v2);
  friend WAS_INLINE mrs_real operator*(const mrs_real& v1, const MarControl& v2);
  friend WAS_INLINE mrs_real operator/(const MarControl& v1, const mrs_real& v2);
  friend WAS_INLINE mrs_real operator/(const mrs_real& v1, const MarControl& v2);

  friend WAS_INLINE MarControl operator+(const MarControl& v1, const MarControl& v2);
  friend WAS_INLINE MarControl operator-(const MarControl& v1, const MarControl& v2);
  friend WAS_INLINE MarControl operator*(const MarControl& v1, const MarControl& v2);
  friend WAS_INLINE MarControl operator/(const MarControl& v1, const MarControl& v2);

  bool operator== (const MarControl& rhs)
  {
    return value_->isEqual(rhs.value_);
  }

  bool operator!= (const MarControl& rhs)
  {
    return !(value_->isEqual(rhs.value_));
  }

  bool operator< (const MarControl & rhs)
  {
    return value_->isLessThan(rhs.value_);
  }
};





/************************************************************************/
/* MarControl template implementation                                   */
/************************************************************************/
template<class T>
const T&
MarControl::to() const
{
  const MarControlValueT<T> *ptr = dynamic_cast<const MarControlValueT<T>*>(value_);
  if(ptr)
  {
    return ptr->get();
  }
  else
  {
    static T invalidValue;
    MRSERR("MarControl::to() -  Incompatible type requested - " << "expected "
           << value_->getType() << " for control  " << this->getName()) ;
    return invalidValue;
  }
}


// setters
template<class T>
WAS_INLINE
bool
MarControl::setValue(const T& t, bool update)
{
  MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(value_);
  if(ptr)
  {
    if (ptr->get() == t)
      return true;

    ptr->set(t, update);

    return true;
  }
  else
  {
    std::ostringstream sstr;
    sstr << "MarControl::setValue() - Trying to set value of incompatible type "
         << "(expected " << value_->getType() << ", given " << typeid(T).name() << ")";
    MRSWARN(sstr.str());
    return false;
  }
}





}//namespace Marsyas

//#include "MarControlAccessor.h" // ?? why after?

#endif /* __MARCONTROL__ */
