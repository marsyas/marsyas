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

#include <marsyas/sched/TmTimer.h>
#include "../common_source.h"

using namespace std;
using namespace Marsyas;
/*
TmTimer::TmTimer()
{
    init();
}
TmTimer::TmTimer(mrs_string name)
{
    init();
    name_=name;
}
*/
TmTimer::TmTimer(std::string type, std::string name)
{
  type_=type;
  name_=name;
  cur_time_=0;
  init();
}
TmTimer::TmTimer(const TmTimer& t)
{
  name_=t.name_;
//    granularity_=t.granularity_;
  type_=t.type_;		// Type of MarSystem
  name_=t.name_;		// Name of instance
//    next_trigger_=t.next_trigger_;
  cur_time_=t.cur_time_;
//    scheduler=t.scheduler;
}

TmTimer::~TmTimer() { }

void
TmTimer::init()
{
//    cur_time_=0;
//    granularity_=0;
//    next_trigger_=0;
}

mrs_string
TmTimer::getName()
{
  return name_;
}

mrs_string
TmTimer::getType()
{
  return type_;
}

mrs_string
TmTimer::getPrefix()
{
  return type_ + "/" + name_;
}

mrs_natural
TmTimer::getTime()
{
  return cur_time_;
}

void
TmTimer::tick()
{
  updtime();
  trigger();
}

void
TmTimer::updtime()
{
  // now update the time for the end of this time slot
  mrs_natural adj_time = readTimeSrc();
  if (adj_time<1)
    return;
  cur_time_ += adj_time;
}

void TmTimer::trigger()
{
  dispatch();
}

void
TmTimer::updtimer(std::string cname, TmControlValue value)
{
  (void)cname; // FIXME These values are unused
  (void)value;
  std::string x = "TmTimer::updtimer(\""+cname+"\","+value.getSType()+")  updtimer not supported for this timer";
  MRSWARN(x);
}

void
TmTimer::updtimer(TmParam& param)
{
  updtimer(param.cname(),param.value());
}

void
TmTimer::updtimer(std::vector<TmParam> params)
{
  vector<TmParam>::const_iterator tvi;
  for(tvi=params.begin(); tvi!=params.end(); ++tvi) {
    TmParam p = *tvi;
    updtimer(p);
  }
  MRSWARN("TmTimer::updtimer(mrs_string,TmControlValue)  updtimer not supported for this timer");
}

void
TmTimer::post(std::string event_time, Repeat rep, EvEvent* ev)
{
  rep--;
  // should probably check if rep.count==0
  mrs_natural stime = getTime() + intervalsize(event_time);
  ev->setTime(stime);
  ev->setRepeat(rep);
  post(ev);
}

void
TmTimer::post(std::string event_time, EvEvent* ev)
{
  mrs_natural stime = getTime() + intervalsize(event_time);
  ev->setTime(stime);
  ev->setRepeat(Repeat());
  post(ev);
}

void
TmTimer::post(EvEvent* ev)
{
  ev->setTimer(this); // for EvEpr type events that want to read the timer
  // add pointer to map
  events_[ev->getPrefix()] = ev;
  // add to heap
  pq_.push(ev);
}

bool
TmTimer::eventPending()
{
  return (!pq_.empty() && pq_.top()->getTime()<getTime());
}

void
TmTimer::dispatch()
{ //if (getPrefix()=="TmVirtualTime/neil") cout << getTime() << endl;
  while (eventPending()) {
    // dispatch
    EvEvent* ev = pq_.pop();
    ev->dispatch();
    // handle repetition
    if (ev->repeat()) {
      ev->doRepeat();
      post(ev);
    }
    else {
      // delete handle to event
      events_iter_ = events_.find(ev->getPrefix());
      if (events_iter_ != events_.end()) {
        events_.erase(events_iter_);
      }
      delete(ev);
    }
  }
}


/*
ostream&
Marsyas::operator<< (ostream& o, Scheduler& sys) {
//    sys.put(o);

    return o;
}
*/
