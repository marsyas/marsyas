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
\class MarControl
\brief 
MarControl is a smart pointer wrapper for MarControlValue.
Created by lfpt@inescporto.pt and lmartins@inescporto.pt

*/

#ifndef __MARCONTROL__
#define __MARCONTROL__

#include <string> 
#include <iostream>
#include <vector>

#include "common.h"
#include "MarControlValue.h"
#include "realvec.h"

#ifdef MRSDEBUGGING
#include <sstream>
#endif

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

#ifdef MARSYAS_QT
#include <QtCore>
#endif

namespace Marsyas
{
	static const bool NOUPDATE = false;
		
	class MarSystem;
	class MarControl;
	class MarControlManager;

class MarControlPtr
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
	inline MarControlPtr(mrs_real re);
	inline MarControlPtr(char *c);
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

#ifdef MARSYAS_QT
class MarControl : public QObject
#else
class MarControl
#endif
{
	friend class MarControlManager;
	

#ifdef MARSYAS_QT
	Q_OBJECT
#endif

#ifdef MARSYAS_QT
protected:
	mutable QReadWriteLock rwLock_;
#endif

protected:
	int refCount_;

	MarControlValue *value_;
	MarSystem* msys_;
	std::string cname_;
	bool state_;
	std::string desc_;

#ifdef MRSDEBUGGING
	std::string value_debug_;
#endif

	// for link controls
	bool isLinked_;
	std::vector<MarControlPtr> linkedTo_;

	// default constructor
	MarControl() {} // not allowed

public:
	// copy constructor
	inline MarControl(const MarControl& a);

	// generic type constructor
	inline MarControl(MarControlValue *value, std::string cname = "", MarSystem* msys = 0, bool state = false);

	// basic types constructors / for compatibility purposes
	inline MarControl(mrs_real re, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(mrs_natural ne, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(std::string st, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(bool be, std::string cname = "", MarSystem* msys = 0, bool state = false);
	inline MarControl(realvec& ve, std::string cname = "", MarSystem* msys = 0, bool state = false);

	// destructor
	~MarControl();

	MarControl& operator=(const MarControl& a);

	MarControl* clone();

	inline void ref() { refCount_++; }
	inline void unref() {	if (--refCount_ <= 0) delete this; }
	int getRefCount() const { return refCount_; }

	void setMarSystem(MarSystem* msys);
	MarSystem* getMarSystem();
	void setName(std::string cname);
	std::string getName() const;
	void setState(bool state);
	bool hasState() const;
	std::string getType() const; // { return value_->getType(); }

	// for link controls
	bool isLinked() const { return isLinked_; }
	bool linkTo(MarControlPtr ctrl);
	std::vector<MarControlPtr>& getLinks() { return linkedTo_; }
	void clearLinks();
	void removeLink(MarControlPtr link);

	//////////////////////////////////////////////////////////////////////////
	// helper functions for basic types
	//////////////////////////////////////////////////////////////////////////
	// setters
	inline bool setValue(MarControlPtr mc, bool update = true);	
	inline bool setValue(MarControlValue *mcv, bool update = true);	
	template<class T> inline bool setValue(const T& t, bool update = true);
	template<class T> inline bool setValue(T& t, bool update = true);

	inline bool setValue(mrs_natural i, mrs_real value, bool update = true);	// to use with realvec MarControls
	inline bool setValue(mrs_natural r, mrs_natural c, mrs_real value, bool update = true);	// to use with realvec MarControls
	inline bool setValue(int i, mrs_real value, bool update = true) { return setValue((mrs_natural)i, value, update); }
	inline bool setValue(int r, int c, mrs_real value, bool update = true)	{ return setValue((mrs_natural)r, (mrs_natural)c, value, update); }

	inline bool setValue(const char *t, bool update = true);
	inline bool setValue(int t, bool update = true);

	// to avoid circular dependencies
	void callMarSystemUpdate();

	// getter by parameter (same interface for all types)
	// note: returned value will be a copy
	template<class T> inline bool getValue(T& t) const;

	// getters by return (user must know the parameter's type)
	template<class T> inline const T& to() const;
	// note: kept for compatibility purposes
	inline const mrs_real& toReal() const;
	inline const mrs_natural& toNatural() const;
	inline const bool& toBool() const;
	inline const std::string& toString() const;
	inline const realvec& toVec() const;

	// bool-specific helper
	bool isTrue();

	// realvec-specific helpers
	void stretch(mrs_natural rows, mrs_natural cols);
	void stretch(mrs_natural size);
	mrs_real operator()(const mrs_natural i) const;
	mrs_real operator()(const mrs_natural r, const mrs_natural c) const;
	//~

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

#ifdef MARSYAS_QT
protected:
	void emitControlChanged(MarControl* control);//inline

	// signals:
	// 	void controlChanged(MarControl* value);
#endif
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

inline MarControlPtr::MarControlPtr(mrs_real re)
{
	control_ = new MarControl(re);
	control_->ref();
	TRACE_ADDCONTROL;
}

inline MarControlPtr::MarControlPtr(char *c)
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

/************************************************************************/
/* MarControl template implementation                                   */
/************************************************************************/
// getters by parameter (same interface for all types)
// note: returned value will be a copy
template<class T>
bool
MarControl::getValue(T& t) const
{
	#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_);
	#endif 
	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(value_);
	if(ptr)
	{
		t = ptr->get();
		return true;
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::getValue] Trying to set value of incompatible type "
			<< "(expected " << value_->getType() << ", given " << typeid(T).name() << ")";
		MRSWARN(sstr.str());
		return false;
	}
}

template<class T>
const T&
MarControl::to() const
{
	#ifdef MARSYAS_QT
	QReadLocker locker(&rwLock_);
	#endif 
	const MarControlValueT<T> *ptr = dynamic_cast<const MarControlValueT<T>*>(value_);
	if(ptr)
	{
	    return ptr->get();
	}
	else
	{
	    std::ostringstream sstr;
	    sstr << "[MarControl::to] Trying to set value of incompatible type "
		 << "(expected " << value_->getType() << ", given " << typeid(T).name() << ")";
	    MRSWARN(sstr.str());
	    return MarControlValueT<T>::invalidValue;
	}
 }

/************************************************************************/
/* MarControlPtr inline implementation                                  */
/************************************************************************/
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

/************************************************************************/
/* MarControl inline implementation                                     */
/************************************************************************/
inline
MarControl::MarControl(const MarControl& a)
{
	#ifdef MARSYAS_QT
	qRegisterMetaType<MarControl*>("MarControl*");
	QWriteLocker locker_w(&rwLock_);
	QReadLocker locker_r(&(a.rwLock_));
	#endif
	refCount_ = 0;
	msys_			= a.msys_;
	cname_		= a.cname_;
	state_		= a.state_;
	desc_			= a.desc_;
	value_		= a.value_->clone();
	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif
	isLinked_ = false;
}

inline
MarControl::MarControl(MarControlValue *value, std::string cname, MarSystem* msys, bool state)
{
	#ifdef MARSYAS_QT
	qRegisterMetaType<MarControl*>("MarControl*");
	#endif
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= value->clone();
	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif
	isLinked_ = false;
}

inline
MarControl::MarControl(mrs_real re, std::string cname, MarSystem* msys, bool state)
{
	#ifdef MARSYAS_QT
	qRegisterMetaType<MarControl*>("MarControl*");
	#endif
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<mrs_real>(re);
	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif
	isLinked_ = false;
}

inline
MarControl::MarControl(mrs_natural ne, std::string cname, MarSystem* msys, bool state)
{
	#ifdef MARSYAS_QT
	qRegisterMetaType<MarControl*>("MarControl*");
	#endif
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<mrs_natural>(ne);
	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif
	isLinked_ = false;
}

inline
MarControl::MarControl(std::string st, std::string cname, MarSystem* msys, bool state)
{
	#ifdef MARSYAS_QT
	qRegisterMetaType<MarControl*>("MarControl*");
	#endif
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<std::string>(st);
	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif
	isLinked_ = false;
}

inline
MarControl::MarControl(mrs_bool be, std::string cname, MarSystem* msys, bool state)
{
	#ifdef MARSYAS_QT
	qRegisterMetaType<MarControl*>("MarControl*");
	#endif
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<bool>(be);
	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif
	isLinked_ = false;
}

inline
MarControl::MarControl(realvec& ve, std::string cname, MarSystem* msys, bool state)
{
	#ifdef MARSYAS_QT
	qRegisterMetaType<MarControl*>("MarControl*");
	#endif
	refCount_ = 0;
	msys_			= msys;
	cname_		= cname;
	state_		= state;
	desc_			= "";
	value_		= new MarControlValueT<realvec>(ve);
	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif
	isLinked_ = false;
}

inline
MarControl::~MarControl()//mutexes? [?]
{
	if (value_)
	{
		delete value_;
	}
}

inline
MarControl& 
MarControl::operator=(const MarControl& a)
{
	#ifdef MARSYAS_QT
	QWriteLocker locker_w(&rwLock_);
	QReadLocker locker_r(&(a.rwLock_));
	#endif
	if (this != &a)
	{
		this->setValue(a.value_);
		/*refCount_ = 0;
		msys_			= a.msys_;
		cname_		= a.cname_;
		state_		= a.state_;
		desc_			= a.desc_;
		value_		= a.value_->clone();*/
	}
	return *this;
}
 
// setters
template<class T>
inline
bool
MarControl::setValue(T& t, bool update)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	#endif

	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(value_);
	if(ptr)
	{
		if (ptr->get() == t)
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock();
			#endif	
			return true;
		}

		ptr->set(t);

		#ifdef MRSDEBUGGING
		std::ostringstream oss;
		value_->serialize(oss);
		value_debug_ = oss.str();
		#endif

		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif

		if(isLinked_)
		{
			for(size_t i=0; i<linkedTo_.size(); i++)
			{
				linkedTo_[i]->setValue(t, update);
			}
		}

		if(update) this->callMarSystemUpdate();

		#ifdef MARSYAS_QT
		//emit controlChanged(this);
		emitControlChanged(this);
		#endif
		return true;
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to set value of incompatible type "
			<< "(expected " << value_->getType() << ", given " << typeid(T).name() << ")";
		MRSWARN(sstr.str());

		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif

		return false;
	}
}

template<class T>
inline
bool
MarControl::setValue(const T& t, bool update)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	#endif

	MarControlValueT<T> *ptr = dynamic_cast<MarControlValueT<T>*>(value_);
	if(ptr)
	{
		if (ptr->get() == t)
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock();
			#endif
			return true;
		}

		ptr->set(const_cast<T&>(t));

		#ifdef MRSDEBUGGING
		std::ostringstream oss;
		value_->serialize(oss);
		value_debug_ = oss.str();
		#endif

		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif

		if(isLinked_)
		{
			for(size_t i=0; i<linkedTo_.size(); i++)
			{
				linkedTo_[i]->setValue(t, update);
			}
		}

		if(update) this->callMarSystemUpdate();

		#ifdef MARSYAS_QT
		//emit controlChanged(this);
		emitControlChanged(this);
		#endif

		return true;
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to set value of incompatible type "
			<< "(expected " << value_->getType() << ", given " << typeid(T).name() << ")";
		MRSWARN(sstr.str());

		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif

		return false;
	}
}

