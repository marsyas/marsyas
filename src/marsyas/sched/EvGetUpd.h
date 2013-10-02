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


#ifndef MARSYAS_EV_GETUPD_H
#define MARSYAS_EV_GETUPD_H

#include <marsyas/sched/EvEvent.h>
#include <marsyas/system/MarControl.h>

#include <string>
#include <iostream>

namespace Marsyas
{
/**
	\class EvGetUpd
	\ingroup Scheduler
	\brief get a control value and use it to update another
	\author Neil Burroughs  inb@cs.uvic.ca
*/


class MarSystem; // forward declaration

class EvGetUpd : public EvEvent {
protected:
  MarSystem* source_;
  MarSystem* target_;
  std::string src_cname_;
  std::string tgt_cname_;

public:
  // Constructors
  EvGetUpd(MarSystem* src, std::string scname, MarSystem* tgt, std::string tcname);
  EvGetUpd(EvGetUpd& e);
  virtual ~EvGetUpd();

  // Set/Get methods
  std::string getSrcCName() const {return tgt_cname_;};
  std::string getTgtCName() const {return src_cname_;};
  MarSystem* getSource() const {return source_;};
  MarSystem* getTarget() const {return target_;};

  void setSrcCName(std::string cname) {src_cname_=cname;};
  void setTgtCName(std::string cname) {tgt_cname_=cname;};
  void setSource(MarSystem* ms) {source_=ms;};
  void setTarget(MarSystem* ms) {target_=ms;};
  void setEvent(MarSystem* src, std::string scname, MarSystem* tgt, std::string tcname);

  // Event dispatch
  void dispatch();

  virtual EvGetUpd* clone();

  // the usual stream IO
  friend std::ostream& operator<<(std::ostream&, EvEvent&);
  friend std::istream& operator>>(std::istream&, EvEvent&);
};

}//namespace Marsyas

#endif
