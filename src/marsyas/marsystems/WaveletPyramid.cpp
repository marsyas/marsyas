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

#include "WaveletPyramid.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

WaveletPyramid::WaveletPyramid(mrs_string name):MarSystem("WaveletPyramid",name)
{
  waveletStep_ = NULL;

  addControls();
}

WaveletPyramid::~WaveletPyramid()
{
  delete waveletStep_;
}


// copy constructor
WaveletPyramid::WaveletPyramid(const WaveletPyramid& a):MarSystem(a)
{
  waveletStep_ = NULL;
}



MarSystem*
WaveletPyramid::clone() const
{
  return new WaveletPyramid(*this);
}

void
WaveletPyramid::addControls()
{
  addctrl("mrs_bool/forward", true);
}

void
WaveletPyramid::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  if (waveletStep_ == NULL)
  {
    // TODO: why is this in myUpdate and not in the constructors?
    waveletStep_ = new Daub4("daub4");
  }

  MRSDIAG("WaveletPyramid.cpp - WaveletPyramid:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  waveletStep_->updControl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
  waveletStep_->updControl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
  waveletStep_->updControl("mrs_real/israte", getctrl("mrs_real/israte"));

}


void
WaveletPyramid::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_natural nn;
  mrs_natural n;
  mrs_bool forward;

  n = getctrl("mrs_natural/inSamples")->to<mrs_natural>();

  if (n < 4)
  {
    // TODO: why do we return here immediately, and not after copying the
    // input to the output for example? Please explain.
    return;
  }

  // Copy input to output.
  for (o = 0; o < inObservations_; o++)
  {
    for (t = 0; t < inSamples_; t++)
    {
      out(o, t) = in(o, t);
    }
  }

  forward = getctrl("mrs_bool/forward")->to<mrs_bool>();

  waveletStep_->updControl("mrs_bool/forward", forward);
  if (forward)
  {
    for (nn = n; nn >=4; nn >>= 1)
    {
      waveletStep_->setctrl("mrs_natural/processSize", nn);
      waveletStep_->process(out, out);
    }
  }
  else
  {
    for (nn = 4; nn <= n; nn <<= 1)
    {
      waveletStep_->setctrl("mrs_natural/processSize", nn);
      waveletStep_->process(out, out);
    }
  }

}
