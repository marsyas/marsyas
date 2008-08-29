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

#ifndef MARSYAS_TM_GETTIME_H
#define MARSYAS_TM_GETTIME_H

#include "MarControlValue.h"
#include "TmTimer.h"

#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif 

namespace Marsyas
{
/**
   \class TmGetTime
	\ingroup Scheduler
   \brief TmGetTime reads the getlocaltime() function, system clock to get the
          current number of microseconds
   \author inb@cs.uvic.ca
*/

class Scheduler; // forward declaration

class TmGetTime : public TmTimer {
protected:
//    Scheduler* scheduler;
    int last_usecs;

public:
    // Constructors 
    TmGetTime();
    TmGetTime(std::string name);
//    TmGetTime(Scheduler* s);
    TmGetTime(const TmGetTime& t);
    ~TmGetTime();
    TmTimer* clone();

//    void setScheduler(Scheduler* s);
    mrs_natural readTimeSrc();
    void trigger();
    mrs_natural intervalsize(std::string interval);
};

}//namespace Marsyas

#endif
