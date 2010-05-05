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

#include <string> 
#include <iostream>
#include <vector>
#include <utility>

#include "common_header.h"
#include "common.h"
#include "MarControlValue.h"
#include "realvec.h"

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
	inline MarControlPtr(const MarControlPtr& a);

	// basic types constructors / for compatibility purposes
	inline MarControlPtr(MarControl control);
	inline MarControlPtr(MarControlValue *value);
	inline MarControlPtr(int ne);
    inline MarControlPtr(float ne);
	inline MarControlPtr(mrs_natural ne);
	inline MarControlPtr(double re);
	inline MarControlPtr(const char *c);
	inline MarControlPtr(std::string st);
	inline MarControlPtr(bool be);
	inline MarControlPtr(realvec ve);

	// generic type constructor
	inline MarControlPtr(MarControl *control);

	// assignment operator
	inline MarControlPtr& operator=(const MarControlPtr& a);

	~MarControlPtr();

	MarControl* operator()() const { return control_; }
	MarControl& operator*() const  { return *control_; }
	MarControl* operator->() const { return control_; }

	inline bool isInvalid() const;
	inline bool isEqual(const MarControlPtr& v1);
  
	friend inline std::ostream& operator<<(std::ostream& os, const MarControlPtr& ctrl);
	friend inline bool operator==(const MarControlPtr& v1, const MarControlPtr& v2);
	friend inline bool operator!=(const MarControlPtr& v1, const MarControlPtr& v2);

	friend inline mrs_real operator+(const MarControlPtr& v1, const mrs_real& v2);
	friend inline mrs_real operator+(const mrs_real& v1, const MarControlPtr& v2);
	friend inline mrs_real operator-(const MarControlPtr& v1, const mrs_real& v2);
	friend inline mrs_real operator-(const mrs_real& v1, const MarControlPtr& v2);
	friend inline mrs_real operator*(const MarControlPtr& v1, const mrs_real& v2);
	friend inline mrs_real operator*(const mrs_real& v1, const MarControlPtr& v2);
	friend inline mrs_real operator/(const MarControlPtr& v1, const mrs_real& v2);
	friend inline mrs_real operator/(const mrs_real& v1, const MarControlPtr& v2);

	friend inline MarControlPtr operator+(const MarControlPtr& v1, const MarControlPtr& v2);
	friend inline MarControlPtr operator-(const MarControlPtr& v1, const MarControlPtr& v2);
	friend inline MarControlPtr operator*(const MarControlPtr& v1, const MarControlPtr& v2);
	friend inline MarControlPtr operator/(const MarControlPtr& v1, const MarControlPtr& v2);
};

//////////////////////////////////////////////////////////////////////////
//	MarControl declaration
//////////////////////////////////////////////////////////////////////////
class marsyas_EXPORT MarControl
{
	friend class MarControlManager;
	friend class MarControlAccessor;

protected:
	int refCount_;
	MarControlValue *value_;
	MarSystem* msys_;
	std::string cname_;
	bool state_;
	std::string desc_;

	// default constructor
	MarControl() {} // not allowed

public:
	// copy constructor
	inline MarControl(const MarControl& a);

	// generic type constructor
	inline MarControl(MarControlValue *value, std::string cname = "", MarSystem* msys = 0, bool state = false);

