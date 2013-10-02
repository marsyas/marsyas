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


#ifndef MARSYAS_EV_VALUPD_H
#define MARSYAS_EV_VALUPD_H

#include <marsyas/sched/EvEvent.h>
#include <marsyas/sched/TmControlValue.h>
#include <marsyas/system/MarControl.h>

#include <string>
#include <iostream>

namespace Marsyas
{
/**
	\class EvValUpd
	\ingroup Scheduler
	\brief update a MarSystem control value
	\author Neil Burroughs  inb@cs.uvic.ca
*/


class MarSystem; // forward declaration

class marsyas_EXPORT EvValUpd : public EvEvent {
protected:
  MarSystem* target_;
  std::string cname_;
  MarControlPtr value_;

public:
  // Constructors
  EvValUpd(std::string cname, MarControlPtr);
  EvValUpd(MarSystem*, std::string cname, MarControlPtr);
  EvValUpd(EvValUpd& e);
  virtual ~EvValUpd();

  // Set/Get methods
  std::string getCName() const {return cname_;};
  MarControlPtr getValue() const {return value_;};
  MarSystem* getTarget() const {return target_;};

  void setCName(std::string cname) {cname_=cname;};
  void setValue(MarControlPtr value) {value_=value;};
  void setTarget(MarSystem* ms) {target_=ms;};
  void set(MarSystem* ms, std::string cname, MarControlPtr);
  void updctrl(std::string cname, TmControlValue value);

  // Event dispatch
  void dispatch();

  virtual EvValUpd* clone();

  // the usual stream IO
  friend std::ostream& operator<<(std::ostream&, EvEvent&);
  friend std::istream& operator>>(std::istream&, EvEvent&);
};

}//namespace Marsyas

#endif
