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
#include "TmTimer.h"

namespace Marsyas
{
/**
   \class Scheduler
	\ingroup Scheduler
   \brief Scheduler schedules things

   Scheduled event contains a control name, and a
          MarControlValue along with a sample time that
          the name and value should be issued at.

*/


class ScheduledEvent {
protected:
    mrs_natural time_;
    MarEvent* event_;
    Repeat repetition;
    TmTimer* timer_;

public:
    // Constructors
    ScheduledEvent();
    ScheduledEvent(mrs_natural time, MarEvent* event);
    ScheduledEvent(mrs_natural time, Repeat rep, MarEvent* event);
    ScheduledEvent(const ScheduledEvent& s);
    virtual ~ScheduledEvent();

    void setEvent(mrs_natural time, MarEvent* event);
    void setEvent(mrs_natural time, Repeat rep, MarEvent* event);
    void setTimer(TmTimer* t);

    friend class ScheduledEventOrdering;
    // Naming methods
    mrs_natural    getTime() const;
    void setTime(mrs_natural t) { time_=t; }

    MarEvent* getEvent();
    mrs_natural getRepetitionCount();
    std::string getRepetitionInterval();
    Repeat getRepetition();

    bool repeat();
    void doRepeat();

//    int operator=(ScheduledEvent& a);
    ScheduledEvent& operator=(ScheduledEvent& a);
    bool operator<(const ScheduledEvent&) const;
    bool operator>(const ScheduledEvent&) const;

    // the usual stream IO 
    friend std::ostream& operator<<(std::ostream&, ScheduledEvent&);
    friend std::istream& operator>>(std::istream&, ScheduledEvent&);
};
class ScheduledEventComparator {
public:
    bool operator()(ScheduledEvent* a, ScheduledEvent* b) {
        return (a->getTime()) < (b->getTime());
    }
};
/*
class ScheduledEventDestructor {
public:
    void operator()(ScheduledEvent* a) { delete(a); }
};
*/
/*
// comparison class for the priority queue
class ScheduledEventOrdering {
public:
    int operator()(ScheduledEvent& x, ScheduledEvent& y) {
        return x.getTime() > y.getTime();
    }
};
*/

}//namespace Marsyas

#endif
