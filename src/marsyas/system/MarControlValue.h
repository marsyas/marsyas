/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef __MARCONTROLVALUE__
#define __MARCONTROLVALUE__

#include <marsyas/common_header.h>
#include <marsyas/realvec.h>

#include <string>
#include <vector>
#include <utility>
#include <typeinfo>
#include <stdexcept>

namespace Marsyas
{
/**
	\class MarControlValue
	\brief Generic class for Marsyas control values
	Created by lfpt@inescporto.pt and lmartins@inescporto.pt
*/

class marsyas_EXPORT MarControl; //forward declaration

template<class T> class MarControlValueT;

class marsyas_EXPORT MarControlValue
{
  friend class MarControl;
  friend class MarControlAccessor;

protected:
  std::string type_;

  std::string value_debug_;

  //MarControls that use this MarControlValue
  //(i.e. linked MarControls)
  std::vector<std::pair<MarControl*, MarControl*> > links_;
  typedef std::vector<std::pair<MarControl*, MarControl*> > link_vector;

protected:
  MarControlValue() {} // can't be directly created (use MarControl or MarControlValueT)
  MarControlValue(const MarControlValue& a)
  {
    type_ = a.type_;
  };

  //for debugging purposes only
  void setDebugValue();
  static void updateMarSystemFor( MarControl * );

public:
  virtual ~MarControlValue() {}

  virtual MarControlValue* clone() = 0;
  virtual void copyValue(MarControlValue& value) = 0;
  virtual void callMarSystemsUpdate() = 0;
  virtual MarControlValue* create() = 0;

  virtual std::string getTypeID() = 0;
  // workaround method to avoid circular dependencies
  std::string getRegisteredType();

  virtual std::string	getType() const ;

  template<typename T> bool hasType()
  {
    return (typeid(*this) == typeid(MarControlValueT<T>));
  }

  // workaround - virtual member functions to overload friend operators
  virtual void createFromStream(std::istream&) = 0;
  // for:	friend std::ostream& operator<<(std::ostream&, const MarControl& ctrl);
  virtual std::ostream& serialize(std::ostream& os) = 0;
  // for:	friend bool operator!=(MarControlValue& v1, MarControlValue& v2)
  virtual bool isNotEqual(MarControlValue *v) = 0;

  virtual MarControlValue* sum(MarControlValue *v) = 0;
  virtual MarControlValue* subtract(MarControlValue *v) = 0;
  virtual MarControlValue* multiply(MarControlValue *v) = 0;
  virtual MarControlValue* divide(MarControlValue *v) = 0;

  template <typename T>
  struct IsArithmetic { static const bool value = false; };

  template <typename T, bool enabled = false>
  struct Arithmetic {};

  struct GenericArithmetic;
};

//////////////////////////////////////////////////////////////////////////

template<class T>
class MarControlValueT : public MarControlValue
{
  friend class MarControl;
  friend class MarControlAccessor;

protected:
  T value_;

public:
  MarControlValueT();
  MarControlValueT(T value);
  MarControlValueT(const MarControlValueT& a);

  virtual ~MarControlValueT() {}

  MarControlValueT& operator=(const MarControlValueT& a);

  virtual MarControlValue* clone()
  {
    return new MarControlValueT<T>(*this);
  }

  virtual MarControlValue* create()
  {
    return new MarControlValueT<T>();
  }

  virtual void copyValue(MarControlValue& value);
  virtual void callMarSystemsUpdate();

  virtual std::string getTypeID();

  //setters
  //void set(MarControlValue *val, bool update);
  void set(const T &re, bool update);

  //getters
  const T& get() const;
  T& getRef();

