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

#ifndef MARSYAS_TM_TIMER_MANAGER_H
#define MARSYAS_TM_TIMER_MANAGER_H

/** Adding new Timers:
 New timers are added by first making them...
 Basically, a map is created from "TimerName"=>TimerConstructorObject.
 This makes it possible to use a map for fast access to specific timers and it
 prevents having to instantiate each Timer type. The constructor object simply
 wraps the new operator so that it constructs objects only when requested.
 1. Add the timer's include file.
 2. Wrap the object using the macro:
    TimerCreateWrapper(TmSomeTimerName);
 3. Register the timer in the map:
    in function TmTimerManager::addTimers() add the line
    registerTimer(TmSomeTimerName);
*/

#include <marsyas/sched/TmTimer.h>
#include <marsyas/sched/TmParam.h>
#include <marsyas/common_header.h>

#include <string>
#include <map>
#include <vector>

//using namespace std;

namespace Marsyas
{
/**
	\class TmTimerManager
	\ingroup Scheduler
	\brief TmTimerManager
	\author Neil Burroughs  inb@cs.uvic.ca
*/

struct MakeTimer {
  MakeTimer() {}
  virtual ~MakeTimer() {}
  virtual TmTimer* make(std::string)=0;
};

class TmTimerManager {
  std::map<std::string, MakeTimer*> registry_;
  static TmTimerManager* instance_;
  TmTimerManager();
  void addTimers();

public:
  static TmTimerManager* getInstance();
  virtual ~TmTimerManager();
  TmTimer* make(std::string class_name, std::string identifier);
  TmTimer* make(std::string class_name, std::string identifier, std::vector<TmParam> params);
};
// make sure the TmTimerManager instance is NULL so the singleton can be created
}

#endif

