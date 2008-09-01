/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "EvValUpd.h"
#include "MarSystem.h"

using namespace std;
using namespace Marsyas;

EvValUpd::EvValUpd(string cname, MarControlPtr val) : MarEvent("EvValUpd","vu")
{
	set(NULL,cname,val);
}

EvValUpd::EvValUpd(MarSystem* ms, string cname, MarControlPtr val) : MarEvent("EvValUpd","vu")
{
	set(ms,cname,val);
}

EvValUpd::EvValUpd(EvValUpd& e) : MarEvent("EvValUpd","vu")
{
	set(e.target_,e.cname_,e.value_);
}

EvValUpd::~EvValUpd() { }

void
EvValUpd::set(MarSystem* ms, string cname, MarControlPtr value)
{
	target_=ms;
	cname_=cname;
	value_=value;
}

void
EvValUpd::dispatch()
{
	if (target_ !=NULL) {
		target_->updctrl(cname_,value_);
	}
}

EvValUpd*
EvValUpd::clone()
{
	return new EvValUpd(*this);
}

void
EvValUpd::updctrl(std::string cname, TmControlValue value)
{
	if (checkupd(cname,"mrs_string/control",value,tmcv_string)) { setCName(value.toString()); }
	else if (checkupd(cname,"MarSystem/target",value,tmcv_marsystem)) { setTarget(value.toMarSystem()); }
// Note that setValue(type) depends on the setting of mrs_string/control
//    else if (checktype(cname,"MarSystem/target",value,mar_marsystem)) { setValue(value.toMarSystem()); }
}

/* these have been moved to the header file
string EvValUpd::getCName() const { return cname_; }
MarControlPtr EvValUpd::getValue() const { return value_; }
MarSystem* EvValUpd::getTarget() const { return target_; }

void EvValUpd::setCName(string cname) { cname_=cname; }
void EvValUpd::setValue(MarControlPtr value) { value_=value; }
void EvValUpd::setTarget(MarSystem* ms) { target_=ms; }
*/
/*
ostream& Marsyas::operator<< (ostream& o, EvValUpd& e) {
//    sys.put(o);
    o << "EvValUpd<" << e.getCName() << "," << e.getValue() << ">";
    return o;
}
*/

