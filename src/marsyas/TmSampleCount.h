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

#ifndef MARSYAS_TM_SAMPLECOUNT_H
#define MARSYAS_TM_SAMPLECOUNT_H

#include "TmTimer.h"
#include "MarControlValue.h"
#include "MarSystem.h" 

namespace Marsyas
{
/**
   \class TmSampleCount
	\ingroup Scheduler
   \brief TmSampleCount reads the insamples information to advance the timer
   \author inb@cs.uvic.ca
*/

// forward declaration of MarSystem allows Scheduler.getctrl("insamples")
// for scheduler count
class MarSystem; // forward declaration
class Scheduler; // forward declaration

class TmSampleCount : public TmTimer {
protected:
    MarSystem* read_src_;
    MarControlPtr read_ctrl_;
    std::string read_cname_;

public:
    // Constructors 
    TmSampleCount();
    TmSampleCount(std::string name);
    TmSampleCount(MarSystem*, std::string cname);
//    TmSampleCount(Scheduler*, MarSystem*, std::string cname);
    TmSampleCount(const TmSampleCount& s);
    virtual ~TmSampleCount();
    TmTimer* clone();

    void setReadCtrl(MarSystem* ms, std::string cname); // where to read time info
    void setSource(MarSystem* ms);
    void setSourceCtrl(std::string cname);
    mrs_natural readTimeSrc();
    void trigger();
    mrs_natural intervalsize(std::string interval);
    virtual void updtimer(std::string cname, TmControlValue value);
};

}//namespace Marsyas

#endif
