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

#ifndef MARSYAS_TM_TIME_H
#define MARSYAS_TM_TIME_H
 
#include <string>
#include "common.h"

namespace Marsyas
{
/**
   \class TmTime
	\ingroup Scheduler
   \brief TmTime
   \author inb@cs.uvic.ca
*/


class TmTime {
protected:
    std::string time_;
    std::string timer_name_;

public:
    // Constructors 
    TmTime(std::string timer_name, std::string time);
    virtual ~TmTime();

    virtual std::string getTime();
    virtual std::string getTimeName();
    virtual void setTime(std::string t);
    virtual void setTimeName(std::string t);
    virtual void set(std::string timer_name, std::string time);

    // the usual stream IO 
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
