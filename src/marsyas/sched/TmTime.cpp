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

#include <marsyas/sched/TmTime.h>

using namespace std;
using namespace Marsyas;

TmTime::TmTime(std::string timer_name, std::string time)
{
  set(timer_name,time);
}

TmTime::~TmTime() {}

void
TmTime::set(std::string tname, std::string time)
{
  time_=time;
  timer_name_=tname;
}

std::string
TmTime::getTime()
{
  return time_;
}


std::string
TmTime::getTimeName()
{
  return timer_name_;
}

void
TmTime::setTime(std::string t)
{
  time_ = t;
}

void
TmTime::setTimeName(std::string t)
{
  timer_name_ = t;
}

