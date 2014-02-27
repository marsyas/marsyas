/*
** Copyright (C) 2014 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_RATIO_MARSYSTEM_INCLUDED
#define MARSYAS_RATIO_MARSYSTEM_INCLUDED

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Processing Basic
\brief Ratio of observations to first observation.

The count of output observations will be the count of input observations - 1,
where output observation x = input observations x / input observation 0.
Optionally, natural or base-10 logarithm may be applied to the ratios.

If there is less than 2 input observations, the output will be a constant 1.0
(or 0.0 if the logarithm is applied).

\b Controls:
- \b mrs_string/mode: One of:
    - "log": natural logarithm of ratio
    - "log10": base 10 logarithm of ratio
    - else (default): pure ratio
*/

class Ratio: public MarSystem
{
public:
  Ratio(std::string name);
  MarSystem* clone() const { return new Ratio(*this); }

private:
  enum mode
  {
    raw,
    log,
    log10
  };

  void myUpdate(MarControlPtr);
  void myProcess(realvec& in, realvec& out);
  mode m_mode;
};

} //namespace Marsyas

#endif // MARSYAS_RATIO_MARSYSTEM_INCLUDED