inline
bool
MarControl::setValue(MarControlPtr mc, bool update)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
#endif
	if (value_->getType() != mc->value_->getType())
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to set value of incompatible type "
			<< "(expected " << value_->getType() << ", given " << mc->value_->getType() << ")";
		MRSWARN(sstr.str());
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		return false;
	}

	if (MarControlPtr(this) == mc)
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif	
		return true;
	}

	delete value_;
	value_ = mc->value_->clone();

	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif

	#ifdef MARSYAS_QT
	rwLock_.unlock();
	#endif

	if(isLinked_)
	{
		for(size_t i=0; i<linkedTo_.size(); i++)
		{
			linkedTo_[i]->setValue(value_, update);
		}
	}

	if(update) this->callMarSystemUpdate();

	#ifdef MARSYAS_QT
	//emit controlChanged(this);
	emitControlChanged(this);
	#endif

	return true;
}

inline
bool
MarControl::setValue(MarControlValue *mcv, bool update)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	#endif
	if (value_->getType() != mcv->getType())
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to set value of incompatible type "
			<< "(expected " << value_->getType() << ", given " << mcv->getType() << ")";
		MRSWARN(sstr.str());
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		return false;
	}

	if (!mcv->isNotEqual(value_))
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif	
		return true;
	}

	delete value_;
	value_ = mcv->clone();

	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif

	#ifdef MARSYAS_QT
	rwLock_.unlock();
	#endif

	if(isLinked_)
	{
		for(size_t i=0; i<linkedTo_.size(); i++)
		{
			linkedTo_[i]->setValue(value_, update);
		}
	}

	if(update) this->callMarSystemUpdate();

	#ifdef MARSYAS_QT
	//emit controlChanged(this);
	emitControlChanged(this);
	#endif

	return true;
}

