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


#include "TmTimer.h"

using namespace std;
using namespace Marsyas;

TmTimer::TmTimer()
{
    init();
}
TmTimer::TmTimer(string name)
{
    init();
    name_=name;
}
TmTimer::TmTimer(string type, string name)
{
    init();
    type_=type;
    name_=name;
}
TmTimer::TmTimer(const TmTimer& t)
{
    name_=t.name_;
    granularity_=t.granularity_;
    type_=t.type_;		// Type of MarSystem
    name_=t.name_;		// Name of instance
    next_trigger_=t.next_trigger_;
    cur_time_=t.cur_time_;
    scheduler=t.scheduler;
}

TmTimer::~TmTimer() { }

void
TmTimer::init() { cur_time_=0; granularity_=0; next_trigger_=0; scheduler=NULL; }

string
TmTimer::getName() { return name_; }

void
TmTimer::setName(string name) { name_=name; }

string
TmTimer::getType() { return type_; }

string
TmTimer::getPrefix() { return type_ + "/" + name_; }

void
TmTimer::setGranularity(mrs_natural g) { granularity_=g; }

void
TmTimer::setScheduler(Scheduler* s) { scheduler=s; }

mrs_natural 
TmTimer::getTime() { return cur_time_; }

/* this is a very simple implementation of granularity. What if we want it to
   be every 1 second of real time. Could be done by reading a control for
   a specific value. What if the size of the sample buffer changes during
   processing. Might have to have a size relative to certain parameters like
   sample buffer size. So many questions. */
void TmTimer::tick()
{
    mrs_natural adj_time = readTimeSrc();
    if (adj_time<1) return;
    cur_time_ += adj_time;

    if (next_trigger_<1) {
        if (granularity_>0) { next_trigger_=granularity_; }
        trigger();
    } else { next_trigger_ -= adj_time; }
}
void
TmTimer::updtimer(std::string cname, TmControlValue value)
{
    MRSWARN("TmTimer::updtimer(string,TmControlValue)  updtimer not supported for this timer");
}

/*
ostream&
Marsyas::operator<< (ostream& o, Scheduler& sys) {
//    sys.put(o);
    
    return o;
}
*/
