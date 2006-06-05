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

*/

#include "Scheduler.h"
using namespace std;

// include marsystem here to prevent circular dependencies
// and allow calling calling of get in samples from parent 
#include "MarSystem.h"
#include "TmGetTime.h"


Scheduler::Scheduler() {
    name_ = "SchedulerInterface";
    type_ = "Scheduler";
    timer=NULL;
    setTimer(new TmGetTime(this));
}
Scheduler::Scheduler(TmTimer* t) {
    name_ = "SchedulerInterface";
    type_ = "Scheduler";
    timer=NULL;
    setTimer(t->clone());
}
Scheduler::Scheduler(string name) {
    type_ = "Scheduler";
    name_ = name;
    timer=NULL;
    setTimer(new TmGetTime(this));
}
Scheduler::Scheduler(const Scheduler& s) {
    timer=NULL;
    setTimer(s.timer->clone());
    setName(s.name_);
}
Scheduler::~Scheduler() {
    if (timer != NULL) { delete(timer); }
}
void Scheduler::copy(const Scheduler& s) {
    setTimer(s.timer->clone());
    setName(s.name_);
}

void Scheduler::setName(string name) { name_ = name; }
string Scheduler::getType() { return type_; }
string Scheduler::getName() { return name_; }
string Scheduler::getTimerName() { return timer->getName(); }
string Scheduler::getPrefix() {
    string prefix = type_ + "/" + name_ + "/";
    return prefix;
}
void Scheduler::setTimer(TmTimer* c) {
    if (timer != NULL) { delete(timer); }
    timer=c;
    timer->setScheduler(this);
}

void Scheduler::tick() { timer->tick(); }

bool Scheduler::eventPending() {
    return (!pq.empty() && pq.top()->getTime()<timer->getTime());
}

void Scheduler::dispatch() {
    while (eventPending()) {
// This is the old way when using the STL priority_queue
//        ScheduledEvent* se = pq.top();
//        pq.pop(); // remove the first element
//        se.getEvent()->dispatch();
//        if (se.repeat()) {
//           se.doRepeat();
//            post(se);
//        }
        ScheduledEvent* se = pq.pop();
        se->getEvent()->dispatch();

        if (se->repeat()) {
            se->doRepeat();
            post(se);
        } else {
            delete(se);
        }
    }
}

void Scheduler::post(string event_time, Repeat rep, MarEvent* me) {
    rep.count--;
    // should probably check if rep.count==0
    natural stime = timer->getTime() + timer->intervalsize(event_time);
    post(new ScheduledEvent(stime,rep,me));
}

void Scheduler::post(string event_time, MarEvent* me) {
    natural stime = timer->getTime() + timer->intervalsize(event_time);
    post(new ScheduledEvent(stime,Repeat("", 0),me));
}
void
Scheduler::post(ScheduledEvent* e) {
    e->setTimer(timer);
    pq.push(e);
}

//ostream&
//operator<< (ostream& o, Scheduler& sys) {
////    sys.put(o);
//    return o;
//}