  virtual void createFromStream(std::istream&);
  virtual std::ostream& serialize(std::ostream& os);
  virtual bool isNotEqual(MarControlValue *v);
  virtual MarControlValue* sum(MarControlValue *v);
  virtual MarControlValue* subtract(MarControlValue *v);
  virtual MarControlValue* multiply(MarControlValue *v);
  virtual MarControlValue* divide(MarControlValue *v);
};

template<class T>
MarControlValueT<T>::MarControlValueT()
{
  value_ = T();

  // simple tests are previously done for basic types for efficiency purposes
  if (typeid(T) == typeid(mrs_real))
    type_ = "mrs_real";
  else if (typeid(T) == typeid(mrs_natural))
    type_ = "mrs_natural";
  else if (typeid(T) == typeid(std::string))
    type_ = "mrs_string";
  else if (typeid(T) == typeid(realvec))
    type_ = "mrs_realvec";
  else if (typeid(T) == typeid(bool))
    type_ = "mrs_bool";
  else
  {
    type_ = this->getRegisteredType();
  }
}

template<class T>
MarControlValueT<T>::MarControlValueT(T value)
{
  value_ = value;

  setDebugValue();

  // simple tests are used for basic types for efficiency purposes
  if (typeid(T) == typeid(mrs_real))
    type_ = "mrs_real";
  else if (typeid(T) == typeid(mrs_natural))
    type_ = "mrs_natural";
  else if (typeid(T) == typeid(std::string))
    type_ = "mrs_string";
  else if (typeid(T) == typeid(realvec))
    type_ = "mrs_realvec";
  else if (typeid(T) == typeid(bool))
    type_ = "mrs_bool";
  else
  {
    type_ = this->getRegisteredType();
  }
}

template<class T>
MarControlValueT<T>::MarControlValueT(const MarControlValueT& a):MarControlValue(a)
{
  value_ = a.value_;
  type_ = a.type_;

  setDebugValue();
}

template<class T>
MarControlValueT<T>&
MarControlValueT<T>::operator=(const MarControlValueT& a)
{
  if (this != &a)
  {
    value_ = a.value_;
    type_ = a.type_;

    setDebugValue();


    //callMarSystemsUpdate(); //[?]
  }
  return *this;
}

template<class T>
void
MarControlValueT<T>::copyValue(MarControlValue& value)
{
  MarControlValueT<T> &v = dynamic_cast<MarControlValueT<T>&>(value);
  value_ = v.value_;
}

template<class T>
void
MarControlValueT<T>::callMarSystemsUpdate()
{
  //must keep a copy of the current value in case
  //this control is "toggled" in the following update calls
  //so it can be "reinjected" into all MarSystem::update() methods
  //(otherwise, only the first MarSystem in the loop below would
  //get the current value - all the remaining ones would get the value
  //"toggled" bu the first MarSystem update() call)
  T tempValue_ = value_;

  //iterate over all the MarControls that own this MarControlValue
  //and call any necessary MarSystem updates after this value change
  for(link_vector::iterator it = links_.begin(); it != links_.end(); ++it)
  {
    value_ = tempValue_; //make sure to use the current value, not a "toggled" one
    MarControl * linked_control = it->first;
    updateMarSystemFor(linked_control);
  }
}

template<class T>
std::string
MarControlValueT<T>::getTypeID()
{
  return typeid(T).name();
}

template<class T>
void
MarControlValueT<T>::set(const T & val, bool update)
{
  value_ = val;

  setDebugValue();

  if(update)
  {
    callMarSystemsUpdate();
  }
}

template<class T>
const T&
MarControlValueT<T>::get() const
{
  return value_;
}

template<class T>
bool
MarControlValueT<T>::isNotEqual(MarControlValue *v)
{
  if(this != v)//if referring to different objects, check if their contents is different...
  {
    if (type_ != v->getType())
    {
      std::ostringstream sstr;
      sstr << "MarControlValueT::isNotEqual() - Trying to compare different types of MarControlValue. "
           << "(" << this->getType() << " with " << v->getType() << ")";
      MRSWARN(sstr.str());
      return false;
    }

    return (value_ != dynamic_cast<MarControlValueT<T>*>(v)->get());
  }
  else //if v1 and v2 refer to the same object, they must be equal (=> return false)
    return false;
}

template<class T>
void
MarControlValueT<T>::createFromStream(std::istream& in)
{
  in >> value_;


  setDebugValue();


  //callMarSystemsUpdate();?!?!?!? [?]
}


template<class T>
std::ostream&
MarControlValueT<T>::serialize(std::ostream& os)
{
  os << value_;
  return os;
}

template<class T>
MarControlValue*
MarControlValueT<T>::sum(MarControlValue *v)
{
  return Arithmetic<T, IsArithmetic<T>::value>::sum(this, v);
}

template<class T>
MarControlValue*
MarControlValueT<T>::subtract(MarControlValue *v)
{
  return Arithmetic<T, IsArithmetic<T>::value>::subtract(this, v);
}

template<class T>
MarControlValue*
MarControlValueT<T>::multiply(MarControlValue *v)
{
  return Arithmetic<T, IsArithmetic<T>::value>::multiply(this, v);
}

template<class T>
MarControlValue*
MarControlValueT<T>::divide(MarControlValue *v)
{
  return Arithmetic<T, IsArithmetic<T>::value>::divide(this, v);
}

template <typename T>
struct MarControlValue::Arithmetic<T, false>
{
  static MarControlValue *sum( void*, void*)
  { throw std::runtime_error("Can not sum this."); }
  static MarControlValue *subtract( void*, void*)
  { throw std::runtime_error("Can not subtract this."); }
  static MarControlValue *multiply( void*, void*)
  { throw std::runtime_error("Can not multiply this."); }
  static MarControlValue *divide( void*, void*)
  { throw std::runtime_error("Can not divide this."); }
};

template<typename T>
MarControlValueT<T> *controlValueFor( const T & val )
{
  return new MarControlValueT<T>(val);
}

struct MarControlValue::GenericArithmetic
{
  template<typename T>
  static MarControlValueT<T> * makeValue( const T & val )
  {
    return new MarControlValueT<T>(val);
  }