inline
bool
MarControl::setValue(mrs_natural i, mrs_real value, bool update)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	#endif

	MarControlValueT<realvec> *ptr = dynamic_cast<MarControlValueT<realvec>*>(value_);
	if(ptr)
	{
		if (ptr->getRef()(i) == value)
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock();
			#endif
			return true; // assuming all linked controls are synced we can return immediately 
		}
		ptr->getRef()(i) = value;
	}
	else
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		std::ostringstream sstr;
		sstr << "[MarControl::create] Trying to use realvec-specific method with " << value_->getType();
		MRSWARN(sstr.str());
		return false;
	}

	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif

	#ifdef MARSYAS_QT
	rwLock_.unlock();
	#endif

	if(isLinked_)
	{
		for(mrs_natural i=0; i<(mrs_natural)linkedTo_.size(); i++)
		{
			linkedTo_[i]->setValue(i, value, update);
		}
	}

	if (update) this->callMarSystemUpdate();

	#ifdef MARSYAS_QT
	//emit controlChanged(this);
	emitControlChanged(this);
	#endif

	return true;
}

bool
MarControl::setValue(mrs_natural r, mrs_natural c, mrs_real value, bool update)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	#endif

	MarControlValueT<realvec> *ptr = dynamic_cast<MarControlValueT<realvec>*>(value_);
	if(ptr)
	{
		if (ptr->getRef()(r,c) == value)
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock();
			#endif
			return true; // assuming all linked controls are synced we can return immediately 
		}
		ptr->getRef()(r,c) = value;
	}
	else
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		std::ostringstream sstr;
		sstr << "[MarControl::create] Trying to use realvec-specific method with " << value_->getType();
		MRSWARN(sstr.str());
		return false;
	}

	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif

	#ifdef MARSYAS_QT
	rwLock_.unlock();
	#endif

	if(isLinked_)
	{
		for(size_t i=0; i<linkedTo_.size(); i++)
		{
			linkedTo_[i]->setValue(r, c, value, update);
		}
	}

	if (update) this->callMarSystemUpdate();

	#ifdef MARSYAS_QT
	//emit controlChanged(this);
	emitControlChanged(this);
	#endif

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
const mrs_real&
MarControl::toReal() const
{
	return to<mrs_real>();
} 

