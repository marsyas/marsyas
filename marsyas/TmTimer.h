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
   \class Timer
   \brief Abstract Timer

*/

#ifndef MARSYAS_TM_TIMER_H
#define MARSYAS_TM_TIMER_H

//#include "MarControlValue.h"
#include "common.h"
#include "TmControlValue.h"

#include <iostream>
#include <string> 

namespace Marsyas
{

class Scheduler;

class TmTimer {
protected:
    std::string type_;		// Type of MarSystem
    std::string name_;		// Name of instance

    unsigned long cur_time_;

    mrs_natural granularity_;
    mrs_natural next_trigger_;

    Scheduler* scheduler;
protected:
    void init();

public:
    // Constructors 
    TmTimer();
    TmTimer(std::string name);
    TmTimer(std::string type, std::string name);
    TmTimer(const TmTimer&);
    virtual ~TmTimer();

    virtual void setName(std::string name);
    virtual std::string getName();
    virtual std::string getType();
    virtual std::string getPrefix();
    virtual TmTimer* clone()=0;

    virtual void setScheduler(Scheduler* s);
    void setGranularity(mrs_natural g);
    mrs_natural getTime();
    void tick();

    virtual mrs_natural readTimeSrc()=0;
    virtual void trigger()=0;
    virtual mrs_natural intervalsize(std::string interval)=0;
    virtual void updtimer(std::string cname, TmControlValue value);

    // the usual stream IO 
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
