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

#include <string>
#include <vector>
#include <utility>
#include <typeinfo>

#include "common_header.h"
#include "core/realvec.h"


namespace Marsyas
{
/**
	\class MarControlValue
	\brief Generic class for Marsyas control values
	Created by lfpt@inescporto.pt and lmartins@inescporto.pt
*/

	class marsyas_EXPORT MarControl; //forward declaration

class marsyas_EXPORT MarControlValue
{
	friend class MarControl;
	friend class MarControlAccessor;

protected:
	std::string type_;
	MarControl* current_link_;

	std::string value_debug_;

	//MarControls that use this MarControlValue
	//(i.e. linked MarControls)
	std::vector<std::pair<MarControl*, MarControl*> > links_;
	std::vector<std::pair<MarControl*, MarControl*> >::iterator lit_;

protected:
	MarControlValue() {} // can't be directly created (use MarControl or MarControlValueT)
	MarControlValue(const MarControlValue& a)
	{
		type_ = a.type_;
	};

	//for debugging purposes only
	void setDebugValue();

	void current_link_update();


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
};

//////////////////////////////////////////////////////////////////////////

template<class T>
class MarControlValueT : public MarControlValue
{
	friend class MarControl;
	friend class MarControlAccessor;

protected:
	T value_;
	T tempValue_;

public:
	static T invalidValue;

public:
	MarControlValueT();
	MarControlValueT(T value);
  MarControlValueT(const MarControlValueT& a);

	virtual ~MarControlValueT() {}

	MarControlValueT& operator=(const MarControlValueT& a);

	virtual MarControlValue* clone();
	virtual void copyValue(MarControlValue& value);
	virtual void callMarSystemsUpdate();
	virtual MarControlValue* create();

	virtual std::string getTypeID();

	//setters
	void set(MarControlValue *val, bool update);
	void set(T &re, bool update);

	//getters
	const T& get() const;

	virtual void createFromStream(std::istream&);
	virtual std::ostream& serialize(std::ostream& os);
	virtual bool isNotEqual(MarControlValue *v);
	virtual MarControlValue* sum(MarControlValue *v);
	virtual MarControlValue* subtract(MarControlValue *v);
	virtual MarControlValue* multiply(MarControlValue *v);
	virtual MarControlValue* divide(MarControlValue *v);
	bool isInvalid() { return &value_ == &invalidValue; }
};

template<>
class marsyas_EXPORT MarControlValueT<realvec> : public MarControlValue
{
	friend class MarControl;
	friend class MarControlAccessor;

protected:
	realvec value_;
	realvec tempValue_;

public:
	static realvec invalidValue;

public:
	MarControlValueT(realvec value);
	MarControlValueT(const MarControlValueT& a);

	virtual ~MarControlValueT() {}

	MarControlValueT& operator=(const MarControlValueT& a);

	virtual MarControlValue* clone();
	virtual void copyValue(MarControlValue& value);
	virtual void callMarSystemsUpdate();
	virtual MarControlValue* create();

	virtual std::string getTypeID();

	//setters
	inline void set(MarControlValue *val, bool update);
	inline void set(realvec &re, bool update);

	//getters
	const realvec& get() const;
	realvec& getRef();

	virtual void createFromStream(std::istream&);
	virtual std::ostream& serialize(std::ostream& os);
	virtual bool isNotEqual(MarControlValue *v);
	virtual MarControlValue* sum(MarControlValue *v);
	virtual MarControlValue* subtract(MarControlValue *v);
	virtual MarControlValue* multiply(MarControlValue *v);
	virtual MarControlValue* divide(MarControlValue *v);
	bool isInvalid() { return &value_ == &invalidValue; }
};

// To avoid the compiler complaints
template<>
class marsyas_EXPORT MarControlValueT<bool> : public MarControlValue
{
	friend class MarControl;
	friend class MarControlAccessor;

protected:
	bool value_;
	bool tempValue_;

public:
	static bool invalidValue;

public:
	MarControlValueT(bool value);
	MarControlValueT(const MarControlValueT& a);

	virtual ~MarControlValueT() {}

	MarControlValueT& operator=(const MarControlValueT& a);

	virtual std::string getTypeID();

	virtual MarControlValue* clone();
	virtual void copyValue(MarControlValue& value);
	virtual void callMarSystemsUpdate();
	virtual MarControlValue* create();

	//setters
	inline void set(MarControlValue *val, bool update);
	inline void set(bool &re, bool update);

