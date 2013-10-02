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

#include "../common_source.h"
#include "BeatHistogram.h"

using std::ostringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

//#define MTLB_DBG_LOG

BeatHistogram::BeatHistogram(mrs_string name):MarSystem("BeatHistogram",name)
{
  addControls();
}


BeatHistogram::~BeatHistogram()
{
}


MarSystem*
BeatHistogram::clone() const
{
  return new BeatHistogram(*this);
}

void
BeatHistogram::addControls()
{
  addctrl("mrs_real/gain", 1.0);
  addctrl("mrs_bool/reset", false);
  setctrlState("mrs_bool/reset", true);
  addctrl("mrs_natural/startBin", 0);
  setctrlState("mrs_natural/startBin", true);
  addctrl("mrs_natural/endBin", 100);
  setctrlState("mrs_natural/endBin", true);
  addctrl("mrs_real/factor", 1.0);
  addctrl("mrs_bool/tempoWeighting", false);
  addctrl("mrs_real/alpha", 0.5);
  setctrlState("mrs_real/alpha", true);
  setctrlState("mrs_real/factor", true);


}

void
BeatHistogram::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("BeatHistogram.cpp - BeatHistogram:myUpdate");

  startBin_ = getctrl("mrs_natural/startBin")->to<mrs_natural>();
  endBin_ = getctrl("mrs_natural/endBin")->to<mrs_natural>();
  reset_ = getctrl("mrs_bool/reset")->to<mrs_bool>();
  factor_ = getctrl("mrs_real/factor")->to<mrs_real>();
  alpha_ = getctrl("mrs_real/alpha")->to<mrs_real>();
  setctrl("mrs_natural/onSamples", endBin_ - startBin_);
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
}


void
BeatHistogram::myProcess(realvec& in, realvec& out)
{

  if (reset_)
  {
    out.setval(0.0);
    reset_ = false;
    setctrl("mrs_bool/reset", false);
  }


  //out.setval(0.0);

  mrs_natural bin=0;
  mrs_real amp;
  mrs_real srate = getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural count = 1;
  mrs_natural prev_bin =endBin_-1;
  mrs_natural pprev_bin =endBin_-1;
  mrs_real sumamp = 0.0;
  mrs_real tempo_weight = 0.0;


#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(in, "acr");
  MATLAB_EVAL("figure(1);plot(acr),grid on");
#endif
#endif


  for (mrs_natural o=0; o < inObservations_; o++)
  {
    for (mrs_natural t = 1; t < inSamples_; t++)
    {
      bin = (mrs_natural)(( (2*srate) * 60.0 * factor_ / (t+1)) + 0.5);

      amp = in(o,t);


      // optional tempo weight

      if (getctrl("mrs_bool/tempoWeighting")->to<mrs_bool>())
      {
        tempo_weight = 5.0 *
                       log10((t+1) * 400.0 / (srate * 60.0 * factor_)) *
                       log10((t+1) * 400.0 / (srate * 60.0 * factor_));
        tempo_weight = exp(0.5 * tempo_weight * tempo_weight);
      }
      else
      {
        tempo_weight = 1.0;
      }

      amp = tempo_weight * amp;

      if (amp < 0.0)
        amp = 0.0;


      if ((bin > 40)&&(bin < endBin_))
      {
        if (prev_bin == bin)
        {
          sumamp += amp;
          count++;
        }
        else
        {
          sumamp += amp;
          //out(o,prev_bin) += ((sumamp / count));
          out(o,prev_bin) = alpha_*out(o, prev_bin) + (1.0-alpha_)*((sumamp / count));
          //out(o,prev_bin) = out(o, prev_bin) + ((sumamp / count));
          count = 1;
          sumamp = 0.0;
        }

        // linear interpolation of the "not-set" bins...
        if (pprev_bin-prev_bin > 1)
        {
          // prev is x0, pprev is x1
          mrs_real x0 = prev_bin;
          mrs_real x1 = pprev_bin;
          mrs_real y0 = out(o, prev_bin);
          mrs_real y1 = out(o, pprev_bin);
          for (mrs_natural k = prev_bin+1; k < pprev_bin; k++)
            out (o,k)	= y0 + (y1-y0)*(k-x0)/(x1-x0);
        }

        pprev_bin = prev_bin;
        prev_bin = bin;
      }
    }



  }


#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(out, "bh");
  MATLAB_EVAL("figure(2);plot(bh),grid on");
#endif
#endif

}
