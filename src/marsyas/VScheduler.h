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

#ifndef MARSYAS_VSCHEDULER_H
#define MARSYAS_VSCHEDULER_H

/*
#include "realvec.h"
#include "common.h"
*/
#include "ScheduledEvent.h"
#include "TmTime.h"
#include "Scheduler.h"
#include "TmControlValue.h"
#include <string> 
#include <cstdlib> 

namespace Marsyas
{
/**
   \class Scheduler
	\ingroup none
   \brief Scheduler schedules things
   \author inb@cs.uvic.ca
*/


class VScheduler {
protected:

    Scheduler** schedulers;
    int schedulers_count;
//    map<std::string,vector<VScheduler> > schedulers;

public:
//  map<std::string,vector<std::string> > synonyms_;

    // Constructors 
    VScheduler();
    virtual ~VScheduler();

    // Naming methods 
    void   setName(std::string name);
    std::string getType();
    std::string getName();
    std::string getPrefix();

    void tick();
    bool eventPending();

    void addTimer(TmTimer* t);
    void addTimer(std::string class_name, std::string identifier);
    void updtimer(std::string cname, TmControlValue value);
    static void split_cname(std::string cname, std::string* head, std::string* tail);
    bool removeTimer(std::string name);
    void removeAll();
private:
    void appendScheduler(Scheduler* s);
    Scheduler* findScheduler(std::string name);
public:
    // post to default timer
    void post(std::string event_time, Repeat rep, MarEvent* me);
    void post(std::string event_time, MarEvent* me);

    // post to user defined timer
    void post(TmTime t, Repeat r, MarEvent* me);
    void post(std::string time, std::string timer_name, Repeat r, MarEvent* me);

    mrs_natural getTime(std::string timer);

    // the usual stream IO 
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
