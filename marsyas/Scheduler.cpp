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

// include marsystem here to prevent circular dependencies
// and allow calling calling of get in samples from parent 
#include "MarSystem.h"
#include "TmGetTime.h"
///A NEXT LINE ADDED
#include "SchedulerManager.h"
using namespace std;
using namespace Marsyas;


Scheduler::Scheduler()
{
    timer=NULL;
    setTimer(new TmSampleCount());
}
Scheduler::Scheduler(TmTimer* t)
{
    timer=NULL;
    setTimer(t->clone());
}
Scheduler::Scheduler(std::string class_name, std::string identifier)
{
    timer = NULL;
    setTimer(SchedulerManager::makeTimer(class_name,identifier));
}
Scheduler::Scheduler(const Scheduler& s)
{
    timer=NULL;
    setTimer(s.timer->clone());
}
Scheduler::~Scheduler()
{
    if (timer != NULL) { delete(timer); }
}
void Scheduler::copy(const Scheduler& s)
{
    timer=NULL;
    setTimer(s.timer->clone());
}
string Scheduler::getType() { return type_; }
string Scheduler::getName() { return name_; }
//string Scheduler::getTimerName() { return timer->getName(); }
string Scheduler::getPrefix() { return type_ + "/" + name_; }

void Scheduler::setTimer(TmTimer* c)
{
    if (c==NULL) {
        MRSWARN("Scheduler::setTimer(TmTimer*)  refusing to set timer to NULL");
    } else {
        if (timer != NULL) { delete(timer); }
        timer=c;
        timer->setScheduler(this);
        name_=timer->getName();
        type_=timer->getType();
    }
}
void
Scheduler::updtimer(std::string cname, TmControlValue value)
{
    string head="";
    string tail="";
    VScheduler::split_cname(cname,&head,&tail);
    if (tail=="") timer->updtimer(cname,value);
    else {
        events_iter_ = events_.find(head);
        if (events_iter_ != events_.end()) {
            ScheduledEvent* se = events_[head];
            MarEvent* me = se->getEvent();
            me->updctrl(tail,value);
        }
    }
}

void Scheduler::tick() { timer->tick(); }

bool Scheduler::eventPending()
{
    return (!pq.empty() && pq.top()->getTime()<timer->getTime());
}

void Scheduler::dispatch()
{
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
            // delete handle to event
            MarEvent* m = se->getEvent();
            events_iter_ = events_.find(m->getPrefix());
            if (events_iter_ != events_.end()) { events_.erase(events_iter_); }
            // delete event
            delete(se);
        }
    }
}

void Scheduler::post(string event_time, Repeat rep, MarEvent* me)
{
    rep.count--;
    // should probably check if rep.count==0
    mrs_natural stime = timer->getTime() + timer->intervalsize(event_time);
    post(new ScheduledEvent(stime,rep,me));
}
void Scheduler::post(string event_time, MarEvent* me)
{
    mrs_natural stime = timer->getTime() + timer->intervalsize(event_time);
    post(new ScheduledEvent(stime,Repeat("", 0),me));
}
void
Scheduler::post(ScheduledEvent* e)
{
    e->setTimer(timer);
    // add pointer to map
    MarEvent* m = e->getEvent();
    events_[m->getPrefix()] = e;
    // add to heap
    pq.push(e);
}
mrs_natural Scheduler::getTime() { return timer->getTime(); }

//ostream&
//Marsyas::operator<< (ostream& o, Scheduler& sys) {
////    sys.put(o);
//    return o;
//}

