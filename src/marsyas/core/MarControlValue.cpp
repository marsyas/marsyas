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

#include "common_source.h"
#include "MarControlValue.h"
#include "MarControl.h"
#include "MarControlManager.h"
#include <algorithm>


using std::ostringstream;
using namespace Marsyas;


/************************************************************************/
/* MarControlValue implementation                                       */
/************************************************************************/

void
MarControlValue::setDebugValue()
{
#ifdef MARSYAS_TRACECONTROLS
	ostringstream oss;
	serialize(oss);
	value_debug_ = oss.str();
#endif
}



mrs_string
MarControlValue::getType() const
{
	return type_;
}

mrs_string
MarControlValue::getRegisteredType()
{
	return MarControlManager::getManager()->getRegisteredType(this->getTypeID());
}


void 
MarControlValue::current_link_update() 
{
	current_link_->callMarSystemUpdate();
}


/************************************************************************/
/* MarControlValueT realvec specialization                              */
/************************************************************************/
realvec MarControlValueT<realvec>::invalidValue;

// constructor specialization for realvec
MarControlValueT<realvec>::MarControlValueT(realvec value)
{
	value_ = value;
	type_ = "mrs_realvec";

	setDebugValue();
}

MarControlValueT<realvec>::MarControlValueT(const MarControlValueT& a):MarControlValue(a)
{
	value_ = a.value_;
	type_ = "mrs_realvec";

	setDebugValue();

}

MarControlValueT<realvec>& 
MarControlValueT<realvec>::operator=(const MarControlValueT& a)
{
	if (this != &a)
	{
		value_ = a.value_;
		type_ = a.type_;

		setDebugValue();

	}
	return *this;
}

MarControlValue*
MarControlValueT<realvec>::clone()
{
	return new MarControlValueT<realvec>(*this);
}

void
MarControlValueT<realvec>::copyValue(MarControlValue& value)
{
	MarControlValueT<realvec> &v = dynamic_cast<MarControlValueT<realvec>&>(value);
	value_ = v.value_;
}

void
MarControlValueT<realvec>::callMarSystemsUpdate()
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
		lit_->first->callMarSystemUpdate(); //lit->first is a pointer to a MarControl*
	}
}

MarControlValue*
MarControlValueT<realvec>::create()
{
	return new MarControlValueT<realvec>(realvec());
}

const realvec&
MarControlValueT<realvec>::get() const
{
	return value_;
}

realvec&
MarControlValueT<realvec>::getRef()
{
	return value_;
}

bool
MarControlValueT<realvec>::isNotEqual(MarControlValue *v)
{
	if(this != v)//if referring to different objects, check if their contents is different...
	{
		if (type_ != "mrs_realvec")
		{
			MRSWARN("MarControlValueT::isNotEqual() - Types of MarControlValue are different");
		}
		return (value_ != dynamic_cast<MarControlValueT<realvec>*>(v)->get());
	}
	else //if v1 and v2 refer to the same object, they must be equal (=> return false)
		return false;
}

void
MarControlValueT<realvec>::createFromStream(std::istream& in)
{
	in >> value_;

	setDebugValue();
}

std::ostream&
MarControlValueT<realvec>::serialize(std::ostream& os)
{
	os << value_;
	return os;
}

MarControlValue*
MarControlValueT<realvec>::sum(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}

MarControlValue*
MarControlValueT<realvec>::subtract(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}

MarControlValue*
MarControlValueT<realvec>::multiply(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}

MarControlValue*
MarControlValueT<realvec>::divide(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}

/************************************************************************/
/* MarControlValueT bool specialization                                 */
/************************************************************************/
bool MarControlValueT<bool>::invalidValue;

// constructor specialization for realvec
MarControlValueT<bool>::MarControlValueT(bool value)
{
	value_ = value;
	type_ = "mrs_bool";


	setDebugValue();


}

MarControlValueT<bool>::MarControlValueT(const MarControlValueT& a):MarControlValue(a)
{
	value_ = a.value_;
	type_ = "mrs_bool";

	setDebugValue();
}

MarControlValueT<bool>& 
MarControlValueT<bool>::operator=(const MarControlValueT& a)
{
	if (this != &a)
	{
		value_ = a.value_;
		type_ = a.type_;

		setDebugValue();
		
	}
	return *this;
}

MarControlValue*
MarControlValueT<bool>::clone()
{
	return new MarControlValueT<bool>(*this);
}

void
MarControlValueT<bool>::copyValue(MarControlValue& value)
{
	MarControlValueT<bool> &v = dynamic_cast<MarControlValueT<bool>&>(value);
	value_ = v.value_;
}

void
MarControlValueT<bool>::callMarSystemsUpdate()
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
		lit_->first->callMarSystemUpdate(); //lit->first is a pointer to a MarControl*
	}
}

MarControlValue*
MarControlValueT<bool>::create()
{
	return new MarControlValueT<bool>(false);
}

const bool&
MarControlValueT<bool>::get() const
{
	return value_;
}

void
MarControlValueT<bool>::createFromStream(std::istream& in)
{
	in >> value_;

	setDebugValue();
}

bool
MarControlValueT<bool>::isNotEqual(MarControlValue *v)
{
	if(this != v)//if referring to different objects, check if their contents is different...
	{
		return (value_ != dynamic_cast<MarControlValueT<bool>*>(v)->get());
	}
	else //if v1 and v2 refer to the same object, they must be equal (=> return false)
	  return false;
}

std::ostream&
MarControlValueT<bool>::serialize(std::ostream& os)
{
	os << value_;
	return os;
}

MarControlValue*
MarControlValueT<bool>::sum(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}

MarControlValue*
MarControlValueT<bool>::subtract(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}

MarControlValue*
MarControlValueT<bool>::multiply(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}

MarControlValue*
MarControlValueT<bool>::divide(MarControlValue *v)
{
	(void) v;
	MRSASSERT(0); //not implemented
	return 0;
}
