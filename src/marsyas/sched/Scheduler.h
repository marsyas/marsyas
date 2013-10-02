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

#ifndef MARSYAS_SCHEDULER_H
#define MARSYAS_SCHEDULER_H

#include <marsyas/sched/TmTimer.h>
#include <marsyas/sched/TmTime.h>
#include <marsyas/sched/TmControlValue.h>
#include <marsyas/sched/TmParam.h>

#include <string>
#include <vector>

namespace Marsyas
{
/**
	\class Scheduler
	\ingroup Scheduler
	\brief Scheduler schedules things
	\author Neil Burroughs  inb@cs.uvic.ca
*/


class Scheduler {
protected:

  TmTimer** timers_;
  int timers_count_;
//    map<std::string,vector<VScheduler> > schedulers;

public:
//  map<std::string,vector<std::string> > synonyms_;

  // Constructors
  Scheduler();
  virtual ~Scheduler();

  // Naming methods
  void setName(std::string name);
  std::string getType();
  std::string getName();
  std::string getPrefix();

  void tick();
  bool eventPending();

  void addTimer(TmTimer* t);
  void addTimer(std::string class_name, std::string identifier);
  void addTimer(std::string class_name, std::string identifier, std::vector<TmParam> params);

  void updtimer(std::string tmr_id, TmControlValue value);
  void updtimer(std::string tmr_id, TmParam& param);
  void updtimer(std::string tmr_id, std::vector<TmParam> params);

  static void split_cname(std::string cname, std::string* head, std::string* tail);
  bool removeTimer(std::string name);
  void removeAll();
private:
  void appendTimer(TmTimer* s);
  TmTimer* findTimer(std::string name);
public:
  // post to default timer
  void post(std::string event_time, Repeat rep, EvEvent* me);
  void post(std::string event_time, EvEvent* me);

  // post to user defined timer
  void post(TmTime t, Repeat r, EvEvent* me);
  void post(std::string time, std::string timer_name, Repeat r, EvEvent* me);

  mrs_natural getTime(std::string timer);

  // the usual stream IO
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
