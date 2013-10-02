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

#ifndef MARSYAS_TM_VIRTUAL_TIME_H
#define MARSYAS_TM_VIRTUAL_TIME_H

#include <marsyas/sched/TmTimer.h>
#include <marsyas/system/MarControlValue.h>
#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class TmVirtualTime
	\ingroup Scheduler
	\brief TmVirtualTime reads the insamples information to advance the timer
	\author Neil Burroughs  inb@cs.uvic.ca
*/

// forward declaration of MarSystem allows Scheduler.getctrl("insamples")
// for scheduler count
class MarSystem; // forward declaration

class TmVirtualTime : public TmTimer {
protected:
  /** \brief the MarSystem containing the control to read from */
  MarSystem* read_src_;
  /** \brief the pointer, constructed in setReadCtrl that will return the
  * number of samples in the buffer: mrs_natural/onSamples */
  MarControlPtr nsamples_;
  /** \brief the pointer, constructed in setReadCtrl that will return the
  * sample rate: mrs_real/osrate */
  MarControlPtr srate_;
  /** \brief error term that is smaller than the sample size, to be added to
  * next tick calculation */
  mrs_real error_term_;
  /** \brief the value to add on the next tick. */
  mrs_natural previous_tick_interval_;

public:
  /** \brief empty constructor. The read source MarSystem and control are zero
  * values and must be updated using setReadCtrl(...)
  * Given the default name: Virtual as in "TmSampleTime/Virtual"
  */
  TmVirtualTime();
  /** \brief named constructor. The read source MarSystem and control are zero
  * values and must be updated using setReadCtrl(...).
  * Given the default name: "TmSampleTime/name"
  * \param name a unique name to call this timer
  */
  TmVirtualTime(std::string name);
  /** \brief main constructor. Has identifier "TmSampleCount/Virtual"
  * \param ms the MarSystem that contains the control to read
  * \param name the control name to read
  */
  TmVirtualTime(std::string name, MarSystem* ms);
  /** \brief copy constructor
  * \param s timer to copy
  */
  TmVirtualTime(const TmVirtualTime& s);

  virtual ~TmVirtualTime();

  /** \brief set the MarSystem that contains the read control.
  *
  * This method sets the read source to the parameter without checking.
  * It then attempts to get the MarControlPtr for the control unless the
  * read source is NULL or the read ctrl path is "". No warnings are
  * produced.
  * \param ms read source MarSystem
  */
  void setSource(MarSystem* ms);
  /** \brief get the difference between the current source control value and its
  * value since it was last read.
  * \return the count since last read
  */
  mrs_natural readTimeSrc();
  /** \brief convert the given interval into a number of samples.
  *
  * The interval must fall within sample time which can include standard time
  * units: us, ms, s, m, h, d. The sample rate used for this function is the
  * value of the mrs_real/israte control of the source MarSystem.
  * \param interval the interval to calculate
  * \return the number of samples contained in the interval
  */
  mrs_natural intervalsize(std::string interval);
  /** \brief update timer values.
  *
  * Allowable control values for this timer are: MarSystem/source, and
  * mrs_string/control.
  * \param cname the control name of a timer value
  * \param value the value to update the control with
  */
  virtual void updtimer(std::string cname, TmControlValue value);
};

}//namespace Marsyas

#endif
