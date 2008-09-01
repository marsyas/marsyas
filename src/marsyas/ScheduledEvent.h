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

#ifndef MARSYAS_SCHEDULEDEVENT_H
#define MARSYAS_SCHEDULEDEVENT_H

#include <cstdio>
#include <string>
#include <iostream>

#include "MarControlValue.h"
#include "MarEvent.h"
#include "Repeat.h"

namespace Marsyas
{
/**
	\class Scheduler
	\ingroup Scheduler
	\brief Scheduler schedules things

	Scheduled event contains a control name, and a MarControlValue along
	with a sample time that the name and value should be issued at.

	\author Neil Burroughs  inb@cs.uvic.ca
*/

class TmTimer;

class ScheduledEvent {
protected:
	/** \brief the event dispatch time */
	mrs_natural time_;
	/** \brief the event to be dispatched */
	MarEvent* event_;
	/** \brief the repetition information for the event */
	Repeat repetition_;
	/** \brief the timer on which the event is to be dispatched */
	TmTimer* timer_;

public:
	/** \brief an empty scheduled event */
	ScheduledEvent();
	/** \brief a scheduled event that does not repeat
	* \param time the time of the event dispatch
	* \param event the event to be dispatched
	*/
	ScheduledEvent(mrs_natural time, MarEvent* event);
	/** \brief a scheduled event with repetition information
	* \param time the time of the event dispatch
	* \param rep the repetition information for the event
	* \param event the event to be dispatched
	*/
	ScheduledEvent(mrs_natural time, Repeat rep, MarEvent* event);
	/** \brief copy constructor
	* \param s the scheduled event to copy
	*/
	ScheduledEvent(const ScheduledEvent& s);
	/** \brief destructor */
	virtual ~ScheduledEvent();

	/** \brief set the event information without repetition.
	*
	* This method is called by the corresponding constructor. It can be
	* used after the empty constructor.
	* \param time the time of the event dispatch
	* \param event the event to be dispatched
	*/
	void setEvent(mrs_natural time, MarEvent* event);
	/** \brief set the event information with repetition.
	*
	* This method is called by the corresponding constructor. It can be
	* used after the empty constructor.
	* \param time the time of the event dispatch
	* \param event the event to be dispatched
	*/
	void setEvent(mrs_natural time, Repeat rep, MarEvent* event);
	/** \brief set the timer on which this event is scheduled. This method
	* is called by the timer's post method when the scheduled event is
	* posted on the timer.
	* \param t the timer on which this event is posted.
	*/
	void setTimer(TmTimer* t);

	/** \brief get the time that this event is to be dispatched
	* \return the dispatch time count for this event
	*/
	mrs_natural getTime() const { return time_; }
	/** \brief set the time at which this event is to be dispatched
	* \param t the dispatch time for this event
	*/
	void setTime(mrs_natural t) { time_=t; }

	/** \brief get the actual event that this scheduled event contains
	*/
	MarEvent* getEvent() { return event_; }
	/** \brief get the repetition count for this event
	* \return a pointer to the event object
	*/
	mrs_natural getRepetitionCount();
	/** \brief get the repetition time interval for this event.
	*
	* If this event does not repeat then the returned time interval is
	* undefined (may be an empty string). It is best to check to see if
	* there is repeat information prior to reading the interval.
	* \return string representation of the time interval
	*/
	std::string getRepetitionInterval();
	/** \brief get the repetition information for this event
	* \return the repetition information for this event
	*/
	Repeat getRepetition();

	/** \brief answer whether this event repeats or not
	* \return true if this event repeats, false if not.
	*/
	bool repeat();
	/** \brief force the event to update its dispatch time and decrement
	* its repeat count based on the repetition information. If the event
	* does not repeat then this method is meaningless and doesn't do
	* anything. This method is used by the dispatch() method of the timer.
	*/
	void doRepeat();

	friend class ScheduledEventOrdering;

//    int operator=(ScheduledEvent& a);
	ScheduledEvent& operator=(ScheduledEvent& a);
	bool operator<(const ScheduledEvent&) const;
	bool operator>(const ScheduledEvent&) const;

	// the usual stream IO 
	friend std::ostream& operator<<(std::ostream&, ScheduledEvent&);
	friend std::istream& operator>>(std::istream&, ScheduledEvent&);
};

/**
	\class ScheduledEventComparator
	\ingroup Scheduler
	\brief class for comparing scheduled events by their event times

	The comparator is required for insertion into the Heap.

	\author Neil Burroughs  inb@cs.uvic.ca
*/
class ScheduledEventComparator {
public:
	bool operator()(ScheduledEvent* a, ScheduledEvent* b) {
		return (a->getTime()) < (b->getTime());
	}
};

}//namespace Marsyas

#endif
