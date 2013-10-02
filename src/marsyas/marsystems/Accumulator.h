/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNAESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_ACCUMULATOR_H
#define MARSYAS_ACCUMULATOR_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{

/**
	\class Accumulator
	\ingroup Composites
	\brief Accumulate result of multiple ticks

	Accumulate result of multiple tick process calls
	to child (i.e internal) MarSystem. Spit output only once when
	all the results are accumulated (either using a predefined
    number of processing ticks or when explicitly "flushed").
    Used to change the rate of process requests.

	For example, if \c mode is "countTicks" and \c nTimes is 5, then
	each time the Accumulator receives a tick(), it sends 5 tick()s to
	the MarSystems that are inside it.

	Controls: - \b mrs_string/mode [rw] : in "countTicks" mode
      accumulates and outputs the result of multiple tick process
      calls (specified by the value in the \c nTimes control) to the
      child MarSystem. In "explicitFlush" mode, it keeps accumulating
      the result of multiple tick process calls to the child MarSystem
      until the \c flush control is set to true, subsequently sending
      the accumulated data to the output.

	- \b mrs_natural/nTimes [rw] : when in "countTicks" mode, it is
		the multiplier of ticks() for the child (i.e. internal)
		MarSystem. Ignored in "explicitFlush" mode.

    - \b mrs_bool/flush [w] : when in "explicitFlush" mode, setting it
      to true stops the current accumulator procedure and outputs the
      accumulated data. Ignored in "countTicks" mode.

    - \b mrs_natural/maxTimes [rw] : when in "explicitFlush" mode,
      this sets the maximum number of times the Accumulator ticks the
      child MarSystem, in the absence of a flush event (e.g. used to
      avoid an "infinite" accumulation of data in case a flush event
      fails to exist). Ignored in "countTicks" mode.

    - \b mrs_natural/minTimes [rw] : when in "explicitFlush" mode,
      this sets the minimum number of times the Accumulator ticks the
      child MarSystem, even in the presence of a flush event
      (e.g. used to set a minimum "distance" between consecutive
      flushes). Ignored in "countTicks" mode.

    - \b mrs_natural/timesToKeep [rw] : when in "explicitFlush" mode,
      this control specifies the number of tick (i.e
      processing)results output by the child MarSystem that will not
      be output when a flush event is issued, being instead kept
      internally for the next accumulator tick. E.g., in some usage
      scenarios (e.g. onset detection), the decision to flush the
      accumulator may need some additional data (i.e. ticks) before
      the flush event is issued. In such cases, the \c timesToKeep
      control allows the specify how many ticks "into the future" it
      will need to accumulate in order to be able to decide a
      flush. In such a case, the onset event (which should correspond
      to a flush to the accumulator) should have benen issued
      "timesToKeep" ticks ago... a way to achieve that is to output
      the accumulated data minus the last "timesToKeep" ticks, which
      will be held internally for the next accumulation process. A
      graphical example, where each dot (".") represents an
      accumulated output from the child MarSystem, and the X is a
      special output where some flush decision should be made:

        ..........X.

        So, having the graphic above in mind, supose we have an
        Accumulator in "explicitFlush", with a child onset detector
        MarSystem that at each tick, looks at its previous output and
        compares it to the current one, deciding if an onset existed
        one output ago (so, it need to look one output into the future
        - which is the current one! - to be able to decide). In case
        it determines an onset occured one output ago (marked as X in
        the picture above), it sets an "onset deteted" control (which
        should be linked to the \c flush control in the parent
        Accumulator), making the Accumulator to stop accumulating and
        output the accumulated data till the detected onset. However,
        if the \c timesToKeep value is set to zero, the Accumulator
        will output the entire accumulated data, and usually in such
        an onset detection scenario, it would be more interesting to
        have it just output the data till the detected onset, keeping
        the remaining data for further accumulation. The way to
        achieve that is to specifiy a non-zero value to the \c
        timesToKeep control, which in the current example could be set
        to 2 (i.e. both the previous and current data outputs will be
        kept internally, and all the previous accumulated data output
        - we could call it the pre-onset data).


*/
class Accumulator: public MarSystem
{
private:
  MarControlPtr ctrl_nTimes_;
  MarControlPtr ctrl_maxTimes_;
  MarControlPtr ctrl_minTimes_;
  MarControlPtr ctrl_timesToKeep_;

  MarControlPtr ctrl_flush_;
  MarControlPtr ctrl_mode_;

  mrs_natural nTimes_;

  mrs_natural childOnSamples_;
  mrs_natural keptOnSamples_;

  realvec childOut_;
  realvec tout_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Accumulator(std::string name);
  Accumulator(const Accumulator& a);
  ~Accumulator();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
  bool addMarSystem(MarSystem *marsystem);

};

}//namespace Marsyas

#endif
