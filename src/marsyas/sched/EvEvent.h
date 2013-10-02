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


#ifndef MARSYAS_EV_EVENT_H
#define MARSYAS_EV_EVENT_H

#include <marsyas/sched/Repeat.h>
#include <marsyas/sched/TmControlValue.h>

#include <iostream>

namespace Marsyas
{
/**
	\class EvEvent
	\ingroup Scheduler
	\brief EvEvent
	\author Neil Burroughs  inb@cs.uvic.ca
*/
class TmTimer;

class marsyas_EXPORT EvEvent {
protected:
  /** \brief the type of the event, probably the class name */
  std::string type_;
  /** \brief a unique identifier for the event. There is no checking for
  * uniqueness. If a search is made for a unique name the first hit will
  * be taken. */
  std::string name_;

  /** \brief the event dispatch time */
  mrs_natural time_;
  /** \brief repeat info for this event */
  Repeat repeat_;
  /** \brief the timer on which the event is to be dispatched */
  TmTimer* timer_;

public:
  EvEvent();
  EvEvent(std::string t, std::string n);

  virtual ~EvEvent();

  /** \brief get the type of the event which is usually the class name
  * \return the type name
  */
  std::string getType() const;
  /** \brief get the name of the event, hopefully a unique identifier
  * \return the event identifier
  */
  std::string getName() const;
  /** \brief the prefix is a concatenation of type and name as "type/name"
  * \return string representation of the prefix
  */
  std::string getPrefix() const;
  /** \brief set the name of the event, should be a unique identifier
  * \param n a unique identifier
  */
  void setName(std::string n);

  /** \brief the action to be performed by the event. This method is
  * called when the event is due. Since Marsyas is not threaded, this
  * method will block so be careful not to do too much.
  */
  virtual void dispatch()=0;

  /** \brief clone this event and all its parameters. This method must
  * be implemented so that scheduled events may be copied.
  * \return a pointer to a copy of this event
  */
  virtual EvEvent* clone()=0;

  /** \brief set the timer on which this event is scheduled. This method
  * is called by the timer's post method when the scheduled event is
  * posted on the timer.
  * \param t the timer on which this event is posted.
  */
  virtual void setTimer(TmTimer* t);
  /** \brief get the time that this event is to be dispatched
  * \return the dispatch time count for this event
  */
  mrs_natural getTime() const;
  /** \brief set the time at which this event is to be dispatched
  * \param t the dispatch time for this event
  */
  void setTime(mrs_natural t);


  /** \brief report if the event is to be repeated
  * \return true if event should repeat
  */
  virtual bool repeat();
  /** \brief set the repeat state of this event
  * \param r new repetition information for this event
  */
  virtual void setRepeat(Repeat r);
  /** \brief get the repetition count for this event
  * \return a pointer to the event object
  */
  virtual mrs_natural getRepeatCount();
  /** \brief get the repetition time interval for this event.
  *
  * If this event does not repeat then the returned time interval is
  * undefined (may be an empty string). It is best to check to see if
  * there is repeat information prior to reading the interval.
  * \return string representation of the time interval
  */
  virtual std::string getRepeatInterval();
  /** \brief get the repetition information for this event
  * \return the repetition information for this event
  */
  Repeat getRepeat();

  /** \brief the reason for this method escapes me. It simply returns
  * parameter supplied but obviously could be overridden for whatever
  * reason. Although this method is never actually called by anyone
  * right now.
  * \param interval an interval of time
  * \return the same as what was supplied
  */
  virtual std::string repeat_interval(std::string interval);

  /** \brief force the event to update its dispatch time and decrement
  * its repeat count based on the repetition information. If the event
  * does not repeat then this method is meaningless and doesn't do
  * anything. This method is used by the dispatch() method of the timer.
  */
  void doRepeat();


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

  friend class EvEventDispatchComparator;

  // the usual stream IO
  friend std::ostream& operator<<(std::ostream&, EvEvent&);
  friend std::istream& operator>>(std::istream&, EvEvent&);
};

/**
	\class EvEventDispatchComparator
	\ingroup Scheduler
	\brief class for comparing dispatch times of events. required for insertion into the Heap.
	\author Neil Burroughs  inb@cs.uvic.ca
*/
class EvEventDispatchComparator {
public:
  bool operator()(EvEvent* a, EvEvent* b) {
    return (a->getTime()) < (b->getTime());
  }
};

}//namespace Marsyas

#endif
