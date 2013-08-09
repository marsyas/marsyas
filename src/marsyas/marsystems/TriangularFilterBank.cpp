/*
** Copyright (C) 1998-2012 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "TriangularFilterBank.h"


using std::ostringstream;


using namespace Marsyas;

TriangularFilterBank::TriangularFilterBank(mrs_string name):MarSystem("TriangularFilterBank",name)
{
  addControls();
  pfftSize_ = 0;
  psamplingRate_ = 0;
  mfcc_offsets_ = NULL;
  pcepstralCoefs_ = 0;
  cepstralCoefs_ = TriangularFilterBank::cepstralCoefs_default;
}

TriangularFilterBank::TriangularFilterBank(const TriangularFilterBank& a) : MarSystem(a)
{
  ctrl_cepstralCoefs_ = getctrl("mrs_natural/coefficients");
  pfftSize_ = 0;
  psamplingRate_ = 0;
  mfcc_offsets_ = NULL;
  pcepstralCoefs_ = 0;
  cepstralCoefs_ = TriangularFilterBank::cepstralCoefs_default;
}

TriangularFilterBank::~TriangularFilterBank()
{
  if (mfcc_offsets_!=NULL)
    delete [] mfcc_offsets_;
}

MarSystem*
TriangularFilterBank::clone() const
{
  return new TriangularFilterBank(*this);
}


void
TriangularFilterBank::addControls() {
  /// Add any specific controls needed by this MarSystem.
  addControl("mrs_natural/coefficients", TriangularFilterBank::cepstralCoefs_default, ctrl_cepstralCoefs_);
  setControlState("mrs_natural/coefficients", true);
}

void
TriangularFilterBank::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  // Get the number of cepstral coefficients from the control
  cepstralCoefs_ = ctrl_cepstralCoefs_->to<mrs_natural>();

  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue((mrs_natural)cepstralCoefs_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  // Initialize frequency boundaries for filters
  mrs_natural i,j;
  fftSize_ = 2 * (ctrl_inObservations_->to<mrs_natural>()-1); //PowerSpectrum outputs N/2+1 "magnitude" spectral points!
  if (fftSize_ == 0) return; // skip unnecessary updates

  samplingRate_ = (mrs_natural) (ctrl_israte_->to<mrs_real>() * fftSize_);

  // Set the observation names: use the first item of the
  // input observation names and prefix it with "TriangularFilterBankxx"
  mrs_string inObsName = stringSplit(ctrl_inObsNames_->to<mrs_string>(), ",")[0];
  ostringstream oss;
  for (i=0; i < cepstralCoefs_; ++i)
  {
    oss << "TriangularFilterBank" << i << "_" << inObsName << ",";
  }
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  if ((pfftSize_ != fftSize_) || (psamplingRate_ != samplingRate_) || (pcepstralCoefs_ != cepstralCoefs_))
  {

    freqs_.create(42);
    lowestFrequency_ = 133.3333f;
    linearFilters_ = 13;
    linearSpacing_ = 66.66666f;
    logFilters_ = 27;
    logSpacing_ = 1.0711703f;

    totalFilters_ = linearFilters_ + logFilters_;
    lower_.create(totalFilters_);
    center_.create(totalFilters_);
    upper_.create(totalFilters_);
    triangle_heights_.create(totalFilters_);


    ctrl_onObservations_->setValue((mrs_natural)totalFilters_, NOUPDATE);

    // Linear filter boundaries
    for (i=0; i< linearFilters_; ++i)
    {
      freqs_(i) = lowestFrequency_ + i * linearSpacing_;
    }

    // Logarithmic filter boundaries
    mrs_real first_log = freqs_(linearFilters_-1);
    for (i=1; i<=logFilters_+2; ++i)
    {
      freqs_(linearFilters_-1+i) = first_log * pow(logSpacing_, (mrs_real)i);
    }

    // Triangles information
    for (i=0; i<totalFilters_; ++i)
    {
      lower_(i) = freqs_(i);
    }

    for (i=1; i<= totalFilters_; ++i)
    {
      center_(i-1) = freqs_(i);
    }

    for (i=2; i<= totalFilters_+1; ++i)
    {
      upper_(i-2) = freqs_(i);
    }

    for (i=0; i<totalFilters_; ++i)
    {
      triangle_heights_(i) = (mrs_real)(2.0 / (upper_(i) - lower_(i)));
    }

    fftFreqs_.stretch(fftSize_);

    for (i=0; i< fftSize_; ++i)
    {
      fftFreqs_(i) = (float)i / (float)fftSize_ * (float)samplingRate_;
    }

    mfccFilterWeights_.create(totalFilters_, fftSize_);
    mfccDCT_.create(cepstralCoefs_, totalFilters_);

    mrs_natural chan;

    // NEIL's filter weight speedup
    if (pfftSize_!=fftSize_)
    {
      if (mfcc_offsets_!=NULL)
      {
        delete [] mfcc_offsets_;
      }
      mfcc_offsets_ = new int[totalFilters_*fftSize_*2];
    }
    // Initialize mfccFilterWeights
    for (chan = 0; chan < totalFilters_; chan++)
    {
      // NEIL's filter weight speedup
      int len=-1;
      int pos=0;
      for (i=0; i< fftSize_; ++i)
      {
        if ((fftFreqs_(i) > lower_(chan))&& (fftFreqs_(i) <= center_(chan)))
        {
          mfccFilterWeights_(chan, i) = triangle_heights_(chan) *
                                        ((fftFreqs_(i) - lower_(chan))/(center_(chan) - lower_(chan)));
          // NEIL's filter weight speedup
          if (len==-1)
          {
            pos=i;
          }
          len=i;
        }
        if ((fftFreqs_(i) > center_(chan)) && (fftFreqs_(i) <= upper_(chan)))
        {
          mfccFilterWeights_(chan, i) = triangle_heights_(chan) *
                                        ((upper_(chan) - fftFreqs_(i))/(upper_(chan) - center_(chan)));
          // NEIL's filter weight speedup
          if (len==-1)
          {
            pos=i;
          }
          len=i;
        }
      }
      // NEIL's filter weight speedup
      mfcc_offsets_[chan] = pos;
      mfcc_offsets_[chan+totalFilters_] = len;
    }

    // Initialize TriangularFilterBank_DCT
    mrs_real scale_fac = (mrs_real)(1.0/ sqrt((mrs_real)(totalFilters_/2)));
    for (j = 0; j<cepstralCoefs_; j++)
    {
      for (i=0; i< totalFilters_; ++i)
      {
        mfccDCT_(j, i) = scale_fac * cos(j * (2*i +1) * PI/2/totalFilters_);
        if (j == 0)
        {
          mfccDCT_(j,i) *= (mrs_real)(sqrt(2.0)/2.0);
        }
      }
    }
  }


  pfftSize_ = fftSize_;
  psamplingRate_ = samplingRate_;

  fmagnitude_.stretch(ctrl_inObservations_->to<mrs_natural>() * 2);
  earMagnitude_.stretch(totalFilters_);
}

void
TriangularFilterBank::myProcess(realvec& in, realvec& out)
{
  mrs_natural i,k,o;

  // mirror the spectrum
  for (o=0; o < inObservations_; o++)
  {
    fmagnitude_(o) = in(o,0);
  }

  for (o=0; o< inObservations_; o++)
  {
    fmagnitude_(o + inObservations_) = fmagnitude_(inObservations_ - o -1);
  }

  mrs_real sum =0.0;
  // Calculate the filterbank responce
  for (i=0; i<totalFilters_; ++i)
  {
    sum = 0.0;
    // NEIL's filter weight speedup
    for (k=mfcc_offsets_[i]; k<=mfcc_offsets_[i+totalFilters_]; k++)
    {
      sum += (mfccFilterWeights_(i, k) * fmagnitude_(k));
    }
    if (sum != 0.0)
    {
      earMagnitude_(i) = log10(20.0 * sum + 1.0);
    }
    else
    {
      earMagnitude_(i) = 0.0;
    }
  }

  for (o=0; o < onObservations_; o++)
    out(o,0) = earMagnitude_(o);

//	MRSMSG(out);

}

