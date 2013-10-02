/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/sched/EvEvent.h>
#include <marsyas/sched/TmTimer.h>
#include <marsyas/sched/EvExpr.h>

using std::ostringstream;
using namespace Marsyas;

EvEvent::EvEvent() { }

EvEvent::EvEvent(std::string t, std::string n)
{
  name_=n;
  type_=t;
}

EvEvent::~EvEvent() { }

std::string
EvEvent::getType() const
{
  return type_;
}

std::string
EvEvent::getName() const
{
  return name_;
}

std::string
EvEvent::getPrefix() const
{
  return type_ + "/" + name_;
}

void
EvEvent::setName(std::string n)
{
  name_=n;
}

mrs_natural
EvEvent::getTime() const
{
  return time_;
}

void
EvEvent::setTime(mrs_natural t)
{
  time_=t;
}

mrs_natural
EvEvent::getRepeatCount()
{
  return repeat_.getCount();
}

std::string
EvEvent::getRepeatInterval()
{
  return repeat_.getInterval();
}

Repeat
EvEvent::getRepeat()
{
  return repeat_;
}

void
EvEvent::setRepeat(Repeat r)
{
  repeat_=r;
}

bool
EvEvent::repeat()
{
  return repeat_.repeat();
}

std::string
EvEvent::repeat_interval(std::string interval)
{
  return interval;
}


/* Note there is a potential problem if time_ is much less than current
   scheduler time, then it may do a bunch of quick events (depending on
   repetition_time_) until it catches up to current time. */
void
EvEvent::doRepeat()
{
  if (repeat()) {
    repeat_--;
    if (timer_ != NULL) {
      std::string t = getRepeatInterval();
      time_ = getTime() + timer_->intervalsize(t);
//            printf("time=%ld  interval=%ld\n",time_,timer_->intervalsize(repetition.interval));
//            time_ = time_ + timer_->intervalsize(repetition.interval);
    }
    else {
      // otherwise kill it
      repeat_.setInfinite(false);
      repeat_.setCount(0);
    }
  }
}

void
EvEvent::setTimer(TmTimer* t)
{
  timer_=t;
}

void
EvEvent::updctrl(std::string cname, TmControlValue value)
{
  (void) cname; (void) value; // FIXME These values are unused
  MRSWARN("EvEvent::updControl(string,TmControlValue)  not supported by this event");
}

bool
EvEvent::checkupd(std::string c1, std::string c2, TmControlValue v, mrs_natural t)
{
  return (c1==c2 && v.getType()==t);
}
/*
ostream& Marsyas::operator<< (ostream& o, EvEvent& e) {
//    sys.put(o);
    o << "EvEvent<" << e.getCName() << "," << e.getValue() << ">";
    return o;
}
*/
