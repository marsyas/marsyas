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
	\class MarControlValue
	\brief Generic class for Marsyas control values
	Created by lfpt@inescporto.pt and lmartins@inescporto.pt
*/

#ifndef __MARCONTROLVALUE__
#define __MARCONTROLVALUE__

#include <string>
#include "common.h"
#include "realvec.h"

namespace Marsyas
{

enum types_
{
    mar_null = 0,
    mar_real,
    mar_bool,
    mar_natural,
    mar_string,
    mar_vec,
		mar_custom
};

class MarControlValue
{
protected:
	int type_;

protected:
	MarControlValue() {} // can't be directly created (use MarControl or MarControlValueT)

public:
	virtual ~MarControlValue() {}

	virtual MarControlValue* clone() = 0;
	virtual MarControlValue* create() = 0;

	virtual int	MarControlValue::getType() const;
	std::string MarControlValue::getSType() const;

	// workaround - virtual member functions to overload friend operators
	// for:	friend std::ostream& operator<<(std::ostream&, const MarControl& ctrl);
	virtual std::ostream& serialize(std::ostream& os) = 0;
	// for:	friend bool operator!=(MarControlValue& v1, MarControlValue& v2)
	virtual bool isNotEqual(MarControlValue *v) = 0;
};

//////////////////////////////////////////////////////////////////////////

template<class T>
class MarControlValueT : public MarControlValue
{
protected:
	T value_;

public:
	static T invalidValue;

public:
	MarControlValueT(T value);
  MarControlValueT(const MarControlValueT& a);

	virtual ~MarControlValueT() {}

	MarControlValueT& operator=(const MarControlValueT& a);

	virtual MarControlValue* clone();
	virtual MarControlValue* create();
	
	//setters
	void set(MarControlValue *val);
	void set(T &re);

	//getters
	const T& get() const;
  
	virtual std::ostream& serialize(std::ostream& os);
	virtual bool isNotEqual(MarControlValue *v);
	bool isInvalid() { return &value_ == &invalidValue; }
};

template<>
class MarControlValueT<realvec> : public MarControlValue
{
protected:
	realvec value_;

public:
	static realvec invalidValue;

public:
	MarControlValueT(realvec value);
	MarControlValueT(const MarControlValueT& a);

	virtual ~MarControlValueT() {}

	MarControlValueT& operator=(const MarControlValueT& a);

	virtual MarControlValue* clone();
	virtual MarControlValue* create();

	//setters
	inline void set(MarControlValue *val);
	inline void set(realvec &re);

	//getters
	const realvec& get() const;

	virtual std::ostream& serialize(std::ostream& os);
	virtual bool isNotEqual(MarControlValue *v);
	bool isInvalid() { return &value_ == &invalidValue; }
};

/************************************************************************/
/* MarControlValueT template implementation                             */
/************************************************************************/
template<class T>
T MarControlValueT<T>::invalidValue;

template<class T>
MarControlValueT<T>::MarControlValueT(T value)
{
	value_ = value;
	if (typeid(T) == typeid(mrs_real))
		type_ = mar_real;
	else if (typeid(T) == typeid(mrs_natural))
		type_ = mar_natural;
	else if (typeid(T) == typeid(std::string))
		type_ = mar_string;
	else if (typeid(T) == typeid(realvec))
		type_ = mar_vec;
	else if (typeid(T) == typeid(bool))
		type_ = mar_bool;
	else
		type_ = mar_custom;
}

template<class T>
MarControlValueT<T>::MarControlValueT(const MarControlValueT& a)
{
	value_ = a.value_;
	type_ = a.type_;
}

template<class T>
MarControlValueT<T>& 
MarControlValueT<T>::operator=(const MarControlValueT& a)
{
	if (this != &a)
	{
		value_ = a.value_;
		type_ = a.type_;
	}
	return *this;
}

template<class T>
MarControlValue*
MarControlValueT<T>::clone()
{
	return new MarControlValueT<T>(*this);
}

template<class T>
MarControlValue*
MarControlValueT<T>::create()
{
	return new MarControlValueT<T>(T());
}

template<class T>
void
MarControlValueT<T>::set(T &val)
{
	value_ = val;
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
			MRSWARN("Types of MarControlValue are different");
		}

		return value_ != dynamic_cast<MarControlValueT<T>*>(v)->get();
	}
	else //if v1 and v2 refer to the same object, they must be equal (=> return false)
		return false;
}

template<class T>
std::ostream&
MarControlValueT<T>::serialize(std::ostream& os)
{
	os << value_;
	return os;
}

}//namespace Marsyas

#endif
