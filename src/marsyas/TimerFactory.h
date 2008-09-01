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

#ifndef MARSYAS_TIMER_FACTORY_H
#define MARSYAS_TIMER_FACTORY_H

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
    in function TimerFactory::addTimers() add the line
    registerTimer(TmSomeTimerName);
*/

#include "TmTimer.h"
#include "common.h"

#include <string>
#include <map>
#include <vector>
#include "TmParam.h"

using namespace std;

namespace Marsyas
{
/**
	\class TimerFactory
	\ingroup Scheduler
	\brief TimerFactory
	\author Neil Burroughs  inb@cs.uvic.ca
*/

struct MakeTimer {
	MakeTimer() {}
	virtual ~MakeTimer(){}
	virtual TmTimer* make(std::string)=0;
};

class TimerFactory {
	std::map<std::string, MakeTimer*> registry_;
	static TimerFactory* instance_;
	TimerFactory();
	void addTimers();

public:
	static TimerFactory* getInstance();
	virtual ~TimerFactory();
	TmTimer* make(std::string class_name, std::string identifier);
	TmTimer* make(std::string class_name, std::string identifier, std::vector<TmParam> params);
};
// make sure the TimerFactory instance is NULL so the singleton can be created
}

#endif

