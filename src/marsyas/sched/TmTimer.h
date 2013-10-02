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

#ifndef MARSYAS_TM_TIMER_H
#define MARSYAS_TM_TIMER_H

#include <marsyas/common_header.h>
#include <marsyas/sched/TmParam.h>
#include <marsyas/sched/TmControlValue.h>
#include <marsyas/sched/EvEvent.h>
#include <marsyas/Heap.h>

#include <map>
#include <iostream>
#include <string>
#include <vector>

namespace Marsyas
{
/**
	\class TmTimer
	\ingroup Scheduler
	\brief Base timer class must be overriden to create new concrete timers.
	\author inb@cs.uvic.ca
*/

class marsyas_EXPORT TmTimer
{
private:
  /** \brief heap containing the scheduled events */
  Heap<EvEvent, EvEventDispatchComparator> pq_;
  /** \brief map for events to allow modifying events while in the heap */
  std::map<std::string, EvEvent*> events_;
  std::map<std::string, EvEvent*>::iterator events_iter_;

protected:
  /** \brief type of the timer or class name */
  std::string type_;

  /** \brief given name identifier of the timer */
  std::string name_;

  /** \brief the current time count */
  unsigned long cur_time_;

//    mrs_natural granularity_;
//    mrs_natural next_trigger_;

protected:
  /** \brief post constructor initialization
  *
  * this method is called after the constructor in TmTimer is called.
  * This method is empty and may be overriden without a super.init() call.
  */
  void init();

public:
  // Constructors
  /** \brief the constructor requires the type and name
  */
  TmTimer(std::string type, std::string name);
  /** \brief copy constructor
  * \param t timer to copy
  */
  TmTimer(const TmTimer& t);
  virtual ~TmTimer();

  /** \brief get the timer type name, ie "TmSampleCount"
  * \return the type name
  */
  std::string getType();
  /** \brief get the timer identifier, ie "foo"
  * \return the type name
  */
  std::string getName();
  /** \brief get the timer prefix, the combination "type/name"
  * \return the prefix name
  */
  std::string getPrefix();

//    virtual TmTimer* clone()=0;

  /** \brief the current count of this timer
  * \return the current timer count
  */
  mrs_natural getTime();

  /** \brief called on each buffer passing through the network
  *
  * Tick calls readTimeSrc to adjust the current timer count. It then
  * calls the trigger method.
  */
  void tick();
  /** \brief called by tick() prior to events being triggered. This
  * method calls readTimeSrc() and adds the difference since the last
  * tick. */
  virtual void updtime();

  // timer methods
  /** \brief calculate the time that has passed since last being read.
  *
  * This method is overriden by concrete timers to calculate the
  * amount of time that has passed between timer ticks, or buffers of
  * data.
  * \return unit count of time passed since last being read.
  */
  virtual mrs_natural readTimeSrc()=0;
  // virtual mrs_natural getElapsedTimeSinceLastTick()=0;
  /** \brief trigger the timer action.
  *
  * Can be overriden to define a custom action of the timer. Normally
  * trigger simply calls the dispatch method, although overriding
  * trigger may be helpful to define pre or post actions around
  * calling the dispatch method.
  */
  virtual void trigger();
  /** \brief calculate the size of the given time interval.
  *
  * A concrete timer may wish to support units for the time reference
  * it defines. This method will define these units by recognizing
  * them in the string and calculating the interval width
  * appropriately.
  * \param interval a string representation of the interval.
  * \return a count relating to the interval width.
  */
  virtual mrs_natural intervalsize(std::string interval)=0;

  /** \brief set a particular parameter value in the timer.
  * \param cname the control path to set
  * \param value the value to set cname with
  */
  virtual void updtimer(std::string cname, TmControlValue value);

  /** \brief set a particular parameter value in the timer.
  * \param param a parameter to update this timer with
  */
  void updtimer(TmParam& param);

  /** \brief set a number of parameter values in the timer.
  * \param params a vector containing parameters to update the timer with.
  */
  void updtimer(std::vector<TmParam> params);

  /** \brief post an event to be scheduled by the timer
  * \param event_time event dispatch time. Must be meaningful to the concrete timer.
  * \param rep repetition information
  * \param me the event
  */
  void post(std::string event_time, Repeat rep, EvEvent* me);
  /** \brief post an event to be scheduled by the timer with no repetition
  * \param event_time event dispatch time. Must be meaningful to the concrete timer.
  * \param me the event
  */
  void post(std::string event_time, EvEvent* me);
  /** \brief post an event to be scheduled by the timer. The event
  * should have correct time information otherwise dispatch is
  * unpredictable but probably immediate.
  * \param e an event with scheduling information
  */
  void post(EvEvent* e);

  /** \brief determine if an event has become due for dispatch
  * \return true if an event is due
  */
  bool eventPending();

  /** \brief dispatch any events that have become due
  *
  * Events are due for dispatch if their dispatch time is less than or
  * equal to the current time. For each event this method will call
  * the event's dispatch method, check to see if it is to be repeated
  * and, if so, will ensure the necessary bookeeping and repost the
  * event.
  */
  void dispatch();

  // the usual stream IO
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
