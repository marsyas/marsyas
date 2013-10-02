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
#include "HarmonicStrength.h"

using std::ostringstream;
using namespace Marsyas;

using namespace std;

HarmonicStrength::HarmonicStrength(mrs_string name) : MarSystem("HarmonicStrength", name)
{
  addControls();
}

HarmonicStrength::HarmonicStrength(const HarmonicStrength& a) : MarSystem(a)
{
  ctrl_base_frequency_ = getctrl("mrs_real/base_frequency");
  ctrl_harmonics_ = getctrl("mrs_realvec/harmonics");
  ctrl_harmonicsSize_ = getctrl("mrs_natural/harmonicsSize");
  ctrl_harmonicsWidth_ = getctrl("mrs_real/harmonicsWidth");
  ctrl_inharmonicity_B_ = getctrl("mrs_real/inharmonicity_B");
}


HarmonicStrength::~HarmonicStrength()
{
}

MarSystem*
HarmonicStrength::clone() const
{
  return new HarmonicStrength(*this);
}

void
HarmonicStrength::addControls()
{
  addctrl("mrs_real/base_frequency", 440.0, ctrl_base_frequency_);
  addctrl("mrs_realvec/harmonics", realvec(0), ctrl_harmonics_);
  addctrl("mrs_natural/harmonicsSize", 0, ctrl_harmonicsSize_);
  setctrlState("mrs_natural/harmonicsSize", true);
  addctrl("mrs_real/harmonicsWidth", 0.05, ctrl_harmonicsWidth_);
  addctrl("mrs_natural/type", 0);
  addctrl("mrs_real/inharmonicity_B", 0.0, ctrl_inharmonicity_B_);
}

