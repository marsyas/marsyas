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
   \class TmSampleCount
   \brief TmSampleCount reads the insamples information to advance the timer
*/
#include "TmSampleCount.h"
#include "MarSystem.h"
#include "Scheduler.h"

using namespace std;
using namespace Marsyas;

TmSampleCount::TmSampleCount() { name_="Virtual"; init(); }
TmSampleCount::TmSampleCount(string name) { name_=name; init(); }
TmSampleCount::TmSampleCount(MarSystem* ms, string cname) {
    name_="Virtual"; scheduler=NULL;
    read_src_=ms; read_cname_=cname; init();
}
TmSampleCount::TmSampleCount(Scheduler* s, MarSystem* ms, string cname) {
    name_="Virtual"; scheduler=s; read_src_=ms; read_cname_=cname; init();
}
TmSampleCount::TmSampleCount(const TmSampleCount& s) {
    setScheduler(s.scheduler);
    setReadCtrl(s.read_src_,s.read_cname_);
    setName(s.name_);
}
TmSampleCount::~TmSampleCount(){ }

TmTimer* TmSampleCount::clone() { return new TmSampleCount(*this); }

void TmSampleCount::setScheduler(Scheduler* s) { scheduler=s; }
void TmSampleCount::setReadCtrl(MarSystem* ms, string cname) {
    read_src_=ms; read_cname_=cname;
}
mrs_natural TmSampleCount::readTimeSrc() {
    return (read_src_->getctrl(read_cname_))->toNatural();
}
void TmSampleCount::trigger() {
    scheduler->dispatch();
}
mrs_natural TmSampleCount::intervalsize(string interval) {
    return (read_src_==NULL) ? 0 :
        time2samples(interval,read_src_->getctrl("mrs_real/israte")->toReal());
}