	// basic types constructors / for compatibility purposes
	inline MarControl(double re, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(float  re, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(mrs_natural ne, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(std::string st, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(bool be, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(realvec& ve, std::string cname = "", MarSystem* msys = 0, bool state = false);

	// destructor
	~MarControl();

	MarControl& operator=(const MarControl& a);

	MarControl* clone();

	inline void ref() 
	{ 
		refCount_++; 
	}
	inline void unref() 
	{
		if (--refCount_ <= 0) delete this; 
	}
	int getRefCount() const 
	{ 
		return refCount_; 
	}

	void setMarSystem(MarSystem* msys);
	MarSystem* getMarSystem();
	void setName(std::string cname);
	std::string getName() const;
	void setState(bool state);
	bool hasState() const;
	std::string getType() const;

	// for link controls
	bool linkTo(MarControlPtr ctrl, bool update = true);
	void unlinkFromAll();
	void unlinkFromTarget();
	bool isLinked() const;
	std::vector<std::pair<MarControlPtr, MarControlPtr> > getLinks();

	// setters
	template<class T> inline bool setValue(const T& t, bool update = true);
	template<class T> inline bool setValue(T& t, bool update = true);
	inline bool setValue(MarControlPtr mc, bool update = true);	
	inline bool setValue(MarControlValue *mcv, bool update = true);	
	inline bool setValue(const char *t, bool update = true);
	inline bool setValue(int t, bool update = true);

	// to avoid circular dependencies
	void callMarSystemUpdate();

	// getters by return (user must know the parameter's type)
	template<class T> inline const T& to() const;

	// bool-specific helper
	bool isTrue();

	friend inline std::ostream& operator<<(std::ostream& os, const MarControl& ctrl);
	friend inline bool operator==(const MarControl& v1, const MarControl& v2);
	friend inline bool operator!=(const MarControl& v1, const MarControl& v2);

	//////////////////////////////////////////////////////////////////////////
	// helper operators
	friend inline mrs_real operator+(const MarControl& v1, const mrs_real& v2);
	friend inline mrs_real operator+(const mrs_real& v1, const MarControl& v2);
	friend inline mrs_real operator-(const MarControl& v1, const mrs_real& v2);
	friend inline mrs_real operator-(const mrs_real& v1, const MarControl& v2);
	friend inline mrs_real operator*(const MarControl& v1, const mrs_real& v2);
	friend inline mrs_real operator*(const mrs_real& v1, const MarControl& v2);
	friend inline mrs_real operator/(const MarControl& v1, const mrs_real& v2);
	friend inline mrs_real operator/(const mrs_real& v1, const MarControl& v2);

	friend inline MarControl operator+(const MarControl& v1, const MarControl& v2);
	friend inline MarControl operator-(const MarControl& v1, const MarControl& v2);
	friend inline MarControl operator*(const MarControl& v1, const MarControl& v2);
	friend inline MarControl operator/(const MarControl& v1, const MarControl& v2);
};

/************************************************************************/
/* MarControlPtr inline implementation                                  */
/************************************************************************/
inline MarControlPtr::MarControlPtr(MarControl control)
{
	control_ = new MarControl(control);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(MarControlValue *value)
{
	control_ = new MarControl(value);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(int ne)
{
	control_ = new MarControl((mrs_natural)ne);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(float ne)
{
	control_ = new MarControl(ne);
	control_->ref();
	TRACE_ADDCONTROL;
}


inline MarControlPtr::MarControlPtr(mrs_natural ne)
{
	control_ = new MarControl(ne);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(double re)
{
	control_ = new MarControl(re);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(const char *c)
{
	control_ = new MarControl(std::string(c));
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(std::string st)
{
	control_ = new MarControl(st);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(mrs_bool be)
{
  control_ = new MarControl(be);
  control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(realvec ve)
{
	control_ = new MarControl(ve);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline std::ostream& operator<<(std::ostream& os, const MarControlPtr& ctrl)
{ 
	return (os << (*ctrl.control_)); 
}

inline bool operator==(const MarControlPtr& v1, const MarControlPtr& v2)
{
	return (*v1.control_) == (*v2.control_);
}

inline bool operator!=(const MarControlPtr& v1, const MarControlPtr& v2)
{
  return (*v1.control_) != (*v2.control_);
}

inline mrs_real operator+(const MarControlPtr& v1, const mrs_real& v2)
{ 
	return (*v1.control_)+v2; 
}

inline mrs_real operator+(const mrs_real& v1, const MarControlPtr& v2)
{
	return v1+(*v2.control_); 
}

inline mrs_real operator-(const MarControlPtr& v1, const mrs_real& v2) 
{
	return (*v1.control_)-v2; 
}

inline mrs_real operator-(const mrs_real& v1, const MarControlPtr& v2)
{
	return v1-(*v2.control_); 
}

inline mrs_real operator*(const MarControlPtr& v1, const mrs_real& v2)
{ 
	return (*v1.control_)*v2; 
}
inline mrs_real operator*(const mrs_real& v1, const MarControlPtr& v2)
{
	return v1*(*v2.control_);
}

inline mrs_real operator/(const MarControlPtr& v1, const mrs_real& v2)
{
	return (*v1.control_)/v2;
}

inline mrs_real operator/(const mrs_real& v1, const MarControlPtr& v2)
{
	return v1/(*v2.control_);
}

inline MarControlPtr operator+(const MarControlPtr& v1, const MarControlPtr& v2)
{ 
	return (*v1.control_)+(*v2.control_); 
}

inline MarControlPtr operator-(const MarControlPtr& v1, const MarControlPtr& v2) 
{
	return (*v1.control_)-(*v2.control_); 
}

inline MarControlPtr operator*(const MarControlPtr& v1, const MarControlPtr& v2)
{ 
	return (*v1.control_)*(*v2.control_); 
}

inline MarControlPtr operator/(const MarControlPtr& v1, const MarControlPtr& v2)
{
	return (*v1.control_)/(*v2.control_);
}

inline
MarControlPtr::MarControlPtr(const MarControlPtr& a) //mutexes? [?]
{
	control_ = a.control_;
	if (control_)
	{
		control_->ref();
		TRACE_ADDCONTROL;
	}
}

inline
MarControlPtr::MarControlPtr(MarControl *control)//mutexes? [?]
{
	control_ = control;
	if (control_)
	{
		control_->ref(); 
		TRACE_ADDCONTROL;
	}	
}

inline
MarControlPtr& 
MarControlPtr::operator=(const MarControlPtr& a)//mutexes? [?]
{
	if (control_)
	{
		TRACE_REMCONTROL;
		control_->unref();
	}
	control_ = a.control_;
	if (control_) 
	{
		control_->ref();
		TRACE_ADDCONTROL;
	}
	return *this;
}

inline
bool
MarControlPtr::isInvalid() const
{
	return (control_== NULL);
}

inline 
bool 
MarControlPtr::isEqual(const MarControlPtr& p) 
{
  return (control_ == p.control_);
}

/************************************************************************/
/* MarControl template implementation                                   */
/************************************************************************/
template<class T>
const T&
MarControl::to() const
{
	if(!this)
	{
		MRSERR("MarControl::to() - trying to get a value from a NULL MarControl! Returning invalid value...");
		return MarControlValueT<T>::invalidValue;
	}

	const MarControlValueT<T> *ptr = dynamic_cast<const MarControlValueT<T>*>(value_);
	if(ptr)
	{
		return ptr->get();
	}
	else
	{
	    std::ostringstream sstr;
	    sstr << "MarControl::to() -  Trying to set value of incompatible type "
				<< "(expected " << value_->getType() << ", given " << typeid(T).name() << ")";
	    MRSERR(sstr.str());
	    return MarControlValueT<T>::invalidValue;
	}
}

/************************************************************************/
/* MarControl inline implementation                                     */
/************************************************************************/
inline
MarControl::MarControl(const MarControl& a)
{
	refCount_ = 0;
	msys_			= a.msys_;
	cname_		= a.cname_;
	state_		= a.state_;
	desc_			= a.desc_;
	value_		= a.value_->clone();
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

inline
MarControl::MarControl(MarControlValue *value, std::string cname, MarSystem* msys, bool state)
{
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= value->clone();
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

inline
MarControl::MarControl(double re, std::string cname, MarSystem* msys, bool state)
{
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<mrs_real>(re);
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}


inline
MarControl::MarControl(float re, std::string cname, MarSystem* msys, bool state)
{
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<mrs_real>(re);
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}


inline
MarControl::MarControl(mrs_natural ne, std::string cname, MarSystem* msys, bool state)
{
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<mrs_natural>(ne);
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

inline
MarControl::MarControl(std::string st, std::string cname, MarSystem* msys, bool state)
{
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<std::string>(st);
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

inline
MarControl::MarControl(mrs_bool be, std::string cname, MarSystem* msys, bool state)
{
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<bool>(be);
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this));
}

inline
MarControl::MarControl(realvec& ve, std::string cname, MarSystem* msys, bool state)
{
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<realvec>(ve);
	value_->links_.push_back(std::pair<MarControl*, MarControl*>(this, this)); 
}

inline
MarControl::~MarControl()
{
	//first unlink this control from everything
	this->unlinkFromAll();
	//now we can safely delete its uniquely owned MarControlValue
	delete value_;
}

inline
MarControl& 
MarControl::operator=(const MarControl& a)
{
	if (this != &a)
	{
		this->setValue(a.value_);
		/*
		refCount_ = 0;
		msys_			= a.msys_;
		cname_		= a.cname_;
		state_		= a.state_;
		desc_			= a.desc_;
		value_		= a.value_->clone();
		*/
	}
	return *this;
}
 
// setters
template<class T>
inline
bool
MarControl::setValue(T& t, bool update)
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

template<class T>
inline
bool
MarControl::setValue(const T& t, bool update)
{
	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(value_);
	if(ptr)
	{
		if (ptr->get() == t)
			return true;

		ptr->set(const_cast<T&>(t), update);

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

inline
bool
MarControl::setValue(MarControlPtr mc, bool update)
{
	if (value_->type_ != mc->value_->type_)
	{
		std::ostringstream sstr;
		sstr << "MarControl::setValue() - Trying to set value of incompatible type "
			<< "(expected " << value_->type_ << ", given " << mc->value_->type_ << ")";
		MRSWARN(sstr.str());
		return false;
	}

	if (MarControlPtr(this) == mc)
	{
		return true;
	}
	
	value_->copyValue(*(mc->value_));

	#ifdef MARSYAS_TRACECONTROLS
	value_->setDebugValue();
	#endif

	//check if it's needed to call update()
	if(update)
		value_->callMarSystemsUpdate();

	return true;
}

inline
bool
MarControl::setValue(MarControlValue *mcv, bool update)
{
	if (value_->type_ != mcv->type_)
	{
		std::ostringstream sstr;
		sstr << "MarControl::setValue() - Trying to set value of incompatible type "
			<< "(expected " << value_->type_ << ", given " << mcv->type_ << ")";
		MRSWARN(sstr.str());
		return false;
	}

	if (!mcv->isNotEqual(value_))
	{
		return true;
	}

	value_->copyValue(*(mcv));

	#ifdef MARSYAS_TRACECONTROLS
	value_->setDebugValue();
	#endif
	
	//check if it's needed to call update()
	if(update)
		value_->callMarSystemsUpdate();

	return true;
}

inline
bool
MarControl::setValue(const char *t, bool update)
{
	return this->setValue(std::string(t), update);
}

inline
bool
MarControl::setValue(int t, bool update)
{
	return this->setValue((mrs_natural)t, update);
}

inline
bool
MarControl::isTrue()
{
	MarControlValueT<bool> *ptr = dynamic_cast<MarControlValueT<bool>*>(value_);
	if(ptr)
	{
		return ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "MarControl::isTrue() - Trying to get use bool-specific method with " << value_->getType(); 
		MRSWARN(sstr.str());
		return false;
	}
}

inline
std::ostream&
operator<<(std::ostream& os, const MarControl& ctrl)
{
	return ctrl.value_->serialize(os);
}

inline
bool
operator==(const MarControl& v1, const MarControl& v2)
{
	return !(v1.value_->isNotEqual(v2.value_));
}

inline
bool
operator!=(const MarControl& v1, const MarControl& v2)
{
	return v1.value_->isNotEqual(v2.value_);
}

inline
mrs_real
operator+(const MarControl& v1, const mrs_real& v2)
{
	mrs_real r1;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
	if(ptr)
	{
		r1 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "MarControl::operator + : Trying to get value of incompatible type "
			<< "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return r1 + v2;
}

inline
mrs_real
operator+(const mrs_real& v1, const MarControl& v2)
{
	mrs_real r2;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
	if(ptr)
	{
		r2 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "MarControl::operator + : Trying to get value of incompatible type "
			<< "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return v1 + r2;
}

inline
mrs_real
operator-(const MarControl& v1, const mrs_real& v2)
{
	mrs_real r1;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
	if(ptr)
	{
		r1 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to get value of incompatible type "
			<< "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return r1 - v2;
}

inline
mrs_real
operator-(const mrs_real& v1, const MarControl& v2)
{
	mrs_real r2;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
	if(ptr)
	{
		r2 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to get value of incompatible type "
			<< "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return v1 - r2;
}

inline
mrs_real
operator*(const MarControl& v1, const mrs_real& v2)
{
	mrs_real r1;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
	if(ptr)
	{
		r1 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to get value of incompatible type "
			<< "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return r1 * v2;
}

inline
mrs_real
operator*(const mrs_real& v1, const MarControl& v2)
{
	mrs_real r2;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
	if(ptr)
	{
		r2 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to get value of incompatible type "
			<< "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return v1 * r2;
}

inline
mrs_real
operator/(const MarControl& v1, const mrs_real& v2)
{
	mrs_real r1;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v1.value_);
	if(ptr)
	{
		r1 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to get value of incompatible type "
			<< "(expected " << v1.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return r1 / v2;
}

inline
mrs_real
operator/(const mrs_real& v1, const MarControl& v2)
{
	mrs_real r2;
	MarControlValueT<mrs_real> *ptr = dynamic_cast<MarControlValueT<mrs_real>*>(v2.value_);
	if(ptr)
	{
		r2 = ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to get value of incompatible type "
			<< "(expected " << v2.getType() << ", given " << typeid(mrs_real).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
	return v1 / r2;
}

inline
MarControl
operator+(const MarControl& v1, const MarControl& v2)
{
	MarControlValue *val = v1.value_->sum(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

inline
MarControl
operator-(const MarControl& v1, const MarControl& v2)
{
	MarControlValue *val = v1.value_->subtract(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

inline
MarControl
operator*(const MarControl& v1, const MarControl& v2)
{
	MarControlValue *val = v1.value_->multiply(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

inline
MarControl
operator/(const MarControl& v1, const MarControl& v2)
{
	MarControlValue *val = v1.value_->divide(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

}//namespace Marsyas

#include "MarControlAccessor.h"

#endif /* __MARCONTROL__ */
