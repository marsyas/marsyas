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
   \class TmTimer
   \brief Abstract TmTimer
*/
#include "TmTimer.h"
using namespace std;


TmTimer::TmTimer() { init(); }
TmTimer::TmTimer(string name) { name_=name; init(); }
TmTimer::~TmTimer(){ }

void TmTimer::init() { cur_time_=0; granularity_=0; next_trigger_=0; }
string TmTimer::getName() { return name_; }
void TmTimer::setName(string name) { name_=name; }

void TmTimer::setGranularity(natural g) { granularity_=g; }

natural TmTimer::getTime() { return cur_time_; }
/* this is a very simple implementation of granularity. What if we want it to
   be every 1 second of real time. Could be done by reading a control for
   a specific value. What if the size of the sample buffer changes during
   processing. Might have to have a size relative to certain parameters like
   sample buffer size. So many questions. */
void TmTimer::tick() {
    natural chunk = readTimeSrc();
    if (next_trigger_<1) {
        if (granularity_>0) { next_trigger_=granularity_; }
        trigger();
    } else { next_trigger_ -= chunk; }
    cur_time_ += chunk;
}

/*
ostream&
operator<< (ostream& o, Scheduler& sys) {
//    sys.put(o);
    
    return o;
}
*/