  template<typename LHS, typename RHS>
  static MarControlValue *add( MarControlValue * lhs, MarControlValue * rhs )
  {
    MarControlValueT<LHS> *lhs_typed = static_cast<MarControlValueT<LHS>*>(lhs);
    MarControlValueT<RHS> *rhs_typed = static_cast<MarControlValueT<RHS>*>(rhs);
    return makeValue(lhs_typed->get() + rhs_typed->get());
  }

  template<typename LHS, typename RHS>
  static MarControlValue *subtract( MarControlValue * lhs, MarControlValue * rhs )
  {
    MarControlValueT<LHS> *lhs_typed = static_cast<MarControlValueT<LHS>*>(lhs);
    MarControlValueT<RHS> *rhs_typed = static_cast<MarControlValueT<RHS>*>(rhs);
    return makeValue(lhs_typed->get() - rhs_typed->get());
  }

  template<typename LHS, typename RHS>
  static MarControlValue *multiply( MarControlValue * lhs, MarControlValue * rhs )
  {
    MarControlValueT<LHS> *lhs_typed = static_cast<MarControlValueT<LHS>*>(lhs);
    MarControlValueT<RHS> *rhs_typed = static_cast<MarControlValueT<RHS>*>(rhs);
    return makeValue(lhs_typed->get() * rhs_typed->get());
  }

  template<typename LHS, typename RHS>
  static MarControlValue *divide( MarControlValue * lhs, MarControlValue * rhs )
  {
    MarControlValueT<LHS> *lhs_typed = static_cast<MarControlValueT<LHS>*>(lhs);
    MarControlValueT<RHS> *rhs_typed = static_cast<MarControlValueT<RHS>*>(rhs);
    return makeValue(lhs_typed->get() / rhs_typed->get());
  }
};

template <typename T>
struct MarControlValue::Arithmetic<T, true>
{
  static MarControlValue *sum( MarControlValueT<T>*lhs, MarControlValue*rhs)
  {
    if (rhs->hasType<mrs_natural>())
    {
      return GenericArithmetic::add<T,mrs_natural>(lhs, rhs);
    }
    else if (rhs->hasType<mrs_real>())
    {
      return GenericArithmetic::add<T,mrs_real>(lhs, rhs);
    }
    else
    {
      throw std::runtime_error("Can not sum this.");
    }
  }

  static MarControlValue *subtract( MarControlValueT<T>*lhs, MarControlValue*rhs)
  {
    if (rhs->hasType<mrs_natural>())
    {
      return GenericArithmetic::subtract<T,mrs_natural>(lhs, rhs);
    }
    else if (rhs->hasType<mrs_real>())
    {
      return GenericArithmetic::subtract<T,mrs_real>(lhs, rhs);
    }
    else
    {
      throw std::runtime_error("Can not subtract this.");
    }
  }

  static MarControlValue *multiply( MarControlValueT<T>*lhs, MarControlValue*rhs)
  {
    if (rhs->hasType<mrs_natural>())
    {
      return GenericArithmetic::multiply<T,mrs_natural>(lhs, rhs);
    }
    else if (rhs->hasType<mrs_real>())
    {
      return GenericArithmetic::multiply<T,mrs_real>(lhs, rhs);
    }
    else
    {
      throw std::runtime_error("Can not multiply this.");
    }
  }

  static MarControlValue *divide( MarControlValueT<T>*lhs, MarControlValue*rhs)
  {
    if (rhs->hasType<mrs_natural>())
    {
      return GenericArithmetic::divide<T,mrs_natural>(lhs, rhs);
    }
    else if (rhs->hasType<mrs_real>())
    {
      return GenericArithmetic::divide<T,mrs_real>(lhs, rhs);
    }
    else
    {
      throw std::runtime_error("Can not divide this.");
    }
  }
};

template<> struct MarControlValue::IsArithmetic<mrs_natural> { static const bool value = true; };
template<> struct MarControlValue::IsArithmetic<mrs_real> { static const bool value = true; };

} //namespace Marsyas

#endif
