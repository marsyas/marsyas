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
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_Combinator_H
#define MARSYAS_Combinator_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Processing
\brief Combine observations by summing, multiplying, or picking largest or smallest of
their respective sample values.

Controls:
- \b mrs_string/combinator [rw] : choose a *, +, max, min combination operation
- \b mrs_realvec/weights [rw] : weight of combinations default [1 1 1 ... 1] with the length of the number of systems
- \b mrs_bool/numInputs [rw] : number of parallels.
*/

class marsyas_EXPORT Combinator: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  // Pointers to controls allow efficient access to their
  // values.  For clarity, we use the ctrl_ prefix so these
  // pointers can be easily identified throughout the code...
  // but this is not mandatory, just recommended)

  enum Combinators_t
  {
    kAdd,
    kMult,
    kMax,
    kMin,

    kNumCombinators
  };
  Combinators_t GetCombinatorIdx (const mrs_string ctrlString);

  static const mrs_string combinatorStrings[kNumCombinators];

  /// MarControlPtr for the gain control
  MarControlPtr	ctrl_weights_,
                ctrl_combinator_,
                ctrl_numInputs_;

public:
  /// Combinator constructor.
  Combinator(std::string name);

  /// Combinator copy constructor.
  Combinator(const Combinator& a);

  /// Combinator destructor.
  ~Combinator();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_Combinator_H

