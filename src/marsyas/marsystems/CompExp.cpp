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

#include "CompExp.h"
#include "../common_source.h"
#include <algorithm>
#include <cmath>

using std::ostringstream;
using namespace Marsyas;

CompExp::CompExp(mrs_string name):
  MarSystem("CompExp",name)
{
  addControls();
  update();
}


CompExp::~CompExp()
{
}


MarSystem*
CompExp::clone() const
{
  return new CompExp(*this);
}

void
CompExp::addControls()
{
  addctrl("mrs_real/thresh", 1.0);
  addctrl("mrs_real/slope", 1.0);
  addctrl("mrs_real/at", 0.0001);
  addctrl("mrs_real/rt", 0.130);
  setControlState("mrs_real/thresh", true);
  setControlState("mrs_real/slope", true);
  setControlState("mrs_real/at", true);
  setControlState("mrs_real/rt", true);
}

void
CompExp::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("CompExp.cpp - CompExp:myUpdate");

  mrs_natural inObservations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  setctrl("mrs_natural/onSamples", inSamples);
  setctrl("mrs_natural/onObservations", inObservations);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  //defaultUpdate(); [!]

  mrs_real thresh = getControl("mrs_real/thresh")->to<mrs_real>();
  m_thresh = std::abs(thresh);
  m_thresh_log10 = std::log10(m_thresh);

  m_slope = getControl("mrs_real/slope")->to<mrs_real>();
  if (thresh < 0.0)
    m_slope = 1.0  / m_slope;

  mrs_real attack = std::max( getControl("mrs_real/at")->to<mrs_real>(), (mrs_real) 0.0 );

  mrs_real release = std::max( getControl("mrs_real/rt")->to<mrs_real>(), (mrs_real) 0.0 );

  // calculate attack and release coefficients from times
  m_k_attack = attack > 0.0 ? 1 - exp(-2.2/(osrate_*attack)) : 1.0;

  m_k_release = release > 0.0 ? 1 - exp(-2.2/(osrate_*release)) : 1.0;

  if (tinObservations_ != inObservations_)
    m_xd.create(inObservations);
}


void
CompExp::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);

  for (mrs_natural o = 0; o < inObservations_; o++)
  {
    mrs_real xd_prev = m_xd(o);

    for (mrs_natural t = 0; t < inSamples_; t++)
    {
      mrs_real x = in(o,t);
      mrs_real x_abs = std::abs(x);

      // Calculates the current amplitude of signal and incorporates
      // the at and rt times into xd
      mrs_real alpha = std::max( x_abs - xd_prev, (mrs_real) 0 );
      mrs_real xd = xd_prev * (1-m_k_release) + alpha * m_k_attack;
      xd_prev = xd;

      mrs_real gain;

      if (xd > m_thresh)
      {
        gain = pow((mrs_real)10.0, m_slope * (log10(xd) - m_thresh_log10)) * m_thresh / xd;
      }
      else
      {
        gain = 1.0;
      }

      out(o,t) =  gain * x;
    }

    m_xd(o) = xd_prev;
  }
}








