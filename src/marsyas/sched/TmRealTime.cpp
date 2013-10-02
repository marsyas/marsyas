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

#include <marsyas/sched/TmRealTime.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/Conversions.h> // time2usecs
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

TmRealTime::TmRealTime() : TmTimer("TmRealTime","System")
{
  last_usecs_=0;
  last_usecs_=readTimeSrc();
}

TmRealTime::TmRealTime(std::string name) : TmTimer("TmRealTime",name)
{
  last_usecs_=0;
  last_usecs_=readTimeSrc();
}

TmRealTime::TmRealTime(const TmRealTime& t) : TmTimer(t)
{
  name_=t.name_;
}

TmRealTime::~TmRealTime() { }

mrs_natural
TmRealTime::getMicroSeconds()
{
  /*  struct timeval {
       int tv_sec; //seconds
       int tv_usec; //microseconds
      }; */
#ifndef WIN32
  struct timeval tv;
  struct timezone tz;
  gettimeofday(&tv, &tz);
  int u = tv.tv_usec;
#else
  int u = 0;
#endif
  return u;
}

mrs_natural
TmRealTime::readTimeSrc()
{
  int read_usecs = getMicroSeconds();
  int u = read_usecs - last_usecs_;
  if (u<0) { u = 1000000 + u; }
  last_usecs_ = read_usecs;
  return u;
}

void
TmRealTime::updtime()
{
  cur_time_ = getMicroSeconds();
}

mrs_natural
TmRealTime::intervalsize(std::string interval)
{
  return time2usecs(interval);
}
