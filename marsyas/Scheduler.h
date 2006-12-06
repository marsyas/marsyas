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

/**
   \class Scheduler
   \brief Scheduler schedules things
   Modified: Neil Burroughs, Dec 4, 2006

*/

#ifndef MARSYAS_SCHEDULER_H
#define MARSYAS_SCHEDULER_H

/*
#include "realvec.h"
#include "common.h"
*/
#include "ScheduledEvent.h"
#include "MarEvent.h"
#include "TmTimer.h"
#include "TmControlValue.h"
#include <string>
//#include <queue>
//#include <functional> // for priority queue comparator
#include <iostream>


#include "Heap.h"
#include <map> 

namespace Marsyas
{

class Scheduler {
protected:
    std::string type_;		// Type of MarSystem
    std::string name_;		// Name of instance

//    priority_queue<ScheduledEvent, vector<ScheduledEvent>, greater<ScheduledEvent> > pq; // this is supposed to be equivalent to ^^^
    Heap<ScheduledEvent, ScheduledEventComparator> pq;
    TmTimer* timer;
    // map for events to allow modifying events while in the heap
    std::map<std::string, ScheduledEvent*> events_;
    std::map<std::string, ScheduledEvent*>::iterator events_iter_;

public:
    // Constructors 
    Scheduler();
    Scheduler(std::string class_name, std::string identifier);
    Scheduler(TmTimer* t);
    Scheduler(const Scheduler& s);
    virtual ~Scheduler();
    Scheduler clone() const;
    void copy(const Scheduler& s);
    void dispatch();

    // Naming methods 
    std::string getType();
    std::string getName();
    std::string getPrefix();

    void setTimer(TmTimer* c);
    void updtimer(std::string cname, TmControlValue value);
    void tick();
    void addScheduler(std::string name);
    bool eventPending();
    mrs_natural getTime();

    void post(std::string event_time, Repeat rep, MarEvent* me);
    void post(std::string event_time, MarEvent* me);
    void post(ScheduledEvent* e);

    // the usual stream IO 
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