void
HarmonicStrength::myUpdate(MarControlPtr sender)
{
  MRSDIAG("HarmonicStrength.cpp - HarmonicStrength:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  mrs_natural num_harmonics = ctrl_harmonicsSize_->to<mrs_natural>();
  // setup default harmonics
  {
    // leave this here for the scope of acc
    MarControlAccessor acc(ctrl_harmonics_);
    mrs_realvec& harmonics = acc.to<mrs_realvec>();
    if ((num_harmonics > 0) && (harmonics.getSize() == 0))
    {
      harmonics.stretch(num_harmonics);
      for (mrs_natural i=0; i < num_harmonics; ++i)
      {
        harmonics(i) = i+1;
      }
    }
  }
  ctrl_onObservations_->setValue(ctrl_harmonicsSize_->to<mrs_natural>(), NOUPDATE);

  //features names
  mrs_string orig = ctrl_inObsNames_->to<mrs_string>();
  // remove final comma in name
  orig = orig.substr(0, orig.size()-1);
  ostringstream oss;
  for (mrs_natural i = 0; i < num_harmonics; ++i)
  {
    oss << "HarmonicStrength_" + orig << i+1 << ",";
  }
  setctrl("mrs_string/onObsNames", oss.str());
}


// used inside myProcess
mrs_real
HarmonicStrength::quadratic_interpolation(mrs_real best_bin,
    mrs_realvec& in, mrs_natural t)
{
  if ((best_bin == 0) || (best_bin == in.getRows()-1))
  {
    // don't try to interpolate using data that's
    // outside of the spectogram
    // TODO: find some convincing DSP thing to do in this case
    return in( (mrs_natural)best_bin, t);
  }
  // https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
  // a = alpha, b = beta, g = gamma
  mrs_real a = in( (mrs_natural)best_bin - 1, t);
  mrs_real b = in( (mrs_natural)best_bin + 0, t);
  mrs_real g = in( (mrs_natural)best_bin + 1, t);

  mrs_real p = 0.5 * (a-g)/(a-2*b+g);
  // avoid some NaNs
  if ((p < -0.5) || (p > 0.5))
  {
    return b;
  }
  mrs_real yp = b - 0.25*(a-g)*p;
  // avoid all NaNs
  if (yp < b)
  {
    // I think this happens because the search window doesn't
    // encompass the entire spectrum, so the "highest" bin
    // might not actually be the highest one, if it was on the
    // edge of search window.
    // TODO: find some convincing DSP thing to do in this case
    return b;
  }
  return yp;
}

mrs_real
HarmonicStrength::find_peak_magnitude(mrs_real central_bin, mrs_realvec& in,
                                      mrs_natural t, mrs_real low,
                                      mrs_real high)
{
  // find peak in 2*radius
  // in real-world signals, the harmonic isn't always a
  // literal multiple of the base frequency.  We allow a bit
  // of margin (the "radius") to find the best bin
  mrs_natural best_bin = -1;
  mrs_real best_magnitude = 0;
  if (low < 0)
  {
    low = 0;
  }
  if (high > inObservations_ - 1)
  {
    high = inObservations_ - 1;
  }
  for (mrs_natural i = (mrs_natural)low; i < high; i++)
  {
    if (in(i,t) > best_magnitude)
    {
      best_bin = i;
      best_magnitude = in(i,t);
    }
  }
  if (best_bin >= 0)
  {
    best_magnitude = quadratic_interpolation(best_bin, in, t);
  }
  else
  {
    best_magnitude = in( (mrs_natural)central_bin, t);
  }

  return best_magnitude;
}

void
HarmonicStrength::myProcess(realvec& in, realvec& out)
{
  mrs_natural num_harmonics = ctrl_harmonicsSize_->to<mrs_natural>();
  mrs_real base_freq = ctrl_base_frequency_->to<mrs_real>();
  MarControlAccessor acc(ctrl_harmonics_);
  mrs_realvec& harmonics = acc.to<mrs_realvec>();
  mrs_real width = ctrl_harmonicsWidth_->to<mrs_real>();

  mrs_real freq2bin = 1.0 / ctrl_israte_->to<mrs_real>();
  //mrs_real bin2freq = ctrl_israte_->to<mrs_real>();

  // Iterate over the samples (remember, FFT is vertical)
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    mrs_real energy_rms = 0.0;
    for (mrs_natural o = 0; o < inObservations_; o++)
    {
      energy_rms += in(o, t) * in(o,t);
    }
    energy_rms = sqrt(energy_rms);

    for (mrs_natural h = 0; h < num_harmonics; h++)
    {
      mrs_real n = harmonics(h);
      mrs_real B = ctrl_inharmonicity_B_->to<mrs_real>();

      mrs_real freq = n * base_freq * sqrt(1.0 + B*n*n);
      mrs_real bin = freq * freq2bin;
      //mrs_real freqold= n*base_freq;
      //mrs_real binold = freqold*freq2bin;
      //cout<<B<<"\t"<<freq<<"\t"<<bin<<endl;
      mrs_real low = bin - width * inObservations_;
      mrs_real high = bin + width * inObservations_;
      //cout<<low<<"\t"<<high<<endl;
      //cout<<low*bin2freq<<"\t"<<high*bin2freq<<endl;
      mrs_real magnitude = find_peak_magnitude(bin, in, t, low, high);
      if (magnitude == 0)
      {
        magnitude = 1e-60;
      }
      else
      {
        switch (getctrl("mrs_natural/type")->to<mrs_natural>())
        {
        case 0:
          //out(h, t) = log(magnitude / energy_rms);
          out(h, t) = magnitude / energy_rms;
          break;
        case 1:
          out(h, t) = magnitude;
          break;
        case 2:
          out(h, t) = log(magnitude);
          break;
        default:
          out(h, t) = magnitude;
          break;
        }
      }
      /*
      if (out(h,t) != out(h,t)) {
          cout<<"zzzzzzzzzz NaN detected! zzzzzzz"<<endl;
          cout<<magnitude<<"\t"<<energy_rms<<endl;
      }
      */
    }
  }
}

