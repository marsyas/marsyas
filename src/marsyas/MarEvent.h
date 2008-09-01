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


#ifndef MARSYAS_MAREVENT_H
#define MARSYAS_MAREVENT_H

#include "Repeat.h"
#include "TmControlValue.h"
#include <iostream>

namespace Marsyas
{
/**
	\class MarEvent
	\ingroup Scheduler
	\brief MarEvent
	\author Neil Burroughs  inb@cs.uvic.ca
*/

class MarEvent {
protected:
	/** \brief the type of the event */
	std::string type_;
	/** \brief a unique identifier for the event. There is no checking for
	* uniqueness. If a search is made for a unique name the first hit will
	* be taken. */
	std::string name_;
	/** \brief repeat info for this event */
	Repeat repeat_;

public:
	MarEvent();
	MarEvent(std::string t, std::string n);

	virtual ~MarEvent();

	std::string getType() const { return type_; }
	std::string getName() const { return name_; }
	/** \brief the prefix is a concatenation of type and name as "type/name"
	* \return string representation of the prefix
	*/
	std::string getPrefix() const { return type_ + "/" + name_; }
	void setName(std::string n) { name_=n; }
	
	/** \brief the action to be performed by the event. This method is
	* called when the event is due. Since Marsyas is not threaded, this
	* method will block so be careful not to do too much.
	*/
	virtual void dispatch()=0;

	/** \brief clone this event and all its parameters. This method must
	* be implemented so that scheduled events may be copied.
	* \return a pointer to a copy of this event
	*/
	virtual MarEvent* clone()=0;

	/** \brief report if the event is to be repeated
	* \return true if event should repeat
	*/
	virtual bool repeat() { return repeat_.repeat(); };
	/** \brief set the repeat state of this event
	* \param r new repetition information for this event
	*/
	virtual void set_repeat(Repeat r) { repeat_=r; }
	/** \brief the reason for this method escaped me. It simply returns
	* parameter supplied but obviously could be overridden for whatever
	* reason. Although this method is never actually called by anyone
	* right now.
	* \param interval an interval of time
	* \return the same as what was supplied
	*/
	virtual std::string repeat_interval(std::string interval) { return interval; };
	/** \brief get the repeat interval for this event
	* \return the repeat interval as promised
	*/
	virtual std::string repeat_interval() { return repeat_.getInterval(); };

	/** \brief update event parameters dynamically. Parameters of the
	* event may be updated while the event is on the heap. The support for
	* modifiable parameters is dependent on the implementation of concrete
	* events. Unsupported parameter updates should generate warnings.
	* \param cname the path name of the parameter to be modified.
	* \param value the value to be assigned
	*/
	virtual void updctrl(std::string cname, TmControlValue value);
	/** \brief convenience method for concrete events to use when checking
	* types of values against path names during updctrl. This method
	* constitutes two checks (c1=c2 and v=t) that must be true to return
	* true.
	*
	* As an example, the call:
	* checkupd(cname,"mrs_string/control",value,tmcv_string)
	* might appear in updctrl(...) and checks the path name against the
	* specific parameter "mrs_string/control" for an exact match. Next,
	* the type of the value supplied will be checked against the type
	* enumeration tmcv_string. The type enumerations can be found in the
	* TmControlValue class.
	* \param c1 a path name to compare against c2
	* \param c2 a path name to compare against c1
	* \param v a control value to check
	* \param t a type enumeration from the TmControlValue class
	* representing the expected type
	* \return true if both checks succeed
	*/
	bool checkupd(std::string c1, std::string c2, TmControlValue v, mrs_natural t);

	// the usual stream IO 
	friend std::ostream& operator<<(std::ostream&, MarEvent&);
	friend std::istream& operator>>(std::istream&, MarEvent&);
};

}//namespace Marsyas

#endif
