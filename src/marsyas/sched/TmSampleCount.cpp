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

#include <marsyas/sched/TmSampleCount.h>
#include <marsyas/sched/Scheduler.h>
#include <marsyas/system/MarSystem.h>

using namespace std;
using namespace Marsyas;

TmSampleCount::TmSampleCount() : TmTimer("TmSampleCount","Virtual")
{
  setReadCtrl(NULL,"mrs_natural/inSamples");
}

TmSampleCount::TmSampleCount(std::string name) : TmTimer("TmSampleCount",name)
{
  setReadCtrl(NULL,"mrs_natural/inSamples");
}

TmSampleCount::TmSampleCount(MarSystem* ms, std::string cname) : TmTimer("TmSampleCount","Virtual")
{
  setReadCtrl(ms,cname);
}

TmSampleCount::TmSampleCount(const TmSampleCount& s) : TmTimer(s)
{
  setReadCtrl(s.read_src_,s.read_cname_);
}

TmSampleCount::~TmSampleCount() { }

void
TmSampleCount::setReadCtrl(MarSystem* ms, std::string cname)
{
  read_src_=ms;
  read_cname_=cname;
  if (read_src_!=NULL)
    read_ctrl_=read_src_->getctrl(cname);
}

void
TmSampleCount::setSource(MarSystem* ms)
{
  read_src_=ms;
  if(read_src_!=NULL && read_cname_!="")
    read_ctrl_=read_src_->getctrl(read_cname_);
//	setReadCtrl(ms,read_cname_);
}

void
TmSampleCount::setSourceCtrl(std::string cname)
{
  read_cname_=cname;
  if(read_src_!=NULL)
    read_ctrl_=read_src_->getctrl(read_cname_);
//	setReadCtrl(read_src_,cname);
}

mrs_natural
TmSampleCount::readTimeSrc()
{
  if (read_src_==NULL) {
    MRSWARN("TmSampleCount::readTimeSrc()  time source is NULL");
    return 0;
  }
  mrs_natural m = read_ctrl_->to<mrs_natural>();
  return m;
//    return (read_src_->getctrl(read_cname_)).toNatural() + getTime();
}

mrs_natural TmSampleCount::intervalsize(std::string interval)
{
  return (read_src_==NULL) ? 0 :
         time2samples(interval,read_src_->getctrl("mrs_real/israte")->to<mrs_real>());
}

void
TmSampleCount::updtimer(std::string cname, TmControlValue value)
{
  bool type_error=false;
  if (cname=="MarSystem/source") {
    if (value.getType()==tmcv_marsystem) {
      setSource(value.toMarSystem());
    }
    else type_error=true;
  }
  else if (cname=="mrs_string/control") {
    if (value.getType()==tmcv_string) {
      setSourceCtrl(value.toString());
    }
    else type_error=true;
  }
  else {
    MRSWARN("TmSampleCount::updtimer(string,TmControlValue)  unsupported control");
  }
  if (type_error) {
    MRSWARN("TmSampleCount::updtimer(string,TmControlValue)  wrong type to "+cname);
  }
}

