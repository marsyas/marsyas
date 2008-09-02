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
	\brief TmGetTime reads the getlocaltime() function, system clock to get the current number of microseconds
	\author Neil Burroughs  inb@cs.uvic.ca
*/

class TmGetTime : public TmTimer {
protected:
	/** \brief last read count of microseconds. */
	int last_usecs_;

public:
	/** \brief empty constructor. Given the default name: System as in "TmGetTime/System"
	*/
	TmGetTime();
	/** \brief named constructor. Given the identifier "TmGetTime/name"
	* \param name a unique name to call this timer
	*/
	TmGetTime(std::string name);
	/** \brief copy constructor
	* \param t timer to copy
	*/
	TmGetTime(const TmGetTime& t);
	~TmGetTime();

	/** \brief get the difference between the current source control value
	* and its value since it was last read. Does not currently work for
	* Windows TODO! architecture as it relies on the Unix function gettimeofday().
	* \return the count since last read
	*/
	mrs_natural readTimeSrc();
	/** \brief convert the given interval into a number of micro seconds.
	*
	* The interval must be defined in real time units: us, ms, s, m, h, d.
	* \param interval the interval to calculate
	* \return the number of micro seconds contained in the interval
	*/
	mrs_natural intervalsize(std::string interval);
};

}//namespace Marsyas

#endif
