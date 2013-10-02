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

#ifndef MARSYAS_REPEAT_H
#define MARSYAS_REPEAT_H

#include <marsyas/common_header.h>
#include <marsyas/Conversions.h>

#include <string>

namespace Marsyas
{
/**
	\class Repeat
	\ingroup Scheduler
	\brief encapsulates repetition information for scheduled events
	\author Neil Burroughs  inb@cs.uvic.ca
*/

class marsyas_EXPORT Repeat {
private:
  bool infinite_;
  std::string interval_;
  mrs_natural count_;

public:
  /** \brief specify no repetition */
  Repeat();
  /** \brief specify infinite repetition
  * \param time_interval the interval of time between repetitions
  */
  Repeat(std::string time_interval);
  /** \brief specify finite repetition
  * \param time_interval the interval of time between repetitions
  * \param rep_count the number of times to repeat
  */
  Repeat(std::string time_interval, mrs_natural rep_count);

  virtual ~Repeat();

  /** \brief specify finite repetition
  * \param inf true if repeat is to be infinite which negates rep_count
  * \param time_interval the interval of time between repetitions
  * \param rep_count the number of times to repeat
  */
  void set(bool inf, std::string time_interval, mrs_natural rep_count);

  /** \brief convert the repeat interval to a sample count based on
  * the given sample rate
  * \param srate the sample rate to be used in the conversion
  * \return the count of samples
  */
  mrs_natural interval2samples(mrs_real srate);

  /** \brief report whether this Repeat specifies another repeat
  * \return true if repeat is warranted
  */
  virtual bool repeat();

  bool isInfinite() {return infinite_;}
  mrs_natural getCount() {return count_;}
  std::string getInterval() {return interval_;}

  void setInfinite(bool inf) {infinite_=inf;}
  void setCount(mrs_natural count) {count_=count;}
  void setInterval(std::string interval) {interval_=interval;}

  void operator++() {++count_;}
  void operator++(int) {count_++;};
  void operator--() {if(count_>0) {--count_;}}
  void operator--(int) {if(count_>0) {count_--;}};
  // the usual stream IO
//    friend ostream& operator<<(ostream&, Scheduler&);
//    friend istream& operator>>(istream&, Scheduler&);
};

}//namespace Marsyas

#endif