inline
const mrs_natural&
MarControl::toNatural() const
{
	return to<mrs_natural>();
}

inline
const bool&
MarControl::toBool() const
{
	return to<bool>();
}

inline
const std::string&
MarControl::toString() const
{
	return to<std::string>();
}

inline
const realvec&
MarControl::toVec() const
{
	return to<realvec>();
}

inline
bool
MarControl::isTrue()
{
	#ifdef MARSYAS_QT
	QReadLocker locker_r(&rwLock_); //[!] added by lmartins
	#endif
	MarControlValueT<bool> *ptr = dynamic_cast<MarControlValueT<bool>*>(value_);
	if(ptr)
	{
		return ptr->get();
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::setValue] Trying to get use bool-specific method with " << value_->getType(); 
		MRSWARN(sstr.str());
		return false;
	}
}

inline
void
MarControl::stretch(mrs_natural rows, mrs_natural cols)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	#endif

	MarControlValueT<realvec> *ptr = dynamic_cast<MarControlValueT<realvec>*>(value_);
	if(ptr)
	{
		if (ptr->getRef().getRows() == rows && ptr->getRef().getCols() == cols)
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock();
			#endif
			return; // assuming all linked controls are synced we can return immediately 
		}
		ptr->getRef().stretch(rows, cols);
	}
	else
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		std::ostringstream sstr;
		sstr << "[MarControl::create] Trying to use realvec-specific method with " << value_->getType();
		MRSWARN(sstr.str());
		return;
	}

	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif

	#ifdef MARSYAS_QT
	rwLock_.unlock();
	#endif

	if(isLinked_)
	{
		for(size_t i=0; i<linkedTo_.size(); i++)
		{
			linkedTo_[i]->stretch(rows, cols);
		}
	}

	this->callMarSystemUpdate();

	#ifdef MARSYAS_QT
	//emit controlChanged(this);
	emitControlChanged(this);
	#endif
}

inline
void
MarControl::stretch(mrs_natural size)
{
	#ifdef MARSYAS_QT
	rwLock_.lockForWrite();
	#endif

	MarControlValueT<realvec> *ptr = dynamic_cast<MarControlValueT<realvec>*>(value_);
	if(ptr)
	{
		if (ptr->getRef().getSize() == size)
		{
			#ifdef MARSYAS_QT
			rwLock_.unlock();
			#endif
			return;  // assuming all linked controls are synced we can return immediately 
		}
		ptr->getRef().stretch(size);
	}
	else
	{
		#ifdef MARSYAS_QT
		rwLock_.unlock();
		#endif
		std::ostringstream sstr;
		sstr << "[MarControl::create] Trying to use realvec-specific method with " << value_->getType();
		MRSWARN(sstr.str());
		return;
	}

	#ifdef MRSDEBUGGING
	std::ostringstream oss;
	value_->serialize(oss);
	value_debug_ = oss.str();
	#endif

	#ifdef MARSYAS_QT
	rwLock_.unlock();
	#endif

	if(isLinked_)
	{
		for(size_t i=0; i<linkedTo_.size(); i++)
		{
			linkedTo_[i]->stretch(size);
		}
	}

	this->callMarSystemUpdate();

	#ifdef MARSYAS_QT
	//emit controlChanged(this);
	emitControlChanged(this);
	#endif
}

