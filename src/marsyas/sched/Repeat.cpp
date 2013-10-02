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


#include <marsyas/sched/Repeat.h>

using namespace std;
using namespace Marsyas;

Repeat::Repeat()
{
  set(false,"",0);
}

Repeat::Repeat(mrs_string time_interval)
{
  set(true,time_interval,0);
}

Repeat::Repeat(mrs_string time_interval, mrs_natural rep_count)
{
  set(false,time_interval,rep_count);
}

bool
Repeat::repeat()
{
  return infinite_ || count_>0;
}

Repeat::~Repeat() { }

void
Repeat::set(bool inf, mrs_string time_interval, mrs_natural rep_count)
{
  infinite_=inf;
  interval_=time_interval;
  count_=rep_count;
}

mrs_natural
Repeat::interval2samples(mrs_real srate)
{
  return time2samples(interval_,srate);
}