	//getters
	const bool& get() const;

	virtual void createFromStream(std::istream&);
	virtual std::ostream& serialize(std::ostream& os);
	virtual bool isNotEqual(MarControlValue *v);
	virtual MarControlValue* sum(MarControlValue *v);
	virtual MarControlValue* subtract(MarControlValue *v);
	virtual MarControlValue* multiply(MarControlValue *v);
	virtual MarControlValue* divide(MarControlValue *v);
	bool isInvalid() { return &value_ == &invalidValue; }
};

// To avoid the compiler complaints
inline std::string operator-(std::string& , std::string&) { return ""; }
inline std::string operator*(std::string& , std::string&) { return ""; }
inline std::string operator/(std::string& , std::string&) { return ""; }

/************************************************************************/
/* MarControlValueT template implementation                             */
/************************************************************************/
template<class T>
T MarControlValueT<T>::invalidValue;

template<class T>
MarControlValueT<T>::MarControlValueT()
{
	value_ = T();
	tempValue_ = T();

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
MarControlValue*
MarControlValueT<T>::clone()
{
	return new MarControlValueT<T>(*this);
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
	tempValue_ = value_;

	//iterate over all the MarControls that own this MarControlValue
	//and call any necessary MarSystem updates after this value change
	for(lit_ = links_.begin(); lit_ != links_.end(); ++lit_)
	{
		value_ = tempValue_; //make sure to use the current value, not a "toggled" one
		current_link_ = lit_->first;
		current_link_update();

	}


}

template<class T>
MarControlValue*
MarControlValueT<T>::create()
{
	return new MarControlValueT<T>(T());
}

template<class T>
std::string
MarControlValueT<T>::getTypeID()
{
	return typeid(T).name();
}

inline
std::string
MarControlValueT<realvec>::getTypeID()
{
	return typeid(realvec).name();
}

inline
std::string
MarControlValueT<bool>::getTypeID()
{
	return typeid(bool).name();
}

template<class T>
void
MarControlValueT<T>::set(T &val, bool update)
{
	value_ = val;

	setDebugValue();

	if(update)
	{
		callMarSystemsUpdate();
	}
}

inline
void
MarControlValueT<realvec>::set(realvec &val, bool update)
{
	value_ = val;

	setDebugValue();

	if(update)
	{
		callMarSystemsUpdate();
	}
}

inline
void
MarControlValueT<bool>::set(bool &val, bool update)
{
	value_ = val;

	setDebugValue();


	if(update)
		callMarSystemsUpdate();
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
MarControlValue*
MarControlValueT<T>::sum(MarControlValue *v)
{
	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(v);
	if(!ptr)
	{
		std::ostringstream sstr;
		sstr << "MarControlValueT::sum() - Trying to sum different types of MarControlValue. "
			<< "(" << this->getType() << " with " << v->getType() << ")";
		MRSWARN(sstr.str());
		return NULL;
	}
	return new MarControlValueT<T>(value_+ptr->value_);
}

template<class T>
MarControlValue*
MarControlValueT<T>::subtract(MarControlValue *v)
{
	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(v);
	if(!ptr)
	{
		std::ostringstream sstr;
		sstr << "MarControlValueT::subtract() - Trying to subtract different types of MarControlValue. "
			<< "(" << this->getType() << " with " << v->getType() << ")";
		MRSWARN(sstr.str());
		return NULL;
	}
	return new MarControlValueT<T>(value_-ptr->value_);
}

template<class T>
MarControlValue*
MarControlValueT<T>::multiply(MarControlValue *v)
{
	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(v);
	if(!ptr)
	{
		std::ostringstream sstr;
		sstr << "MarControlValueT::multiply() - Trying to multiply different types of MarControlValue. "
			<< "(" << this->getType() << " with " << v->getType() << ")";
		MRSWARN(sstr.str());
		return NULL;
	}
	return new MarControlValueT<T>(value_*ptr->value_);
}

template<class T>
MarControlValue*
MarControlValueT<T>::divide(MarControlValue *v)
{
	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(v);
	if(!ptr)
	{
		std::ostringstream sstr;
		sstr << "[MarControlValueT::divide] Trying to divide different types of MarControlValue. "
			<< "(" << this->getType() << " with " << v->getType() << ")";
		MRSWARN(sstr.str());
		return NULL;
	}
	return new MarControlValueT<T>(value_/ptr->value_);
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

}//namespace Marsyas

#endif
