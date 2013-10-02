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

#include <marsyas/common_header.h>

#include <string>

namespace Marsyas
{
/**
	\class TmTime
	\ingroup Scheduler
	\brief TmTime

	Associates a string representation of a time interval with a timer
	name on which the interval makes sense.
	\author Neil Burroughs  inb@cs.uvic.ca
*/

class marsyas_EXPORT TmTime {
protected:
  /** \brief a string representation of time meaningful on a particular timer */
  std::string time_;
  /** \brief the timer on which the specified time is meaningful */
  std::string timer_name_;

public:
  /** \brief constructor
  * \param timer_name the timer on which the specified time is meaningful
  * \param time a string representing a point in time on a given timer
  */
  TmTime(std::string timer_name, std::string time);
  virtual ~TmTime();

  /** \brief get the time that this object represents
  * \return a string representation of the time
  */
  virtual std::string getTime();

  /** \brief get the name of the timer on which this time is specified.
  * \return the name of the timer
  */
  virtual std::string getTimeName();
  /** \brief set the time
  * \param t string representing the time
  */
  virtual void setTime(std::string t);
  /** \brief set the timer name
  * \param t string representing the time
  */
  virtual void setTimeName(std::string t);
  /** \brief set everything
  * \param timer_name name of the timer
  * \param time string representing the time
  */
  virtual void set(std::string timer_name, std::string time);

  // the usual stream IO
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
