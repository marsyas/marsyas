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


#ifndef MARSYAS_MAREVENT_H
#define MARSYAS_MAREVENT_H

#include "Repeat.h"
#include "TmControlValue.h"
#include <iostream>

namespace Marsyas
{
/**
   \class MarEvent
   \ingroup Experimental
   \brief MarEvent 

*/


class MarSystem; // forward declaration

class MarEvent {
protected:
    std::string type_;
    std::string name_;

    Repeat repeat_;

public:
    MarEvent();
    MarEvent(std::string t, std::string n);

    virtual ~MarEvent();

    std::string getType() const { return type_; }
    std::string getName() const { return name_; }
    std::string getPrefix() const { return type_ + "/" + name_; }
    void setName(std::string n) { name_=n; }

    // Event dispatch
    virtual void dispatch()=0;

    // copy must be implemented for copying of ScheduledEvents
    virtual MarEvent* clone()=0;

    virtual bool repeat() { return repeat_.repeat(); };
    virtual void set_repeat(Repeat r) { repeat_=r; }
    virtual std::string repeat_interval(std::string interval) { return interval; };
    virtual std::string repeat_interval() { return repeat_.interval; };

    virtual void updctrl(std::string cname, TmControlValue value);
    bool checkupd(std::string c1, std::string c2, TmControlValue v, mrs_natural t);

    // the usual stream IO 
    friend std::ostream& operator<<(std::ostream&, MarEvent&);
    friend std::istream& operator>>(std::istream&, MarEvent&);
};

}//namespace Marsyas

#endif
