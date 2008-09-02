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


#include "TmGetTime.h"
#include "Scheduler.h"
#include "Conversions.h" // time2usecs

using namespace std;
using namespace Marsyas;

TmGetTime::TmGetTime() : TmTimer("TmGetTime","System")
{
	last_usecs_=readTimeSrc();
}

TmGetTime::TmGetTime(std::string name) : TmTimer("TmGetTime",name)
{
	last_usecs_=readTimeSrc();
}

TmGetTime::TmGetTime(const TmGetTime& t) : TmTimer(t)
{
	name_=t.name_;
}

TmGetTime::~TmGetTime(){ }

mrs_natural
TmGetTime::readTimeSrc()
{
/*  struct timeval {
     int tv_sec; //seconds
     int tv_usec; //microseconds
    }; */
#ifndef WIN32
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	int read_usecs = tv.tv_usec;
	int u = read_usecs - last_usecs_;
	if (u<0) { u = 1000000 + u; }
	last_usecs_ = read_usecs;
#else 
	int u = 0;
#endif 
	return u;
}

mrs_natural
TmGetTime::intervalsize(std::string interval)
{
	return time2usecs(interval);
}
