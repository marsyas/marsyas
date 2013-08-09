/*
** Copyright (C) 1998-2008 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "PvUnconvert.h"

#include <algorithm>

using std::ostringstream;

using namespace Marsyas;

PvUnconvert::PvUnconvert(mrs_string name):MarSystem("PvUnconvert",name)
{
  //type_ = "PvUnconvert";
  //name_ = name;

  addControls();
  transient_counter_ = 0;
}


PvUnconvert::PvUnconvert(const PvUnconvert& a):MarSystem(a)
{
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_peakPicking_ = getctrl("mrs_string/peakPicking");

  ctrl_lastphases_ = getctrl("mrs_realvec/lastphases");
  ctrl_analysisphases_ = getctrl("mrs_realvec/analysisphases");
  ctrl_regions_ = getctrl("mrs_realvec/regions");
  ctrl_magnitudes_ = getctrl("mrs_realvec/magnitudes");
  ctrl_peaks_ = getctrl("mrs_realvec/peaks");

  ctrl_phaselock_ = getctrl("mrs_bool/phaselock");

  transient_counter_ = 0;

}


PvUnconvert::~PvUnconvert()
{

}


MarSystem*
PvUnconvert::clone() const
{
  return new PvUnconvert(*this);
}


void
PvUnconvert::addControls()
{
  addctrl("mrs_natural/Interpolation", MRS_DEFAULT_SLICE_NSAMPLES/4);
  addctrl("mrs_natural/Decimation", MRS_DEFAULT_SLICE_NSAMPLES/4);
  addctrl("mrs_string/mode", "loose_phaselock", ctrl_mode_);
  addctrl("mrs_string/peakPicking", "multires", ctrl_peakPicking_);
  addctrl("mrs_realvec/lastphases", realvec(), ctrl_lastphases_);
  addctrl("mrs_realvec/analysisphases", realvec(), ctrl_analysisphases_);
  addctrl("mrs_realvec/regions", realvec(), ctrl_regions_);
  addctrl("mrs_realvec/magnitudes", realvec(), ctrl_magnitudes_);
  addctrl("mrs_realvec/peaks", realvec(), ctrl_peaks_);
  addctrl("mrs_bool/phaselock", false, ctrl_phaselock_);
}

void
PvUnconvert::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>() - 2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>() / getctrl("mrs_natural/onObservations")->to<mrs_natural>());

  mrs_natural onObservations = getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  mrs_real israte = getctrl("mrs_real/israte")->to<mrs_real>();

  N2_ = onObservations/2;

  if (N2_+1 != (mrs_natural)phase_.getSize())
  {

    {
      MarControlAccessor acc(ctrl_lastphases_);
      mrs_realvec& lastphases = acc.to<mrs_realvec>();
      lastphases.create(N2_+1);
    }

    {
      MarControlAccessor acc(ctrl_analysisphases_);
      mrs_realvec& analysisphases = acc.to<mrs_realvec>();
      analysisphases.create(N2_+1);
    }


    {
      MarControlAccessor acc(ctrl_regions_);
      mrs_realvec& regions = acc.to<mrs_realvec>();
      regions.create(N2_+1);
      for (int i=0; i < N2_+1; ++i)
        regions(i) = i;
    }

    {
      MarControlAccessor acc(ctrl_magnitudes_);
      mrs_realvec& magnitudes = acc.to<mrs_realvec>();
      magnitudes.create(N2_+1);
    }

    {
      MarControlAccessor acc(ctrl_peaks_);
      mrs_realvec& peaks = acc.to<mrs_realvec>();
      peaks.create(N2_+1);
    }


    phase_.create(N2_+1);
    lphase_.create(N2_+1);
    iphase_.create(N2_+1);
    lmag_.create(N2_+1);
  }


  fundamental_ = (mrs_real) (israte  / onObservations);
  factor_ = (((getctrl("mrs_natural/Interpolation")->to<mrs_natural>()* TWOPI)/(israte)));


}


int
PvUnconvert::subband(int bin)
{
  int si;
  si = 0;

  if (bin  < 16)
    si = 0;
  else if ((bin >= 16) && (bin < 32))
    si = 1;
  else if (bin < 512)
    si = (int)(log(bin*1.0) / log(2.0))-3;
  else if (bin > 512)
    si = 6;
  return si;
}


bool
PvUnconvert::isPeak(int bin, mrs_realvec& magnitudes, mrs_real maxAmp)
{
  bool res = true;

  int h = subband(bin);
  h = 2;

  if ((bin > 2) && (bin <= N2_-2))
    for (int i = bin-h; i < bin+h; ++i)
    {
      if (magnitudes(bin) < magnitudes(i))
        res = false;
    }

  if (magnitudes(bin) < 0.005 * maxAmp)
    res = false;
  return res;
}



void
PvUnconvert::myProcess(realvec& in, realvec& out)
{

  mrs_natural t;
  MarControlAccessor acc(ctrl_lastphases_);
  mrs_realvec& lastphases = acc.to<mrs_realvec>();
  MarControlAccessor  acc1(ctrl_analysisphases_);
  mrs_realvec& analysisphases = acc1.to<mrs_realvec>();

  MarControlAccessor  acc2(ctrl_regions_);
  mrs_realvec& regions = acc2.to<mrs_realvec>();

  MarControlAccessor acc3(ctrl_magnitudes_);
  mrs_realvec& magnitudes = acc3.to<mrs_realvec>();

  MarControlAccessor acc4(ctrl_peaks_);
  mrs_realvec& peaks = acc4.to<mrs_realvec>();

  peaks.setval(0.0);


  static int count = 0;
  count++;

  mrs_natural re, amp, im, freq;
  mrs_real avg_re;
  mrs_real avg_im;

  const mrs_string& mode = ctrl_mode_->to<mrs_string>();

  mrs_real interpolation = getctrl("mrs_natural/Interpolation")->to<mrs_natural>() * 1.0;
  mrs_real decimation = getctrl("mrs_natural/Decimation")->to<mrs_natural>() * 1.0;
  mrs_real tratio = interpolation / decimation;



  mrs_real beta = 0.66 + tratio/3.0;
  if (mode == "identity_phaselock")
    beta = 1.0;

  mrs_real maxAmp =0.0;

  // calculate magnitude
  for (t=0; t <= N2_; t++)
  {
    re = amp = 2*t;
    im = freq = 2*t+1;
    if (t== N2_)
    {
      re = 1;
    }
    magnitudes(t) = in(re,0);
    if (t==N2_)
      magnitudes(t) = 0.0;
    if (magnitudes(t) > maxAmp)
      maxAmp = magnitudes(t);
  }


  if (mode == "loose_phaselock")
  {
    for (t=0; t <= N2_; t++)
    {
      re = amp = 2*t;
      im = freq = 2*t+1;
      if (t == N2_)
        re = 1;

      phase_(t) = lastphases(t) + interpolation * in(freq,0);
    }
  }

  // propagate phases for peaks and calculate regions of influence
  if ((mode == "identity_phaselock")||(mode == "scaled_phaselock"))
  {
    int previous_peak=0;
    int peak = 0;
    int peakCount = 0;

    for (t=0; t <= N2_; t++)
    {
      re = amp = 2*t;
      im = freq = 2*t+1;
      if (t == N2_)
        re = 1;
      if (isPeak(t, magnitudes, maxAmp))
      {
        peakCount++;
        if (mode == "identity_phaselock")
          iphase_(t) = lastphases(t) + interpolation * in(freq,0);
        else if (mode == "scaled_phaselock")
          iphase_(t) = lastphases((mrs_natural)regions(t)) + interpolation * in(freq,0);
      }
    }

    for (t=0; t <= N2_; t++)
    {
      if (isPeak(t, magnitudes, maxAmp))
      {
        // calculate significant peaks and corresponding
        // non-overlapping intervals
        peak = t;

        if (peak-previous_peak == 1)
          regions(peak) = peak;
        else
        {
          for (int j=previous_peak; j< previous_peak + (int)((peak-previous_peak)/2.0); j++)
          {
            peaks(j) = magnitudes(previous_peak);
            regions(j) = previous_peak;
          }

          for (int j= previous_peak + (int)((peak-previous_peak)/2.0); j < peak; j++)
          {
            peaks(j) = magnitudes(peak);
            regions(j) = peak;
          }
        }
        previous_peak = peak;
      }

    }
  }



  // resynthesis for all bins
  for (t=0; t <= N2_; t++)
  {
    re = amp = 2*t;
    im = freq = 2*t+1;

    if ((mode == "identity_phaselock")||(mode == "scaled_phaselock"))
    {
      if (t == N2_)
        re = 1;


      // unwrap analysis phases
      while (analysisphases(t) > PI)
        analysisphases(t) -= TWOPI;
      while (analysisphases(t) < -PI)
        analysisphases(t) += TWOPI;

      iphase_(t) = iphase_((mrs_natural)regions(t)) +
                   beta * (analysisphases(t) - analysisphases((mrs_natural)regions(t)));

      // sinusoidal trajectory continuation heuristic
      /* if (t - regions(t) > subband(t))
      	iphase_(t) = phase_(regions(t)) + beta * (analysisphases(t) - analysisphases(regions(t)));
      else
      {
      	iphase_(t) = lastphases(t) + interpolation * in(freq,0);
      }
      */

      // }

      if (ctrl_phaselock_->to<mrs_bool>())
      {
        iphase_(t) = analysisphases(t);
      }

      out(re,0) = magnitudes(t) * cos(iphase_(t));
      if (t != N2_)
        out(im,0) = -magnitudes(t) * sin(iphase_(t));
      lastphases(t) = iphase_(t);
    }
    else if (mode == "loose_phaselock")
    {
      if (t == N2_)
        re = 1;

      if ((t >= 2) && (t < N2_-2))
      {
        avg_re = magnitudes(t) * cos(phase_(t)) +
                 0.25 * magnitudes(t-1) * cos(phase_(t-1)) +
                 0.25 * magnitudes(t+1) * cos(phase_(t));
        avg_im = -magnitudes(t) * sin(phase_(t)) -
                 -0.25 * magnitudes(t-1) * sin(phase_(t-1)) -
                 -0.25 * magnitudes(t+1) * sin(phase_(t));
        lphase_(t) = -atan2(avg_im,avg_re);
      }
      else
        lphase_(t) = phase_(t);

      lmag_(t) = magnitudes(t);

      if (ctrl_phaselock_->to<mrs_bool>())
      {

        lphase_ = analysisphases;
        ctrl_phaselock_->setValue(false);
      }

      /* while (lphase_(t) > PI)
      	lphase_(t) -= TWOPI;
      while (lphase_(t) < -PI)
      	lphase_(t) += TWOPI;
      */


      out(re,0) = lmag_(t) * cos(lphase_(t));
      if (t != N2_)
        out(im,0) = -lmag_(t) * sin(lphase_(t));
      lastphases(t) = lphase_(t);
    }
    else // classic
    {
      if (t == N2_)
      {
        re = 1;
      }
      phase_(t) = lastphases(t) + interpolation * in(freq,0);

      if (ctrl_phaselock_->to<mrs_bool>())
      {
        phase_(t) = analysisphases(t);
      }

      out(re,0) = magnitudes(t) * cos(phase_(t));
      if (t != N2_)
        out(im,0) = -magnitudes(t) * sin(phase_(t));
      lastphases(t) = phase_(t);
    }
  }

  // Reset phaselock control
  if (ctrl_phaselock_->to<mrs_bool>())
  {
    ctrl_phaselock_->setValue(false);

  }

}























