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

#ifndef MARSYAS_SELECTOR_H
#define MARSYAS_SELECTOR_H

#include <marsyas/system/MarSystem.h>
#include <algorithm>

namespace Marsyas
{
/**
 \class Selector
 \ingroup Composites
 \brief Select different observations from input data
  \author Steven Ness (sness@sness.net)

 Select one or more observations from your input data and pass it
 to the next MarSystem.

 The enable and disable controls are used in a similar way to the
 way they are used in FanOut.

 This is useful for cases like PitchPraat that output both a
 frequency and a confidence and you just are interested in one or
 the other.

 Controls:
 - \b mrs_natural/disable [w] : The number of an observation to disable.
 - \b mrs_realvec/disableRange [w] : A realvec containing a pair of numbers:
   the low and high ends of the range of observations to disable.
 - \b mrs_natural/enable [w] : The number of a previously disabled observation
   to enable.
 - \b mrs_realvec/enableRange [w] : A realvec containing a pair of numbers:
   the low and high ends of a range of (previously disabled) observations
   to enable.

*/

class marsyas_EXPORT Selector: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_enabled_;

  bool is_enabled(const realvec & mask, int index)
  {
    return index >= mask.getSize() ||
        (index >= 0 && mask(index) > 0);
  }

  void set_enabled(realvec & mask, int index, bool enabled)
  {
    if (index < 0)
      return;
    fit_mask(mask, index);
    mask(index) = enabled ? 1.0 : 0.0;
  }

  void set_enabled_range(realvec & mask, int begin, int end, bool enabled)
  {
    if (end < begin || end < 0)
      return;
    fit_mask(mask, end);
    begin = std::max(0, begin);
    for (int i = begin; i <= end; ++i)
      mask(i) = enabled ? 1.0 : 0.0;
  }

  void fit_mask(realvec & mask, int max_index)
  {
    if (max_index < mask.getSize())
      return;
    int current_size = mask.getSize();
    int new_size = max_index+1;
    mask.stretch(new_size);
    // initialize as enabled:
    for (int i = current_size; i < new_size; ++i)
        mask(i) = 1.0;
  }

  int enabled_count(const realvec & mask, int total)
  {
    int count = 0;
    for (int i = 0; i < total; i++)
      if (is_enabled(mask, i))
        count++;
    return count;
  }

public:
  Selector(std::string name);
  Selector(const Selector& a);
  ~Selector();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

