/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "BeatHistogramFromPeaks.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;


BeatHistogramFromPeaks::BeatHistogramFromPeaks(mrs_string name):MarSystem("BeatHistogramFromPeaks",name)
{
  addControls();
}


BeatHistogramFromPeaks::~BeatHistogramFromPeaks()
{
}


MarSystem*
BeatHistogramFromPeaks::clone() const
{
  return new BeatHistogramFromPeaks(*this);
}

void
BeatHistogramFromPeaks::addControls()
{
  addctrl("mrs_real/gain", 1.0);
  addctrl("mrs_bool/reset", false);
  setctrlState("mrs_bool/reset", true);
  addctrl("mrs_natural/startBin", 0);
  setctrlState("mrs_natural/startBin", true);
  addctrl("mrs_natural/endBin", 100);
  setctrlState("mrs_natural/endBin", true);
}

void
BeatHistogramFromPeaks::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("BeatHistogramFromPeaks.cpp - BeatHistogramFromPeaks:myUpdate");

  startBin_ = getctrl("mrs_natural/startBin")->to<mrs_natural>();
  endBin_ = getctrl("mrs_natural/endBin")->to<mrs_natural>();
  reset_ = getctrl("mrs_bool/reset")->to<mrs_bool>();

  setctrl("mrs_natural/onSamples", endBin_ - startBin_);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}


void
BeatHistogramFromPeaks::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  if (reset_) {
    out.setval(0.0);
    reset_ = false;
    setctrl("mrs_bool/reset", false);
  }

  mrs_natural bin=0;
  mrs_real amp;

  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_/2; t++)
    {
      bin = (mrs_natural)(in(o,2*t+1)+0.5);
      amp = in(o,2*t);

      if ((bin < endBin_ - startBin_)&&(bin > 1))
      {
        //		out(0,bin) += amp;
        out(0,bin) += (amp * (bin-startBin_));
        /* out(0,bin-1) += 0.5 * amp;
           out(0,bin-2) += 0.25 * amp;
           out(0,bin+1) += 0.5 * amp;
           out(0,bin-2) += 0.25 * amp;
        */

        /* out(0,bin) += 0.1 * out(0, bin-1);
           out(0,bin) += 0.1 * out(0, bin+1);
        */

      }

      /*
         index = (mrs_natural) 2 * bin;
         factor = 0.25;
         if ((index < endBin_ - startBin_)&&(index > 0))
         out(0,index) += factor * amp;



         index = (mrs_natural) (0.5 * bin + 0.5);
         factor = 0.25;

         if ((index < endBin_ - startBin_)&&(index > 0))
         out(0,index) += factor * amp;

         index = 3 * bin;
         factor = 0.15;
         if ((index < endBin_ - startBin_)&&(index > 0))
         out(0,index) += factor * amp;


         index = (mrs_natural) (0.33333 * bin + 0.5);
         factor = 0.15;

         if ((index < endBin_ - startBin_)&&(index > 0))
         out(0,index) += (factor * amp);
      */

    }

}
