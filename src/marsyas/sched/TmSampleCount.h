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

#include <marsyas/sched/TmTimer.h>
#include <marsyas/system/MarControlValue.h>
#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class TmSampleCount
	\ingroup Scheduler
	\brief TmSampleCount reads the insamples information to advance the timer
	\author Neil Burroughs  inb@cs.uvic.ca
*/

// forward declaration of MarSystem allows Scheduler.getctrl("insamples")
// for scheduler count
class MarSystem; // forward declaration

class TmSampleCount : public TmTimer {
protected:
  /** \brief the MarSystem containing the control to read from */
  MarSystem* read_src_;
  /** \brief the control to read from */
  std::string read_cname_;
  /** \brief the pointer, constructed in setReadCtrl that will be read to update the time */
  MarControlPtr read_ctrl_;

public:
  /** \brief empty constructor. The read source MarSystem and control are zero
  * values and must be updated using setReadCtrl(...)
  * Given the default name: Virtual as in "TmSampleTime/Virtual"
  */
  TmSampleCount();
  /** \brief named constructor. The read source MarSystem and control are zero
  * values and must be updated using setReadCtrl(...).
  * Given the default name: "TmSampleTime/name"
  * \param name a unique name to call this timer
  */
  TmSampleCount(std::string name);
  /** \brief main constructor. Has identifier "TmSampleCount/Virtual"
  * \param ms the MarSystem that contains the control to read
  * \param cname the control name to read
  */
  TmSampleCount(MarSystem* ms, std::string cname);
  /** \brief copy constructor
  * \param s timer to copy
  */
  TmSampleCount(const TmSampleCount& s);

  virtual ~TmSampleCount();

  /** \brief set the control that is to be the reference for this timer
  *
  * Set the reference control for this timer. The control must be of type
  * mrs_natural. The control value is read by getting a MarControlPtr from
  * the MarSystem that owns the control then reading that pointer on each
  * readTimeSrc() call.
  * \param ms the MarSystem that owns the control
  * \param cname the control name to read
  */
  void setReadCtrl(MarSystem* ms, std::string cname);
  /** \brief set the MarSystem that contains the read control.
  *
  * This method sets the read source to the parameter without checking.
  * It then attempts to get the MarControlPtr for the control unless the
  * read source is NULL or the read ctrl path is "". No warnings are
  * produced.
  * \param ms read source MarSystem
  */
  void setSource(MarSystem* ms);
  /** \brief set the control path
  *
  * This method sets the read control path to the parameter. It then
  * attempts to get the MarControlPtr for the control unless the
  * read source is NULL or the read ctrl path is "". No warnings are
  * produced.
  * \param cname the control path to read from
  */
  void setSourceCtrl(std::string cname);
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