inline
mrs_real
MarControl::operator()(const mrs_natural i) const
{
	#ifdef MARSYAS_QT
	QReadLocker locker(&(ctrl.rwLock_));
	#endif

	MarControlValueT<realvec> *ptr = dynamic_cast<MarControlValueT<realvec>*>(value_);
	if(ptr)
	{
		return ptr->getRef()(i);
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::operator()] Trying to get use realvec-specific method with " << value_->getType();
		MRSWARN(sstr.str());
		return MarControlValueT<mrs_real>::invalidValue;
	}
}

inline
mrs_real
MarControl::operator()(const long r, const long c) const
{
	#ifdef MARSYAS_QT
	QReadLocker locker(&(ctrl.rwLock_));
	#endif

	MarControlValueT<realvec> *ptr = dynamic_cast<MarControlValueT<realvec>*>(value_);
	if(ptr)
	{
		return ptr->getRef()(r, c);
	}
	else
	{
		std::ostringstream sstr;
		sstr << "[MarControl::operator()] Trying to get use realvec-specific method with " << value_->getType();
		MRSWARN(sstr.str());
		return MarControlValueT<mrs_real>::invalidValue;
	}
}

inline
std::ostream&
operator<<(std::ostream& os, const MarControl& ctrl)
{
	#ifdef MARSYAS_QT
	QReadLocker locker(&(ctrl.rwLock_));
	#endif
	return ctrl.value_->serialize(os);
}

inline
bool
operator==(const MarControl& v1, const MarControl& v2)
{
	#ifdef MARSYAS_QT
	QReadLocker locker1(&(v1.rwLock_));
	QReadLocker locker2(&(v2.rwLock_));
#endif
	return !(v1.value_->isNotEqual(v2.value_));
}

inline
bool
operator!=(const MarControl& v1, const MarControl& v2)
{
	#ifdef MARSYAS_QT
	QReadLocker locker1(&(v1.rwLock_));
	QReadLocker locker2(&(v2.rwLock_));
	#endif
	return v1.value_->isNotEqual(v2.value_);
}

inline
mrs_real
operator+(const MarControl& v1, const mrs_real& v2)//[?] lock?!?
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
	return r1 + v2;
}

inline
mrs_real
operator+(const mrs_real& v1, const MarControl& v2)//[?] lock?!?
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
	return v1 + r2;
}

inline
mrs_real
operator-(const MarControl& v1, const mrs_real& v2)//[?] lock?!?
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
operator-(const mrs_real& v1, const MarControl& v2)//[?] lock?!?
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
operator*(const MarControl& v1, const mrs_real& v2)//[?] lock?!?
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
operator*(const mrs_real& v1, const MarControl& v2)//[?] lock?!?
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
operator/(const MarControl& v1, const mrs_real& v2)//[?] lock?!?
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
operator/(const mrs_real& v1, const MarControl& v2)//[?] lock?!?
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
operator+(const MarControl& v1, const MarControl& v2)//[?] lock?!?
{
	MarControlValue *val = v1.value_->sum(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

inline
MarControl
operator-(const MarControl& v1, const MarControl& v2)//[?] lock?!?
{
	MarControlValue *val = v1.value_->subtract(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

inline
MarControl
operator*(const MarControl& v1, const MarControl& v2)//[?] lock?!?
{
	MarControlValue *val = v1.value_->multiply(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

inline
MarControl
operator/(const MarControl& v1, const MarControl& v2)//[?] lock?!?
{
	MarControlValue *val = v1.value_->divide(v2.value_);
	MarControl ret(val);
	delete val;
	return ret;
}

// #ifdef MARSYAS_QT
// inline
// void
// MarControlValue::emitControlChanged(MarControlValue* cvalue)
// {
// 	//only bother calling MarSystem's controlChanged signal
// 	//if there is a GUI currently active(i.e. being displayed)
// 	//=> more efficient! [!]
// 	if(msys_->controlsGUI_ || msys_->dataGUI_)//possible because this class is friend of MarSystem //[!]
// 	{
// 		Q_ASSERT(QMetaObject::invokeMethod(msys_, "controlChanged", Qt::AutoConnection,
// 			Q_ARG(MarControlValue*, cvalue)));
// 	}
// }
// #endif //MARSYAS_QT 

}//namespace Marsyas

#endif /* __MARCONTROL__ */
