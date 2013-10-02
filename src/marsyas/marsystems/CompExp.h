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

#ifndef MARSYAS_COMPEXP_H
#define MARSYAS_COMPEXP_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
  \class CompExp
  \ingroup Processing
  \brief Compressor/Expander

  Acts as a compressor or expander: scales the signal above a desired
  threshold.

  Controls:
  - \b mrs_real/thresh [w] : Threshold (in the range [-1,1]).
    If negative, effective slope will be 1/slope, thus switching between
    operation as compressor and expander. Effective threshold will always
    be absolute value of this control.
  - \b mrs_real/slope [w]: Scaling of signal above threshold.
    - slope < 1 => operates as compressor
    - slope > 1 => operates as expander
  - \b mrs_real/at [w]: Attack time - time for scaling to fade into effect.
  - \b mrs_real/rt [w]: Release time - time for scaling to fade out of effect.

*/

/* How to use the Compressor/Expander Marsystem

  The system can be setup using the marsystem manager
    series->addMarSystem(mng.create("CompExp", "compexp"));

  Options: Threshold can be set to any value between -1.0 and 1
  Compressor  -1.0 <= thresh < 0
  Expander	0 <= thresh <= 1.0
    series->updctrl("CompExp/compexp/mrs_real/thresh", thresh);

  Attack time can be calculated using the following formula: at = 1 - exp(-2.2*T/t_AT)
  where at = attack time, T = sampling period, t_AT = time parameter 0.00016 < t_AT < 2.6 sec
    series->updctrl("CompExp/compexp/mrs_real/rolloff", t_AT);

  Release time can be calculated similar to at time: rt = 1 - exp(-2.2*T/t_RT)
  where rt = release time, T = sampling period, t_RT = time parameter 0.001 < t_RT < 5.0 msec
    series->updctrl("CompExp/compexp/mrs_real/rt", t_RT);

  Slope factor: 0 < slope <= 1.0
    series->updctrl("CompExp/compexp/mrs_real/rt", slope);
*/


class CompExp: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctl_thresh;
  MarControlPtr ctl_slope;
  MarControlPtr ctl_attack;
  MarControlPtr ctl_release;

  mrs_real m_thresh;
  mrs_real m_thresh_log10;
  mrs_real m_slope;
  mrs_real m_k_attack;
  mrs_real m_k_release;

  realvec m_xd;

public:
  CompExp(std::string name);
  ~CompExp();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
