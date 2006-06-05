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
   \class Repeat
   \brief Repeat encapsulates event repetition information

*/

#ifndef MARSYAS_REPEAT_H
#define MARSYAS_REPEAT_H

#include "common.h"
#include "Conversions.h"

#include <string>


class Repeat {
public:
    bool infinite;
    std::string interval;
    natural count;

public:
    Repeat(); // no repetition
    Repeat(std::string time_interval); // infinite
    Repeat(std::string time_interval, natural rep_count);

    void set(bool inf, std::string time_interval, natural rep_count);
    natural interval2samples(real srate);

    void operator++() { ++count; }
    void operator++(int) { count++; };
    void operator--() { --count; }
    void operator--(int) { count--; };
    // the usual stream IO 
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

#endif
