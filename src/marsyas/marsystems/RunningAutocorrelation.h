/*
 ** Copyright (C) 2010 Stefaan Lippens
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

#ifndef MARSYAS_RUNNINGAUTOCORRELATION_H
#define MARSYAS_RUNNINGAUTOCORRELATION_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
 \class RunningAutocorrelation
 \ingroup Analysis
 \brief Running calculation (across slices) of the autocorrelation values.

 This MarSystem calculates the autocorrelation function of the input signal
 defined by successive input slices. Unlike the AutoCorrelation MarSystem,
 the calculations are done across slice boundaries in a seamless fashion
 (RunningAutocorrelation keeps an internal buffer of the appropriate
 number of samples from previous slices to implement this feature).
 Calculations are done in time domain for time lags from 0 to a user defined
 maximum lag (in number of samples).
 Note that this assumes that the input slices are non overlapping slices.

 The autocorrelation values are laid out in the output slice along the
 time/samples dimension from lag zero to the maximum lag.
 Multiple input observation channels are supported.
 For example, if there are two input channels and the maximum lag is 4,
 the output slice will have two rows and five (not four) columns:
 \f[\begin{array}{ccccc}
 R_{xx}[0] & R_{xx}[1] & R_{xx}[2] & R_{xx}[3] & R_{xx}[4] \\
			R_{yy}[0] & R_{yy}[1] & R_{yy}[2] & R_{yy}[3] & R_{yy}[4] \\
		\end{array}\f]
 with \f$R_{xx}[n]\f$ the autocorrelation of the first channel for lag \f$n\f$
 and \f$R_{yy}[n]\f$ the autocorrelation of the second channel.

 @todo Support overlap between slices (e.g. provide a control for skipping
 a certain amount of samples).

 Controls:
 - \b mrs_natural/maxLag: the maximum time lag (in samples) to calculate
 - \b mrs_bool/normalize: normalize the autocorrelation values on the value
 for lag = 0 (which is the energy of the signal). Note that the
 autocorrelation value for lag 0 will consequently be always 1 (unless
 the input signal is 0 everywhere).
 - \b mrs_bool/doNotNormalizeForLag0: when normalizing the autocorrelation
 values, do not normalize the value for lag 0.
 - \b mrs_bool/clear: clear the internal buffers to start fresh.
 - \b mrs_bool/unfoldToObservations: instead of putting the autocorrelation
 values along the time/samples axis in the slice, they can also be laid
 out along the observation axis. Using the example from above, the output
 slice will have one column and 10 rows with values
 \f$R_{xx}[0], R_{xx}[1], \ldots, R_{xx}[4], R_{yy}[0], R_{yy}[1], \ldots, R_{yy}[4] \f$

 */

class marsyas_EXPORT RunningAutocorrelation: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  /// Cache of the maxLag control value
  mrs_natural maxLag_;
  /// MarControlPtr for the gain control
  MarControlPtr ctrl_maxLag_;

  /// Internal buffer of the running autocorrelation values.
  realvec acBuffer_;

  /// Buffer of previous samples.
  realvec memory_;

  /// The normalize control
  MarControlPtr ctrl_normalize_;
  /// Cache of the normalize control value.
  mrs_bool normalize_;

  /// The doNotNormalizeForLag0 control.
  MarControlPtr ctrl_doNotNormalizeForLag0_;
  /// Cache of the doNotNormalizeForLag0 control value
  mrs_bool doNotNormalizeForLag0_;

  /// MarControlPtr for the clear control
  MarControlPtr ctrl_clear_;

  /// MarControlPtr for the unfoldToObservations control
  MarControlPtr ctrl_unfoldToObservations_;
  /// Cache for the unfoldToObservations control value.
  mrs_bool unfoldToObservations_;

public:
  /// RunningAutocorrelation constructor.
  RunningAutocorrelation(std::string name);

  /// RunningAutocorrelation copy constructor.
  RunningAutocorrelation(const RunningAutocorrelation& a);

  /// RunningAutocorrelation destructor.
  ~RunningAutocorrelation();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_RUNNINGAUTOCORRELATION_H

