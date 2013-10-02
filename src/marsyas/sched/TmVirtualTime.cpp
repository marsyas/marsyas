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

#include <marsyas/sched/TmVirtualTime.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/system/MarSystem.h>

using namespace std;
using namespace Marsyas;

TmVirtualTime::TmVirtualTime() : TmTimer("TmVirtualTime","Virtual")
{
  setSource(NULL);
}

TmVirtualTime::TmVirtualTime(std::string name) : TmTimer("TmVirtualTime",name)
{
  setSource(NULL);
}

TmVirtualTime::TmVirtualTime(std::string name, MarSystem* ms) : TmTimer("TmVirtualTime",name)
{
  setSource(ms);
}

TmVirtualTime::TmVirtualTime(const TmVirtualTime& s) : TmTimer(s)
{
  setSource(s.read_src_);
}

TmVirtualTime::~TmVirtualTime() {}

void
TmVirtualTime::setSource(MarSystem* ms)
{
  // start at 0
  previous_tick_interval_=0;
  read_src_=ms;
  error_term_=0.0;
  if(read_src_!=NULL) {
    nsamples_=read_src_->getctrl("mrs_natural/onSamples");
    srate_=read_src_->getctrl("mrs_real/osrate");
  }
}

mrs_natural
TmVirtualTime::readTimeSrc()
{
  if (read_src_==NULL) {
    MRSWARN("TmVirtualTime::readTimeSrc()  time source is not defined.");
    return 0;
  }
  // this is the width of the last tick - elapsed time since last tick
  mrs_natural ret = previous_tick_interval_;
  mrs_real srate = srate_->to<mrs_real>();
  if (srate<1.0)
    return 0;
  mrs_real interval_width = nsamples_->to<mrs_natural>() / srate;
  mrs_real microseconds = (interval_width * 1000000.0) + error_term_;
  error_term_ = microseconds - ((long)microseconds);
//	mrs_natural count = (mrs_natural)microseconds;
//	cout << "us=" << ((mrs_natural)microseconds) << endl;
  previous_tick_interval_ = (mrs_natural)microseconds;
  return ret;
}

mrs_natural
TmVirtualTime::intervalsize(std::string interval)
{
  return time2usecs(interval);
//	return (read_src_==NULL) ? 0 :
//		time2samples(interval,read_src_->getctrl("mrs_real/osrate")->to<mrs_real>());
}

void
TmVirtualTime::updtimer(std::string cname, TmControlValue value)
{
  bool type_error=false;
  if (cname=="MarSystem/source") {
    if (value.getType()==tmcv_marsystem) {
      setSource(value.toMarSystem());
    }
    else type_error=true;
  }
  else {
    MRSWARN("TmVirtualTime::updtimer(string,TmControlValue)  unsupported control");
  }
  if (type_error) {
    MRSWARN("TmVirtualTime::updtimer(string,TmControlValue)  wrong type to "+cname);
  }
}

