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

#include <marsyas/sched/EvGetUpd.h>
#include <marsyas/system/MarSystem.h>

using std::ostringstream;
using namespace Marsyas;

EvGetUpd::EvGetUpd(MarSystem* src, mrs_string scname, MarSystem* tgt, mrs_string tcname) : EvEvent("EvGetUpd","GetUpd")
{
  setEvent(src,scname,tgt,tcname);
}

EvGetUpd::EvGetUpd(EvGetUpd& e) : EvEvent("EvGetUpd","GetUpd")
{
  setEvent(e.source_,e.src_cname_,e.target_,e.tgt_cname_);
}
EvGetUpd::~EvGetUpd() { }

void
EvGetUpd::setEvent(MarSystem* src, mrs_string scname, MarSystem* tgt, mrs_string tcname)
{
  src_cname_=scname;
  tgt_cname_=tcname;
  source_=src;
  target_=tgt;
}

void
EvGetUpd::dispatch()
{
  if (target_!=NULL && source_!=NULL) {
    target_->updControl(tgt_cname_,source_->getctrl(src_cname_));
  }
}

EvGetUpd*
EvGetUpd::clone()
{
  return new EvGetUpd(*this);
}

/* these have been moved to the header file
mrs_string EvGetUpd::getSrcCName() const { return tgt_cname_; }
mrs_string EvGetUpd::getTgtCName() const { return src_cname_; }
MarSystem* EvGetUpd::getSource() const { return source_; }
MarSystem* EvGetUpd::getTarget() const { return target_; }

void EvGetUpd::setSrcCName(mrs_string cname) { src_cname_=cname; }
void EvGetUpd::setTgtCName(mrs_string cname) { tgt_cname_=cname; }
void EvGetUpd::setSource(MarSystem* ms) { source_=ms; }
void EvGetUpd::setTarget(MarSystem* ms) { target_=ms; }
*/

/*
ostream& Marsyas::operator<< (ostream& o, EvValUpd& e) {
//    sys.put(o);
    o << "EvValUpd<" << e.getCName() << "," << e.getValue() << ">";
    return o;
}
*/
