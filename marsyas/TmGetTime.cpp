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
   \class TmGetTime
	\ingroup none
   \brief TmGetTime reads the getlocaltime() function, system clock to get the
          current number of microseconds
   \author inb@cs.uvic.ca
*/
#include "TmGetTime.h"
#include "Scheduler.h"
#include "Conversions.h" // time2usecs

using namespace std;
using namespace Marsyas;

TmGetTime::TmGetTime() { name_="System"; init(); last_usecs=readTimeSrc(); }
TmGetTime::TmGetTime(string name) { name_=name; init(); last_usecs=readTimeSrc(); }
TmGetTime::TmGetTime(Scheduler* s) { name_="System"; setScheduler(s); init(); last_usecs=readTimeSrc(); }
TmGetTime::TmGetTime(const TmGetTime& t) { name_=t.name_; scheduler=t.scheduler; }
TmGetTime::~TmGetTime(){ }
TmTimer* TmGetTime::clone() { return new TmGetTime(*this); }

void TmGetTime::setScheduler(Scheduler* s) { scheduler=s; }

mrs_natural TmGetTime::readTimeSrc() {
/*  struct timeval {
     int tv_sec; //seconds
     int tv_usec; //microseconds
    }; */
#ifndef WIN32
    struct timeval tv; struct timezone tz;
    gettimeofday(&tv, &tz);
    int read_usecs = tv.tv_usec;
    int u = read_usecs - last_usecs;
    if (u<0) { u = 1000000 + u; }
    last_usecs = read_usecs;
#else 
    int u = 0;
#endif 
    return u;
}
void TmGetTime::trigger() { scheduler->dispatch(); }

mrs_natural TmGetTime::intervalsize(string interval) {
    return time2usecs(interval);
}

