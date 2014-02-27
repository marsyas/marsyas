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

#ifndef MARSYAS_THRESHOLD_MARSYSTEM_INCLUDED
#define MARSYAS_THRESHOLD_MARSYSTEM_INCLUDED

#include <marsyas/system/MarSystem.h>

namespace Marsyas {

using namespace Marsyas;
using std::string;

/**
 * @brief Count of observations above threshold.
 *
 * Outputs a single observation per sample expressing the count of
 * observations at that sample with value above the given threshold.
 *
 * \b Controls:
 * - \b mrs_real/value: Threshold value. Default: 0.0.
 */

class Threshold : public MarSystem
{
public:
  Threshold(const string & name);
  Threshold(const Threshold & other);
  MarSystem *clone() const { return new Threshold(*this); }

private:
  void myUpdate( MarControlPtr );
  void myProcess( realvec & in, realvec & out );

  mrs_real m_threshold;
};

} // namespace Marsyas

#endif // MARSYAS_THRESHOLD_MARSYSTEM_INCLUDED
