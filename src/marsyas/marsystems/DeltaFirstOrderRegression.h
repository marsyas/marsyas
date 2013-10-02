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

#ifndef MARSYAS_DELTAFIRSTORDERREGRESSION_H
#define MARSYAS_DELTAFIRSTORDERREGRESSION_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\class DeltaFirstOrderRegression
\ingroup Processing
\brief Calculate simple time domain delta in first order regression fashion.

This Delta calculation is typically used for Delta-MFCC features.

The calculation of the first order regression delta is as follows.
Given a signal \f$x[t]\f$ (discrete time \f$t\f$). For the delta at
time \f$t\f$, we consider the three samples \f$x[t-1]\f$, \f$x[t]\f$ and \f$x[t+1]\f$
and fit a linear function \f$y=a t + b\f$ to these points in the least squares sense.
The slope \f$a\f$ of this function is used as delta \f$\Delta[t]\f$ for time \f$t\f$.
Mathematically, this regression problem with three points at uniform time
point simplifies to a simple calculation \f[\Delta[t] = \frac{x[t+1] - x[t-1]}{2}\f].

This MarSystem uses the calculation above, but adds a time lag of one sample
to make the calculation causal (otherwise we need a sample from the future).

Works with multiple observation channels and works across slices
(the MarSystem keeps an internal buffer of two samples from previous
slice).

This MarSystem has no additional controls.
*/

class marsyas_EXPORT DeltaFirstOrderRegression: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  /// Buffers of previous samples.
  realvec memory_;

public:
  /// DeltaFirstOrderRegression constructor.
  DeltaFirstOrderRegression(std::string name);

  /// DeltaFirstOrderRegression copy constructor.
  DeltaFirstOrderRegression(const DeltaFirstOrderRegression& a);

  /// DeltaFirstOrderRegression destructor.
  ~DeltaFirstOrderRegression();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_DELTAFIRSTORDERREGRESSION_H

