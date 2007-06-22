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
	\ingroup Scheduler
   \brief Scheduler schedules things

*/

#include "ScheduledEvent.h"
#include "MarSystem.h"
#include "EvExpr.h"
using namespace std;
using namespace Marsyas;

ScheduledEvent::ScheduledEvent() { event_=NULL; }

ScheduledEvent::ScheduledEvent(mrs_natural time, MarEvent* event)
{
    setEvent(time,event);
}
ScheduledEvent::ScheduledEvent(mrs_natural time, Repeat rep, MarEvent* event)
{
    setEvent(time,rep,event);
}
ScheduledEvent::ScheduledEvent(const ScheduledEvent& s)
{
    setEvent(s.time_,s.repetition,s.event_->clone());
    setTimer(s.timer_);
}
ScheduledEvent::~ScheduledEvent()
{
    if (event_!=NULL) { delete(event_); event_=NULL; }
}

void ScheduledEvent::setEvent(mrs_natural time, MarEvent* event)
{
    setEvent(time,Repeat(),event);
}
void ScheduledEvent::setEvent(mrs_natural time, Repeat rep, MarEvent* event)
{
    time_=time;
    event_=event;
    repetition=rep;
}
void ScheduledEvent::setTimer(TmTimer* t)
{
    timer_=t;
    if (event_->getType()=="EvExpr") {
        EvExpr* e = dynamic_cast<EvExpr*>(event_);
        if (e!=NULL) {
            Expr* x=e->getExpression();
            if (x!=NULL) {
                x->setTimer(timer_);
                x->post(); // evaluate init expressions
            }
        }
    }
}

mrs_natural ScheduledEvent::getTime() const { return time_; }
MarEvent* ScheduledEvent::getEvent() { return event_; }

mrs_natural ScheduledEvent::getRepetitionCount() { return repetition.count; }
string ScheduledEvent::getRepetitionInterval() { return repetition.interval; }
Repeat ScheduledEvent::getRepetition() { return repetition; }

bool ScheduledEvent::repeat()
{
    return event_->repeat();
}
/* Note there is a potential problem if time_ is much less than current
   scheduler time, then it may do a bunch of quick events (depending on
   repetition_time_) until it catches up to current time. */
void ScheduledEvent::doRepeat()
{
    if (repeat()) {
        repetition--;
        if (timer_ != NULL) {
            std::string t = event_->repeat_interval();
            time_ = timer_->getTime() + timer_->intervalsize(t);
//            printf("time=%ld  interval=%ld\n",time_,timer_->intervalsize(repetition.interval));
//            time_ = time_ + timer_->intervalsize(repetition.interval);
        } else {
            // otherwise kill it
            repetition.infinite=false;
            repetition.count=0;
        }
    }
}
/*
int ScheduledEvent::operator=(ScheduledEvent& a) {
    setEvent(a.time_,a.repetition,a.event_->clone());
    setTimer(a.timer_);
    return 1;
}
*/
ScheduledEvent& ScheduledEvent::operator=(ScheduledEvent& a)
{
    setEvent(a.time_,a.repetition,a.event_->clone());
    setTimer(a.timer_);
    return *this;
}
bool ScheduledEvent::operator<(const ScheduledEvent &se) const
{
    return (getTime() < se.getTime());
}
bool ScheduledEvent::operator>(const ScheduledEvent &se) const
{
    return (getTime() > se.getTime());
}

ostream&
Marsyas::operator<< (ostream& o, ScheduledEvent& sys)
{
//    sys.put(o);
    o << "ScheduledEvent<" << sys.time_ << "," << sys.event_ << ">";
    return o;
}

