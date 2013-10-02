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

#include "../common_source.h"
#include "AutoCorrelation.h"
#include "Windowing.h"

using std::cout;
using std::endl;

using std::ostringstream;
using namespace Marsyas;

AutoCorrelation::AutoCorrelation(mrs_string name):MarSystem("AutoCorrelation",name)
{
  myfft_ = NULL;
  addControls();
}

AutoCorrelation::~AutoCorrelation()
{
  delete myfft_;
}

// copy constructor
AutoCorrelation::AutoCorrelation(const AutoCorrelation& a):MarSystem(a)
{
  myfft_ = NULL;

  ctrl_magcompress_ = getctrl("mrs_real/magcompress");
  ctrl_normalize_ = getctrl("mrs_natural/normalize");
  ctrl_octaveCost_ = getctrl("mrs_real/octaveCost");
  ctrl_voicingThreshold_ = getctrl("mrs_real/voicingThreshold");
  ctrl_aliasedOutput_ = getctrl("mrs_bool/aliasedOutput");
  ctrl_makePositive_ = getctrl("mrs_bool/makePositive");
  ctrl_setr0to1_ = getctrl("mrs_bool/setr0to1");
  ctrl_setr0to0_ = getctrl("mrs_bool/setr0to0");
  ctrl_lowCutoff_ = getctrl("mrs_real/lowCutoff");
  ctrl_highCutoff_ = getctrl("mrs_real/highCutoff");
}

void
AutoCorrelation::addControls()
{
  addctrl("mrs_real/magcompress", 2.0, ctrl_magcompress_);
  addctrl("mrs_natural/normalize", 0, ctrl_normalize_);
  addctrl("mrs_real/octaveCost", 0.0, ctrl_octaveCost_);
  addctrl("mrs_real/voicingThreshold", 0.1, ctrl_voicingThreshold_);
  addctrl("mrs_bool/aliasedOutput", false, ctrl_aliasedOutput_);
  addctrl("mrs_bool/makePositive", false, ctrl_makePositive_);
  addctrl("mrs_bool/setr0to1", false, ctrl_setr0to1_);
  addctrl("mrs_bool/setr0to0", true, ctrl_setr0to0_);
  addctrl("mrs_real/lowCutoff", 0.0, ctrl_lowCutoff_);
  addctrl("mrs_real/highCutoff", 1.0, ctrl_highCutoff_);


  ctrl_normalize_->setState(true);
  ctrl_octaveCost_->setState(true);
  ctrl_voicingThreshold_->setState(true);
  ctrl_aliasedOutput_->setState(true);
  ctrl_lowCutoff_->setState(true);
  ctrl_highCutoff_->setState(true);
}

MarSystem*
AutoCorrelation::clone() const
{
  return new AutoCorrelation(*this);
}

void
AutoCorrelation::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  if(!myfft_)
    myfft_ = new fft();


  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  // round up with ceil()
  lowSamples_ = (mrs_natural) ceil(inSamples_
                                   * getctrl("mrs_real/lowCutoff")->to<mrs_real>());
  numSamples_ = (mrs_natural) ceil( inSamples_
                                    * getctrl("mrs_real/highCutoff")->to<mrs_real>()
                                  ) - lowSamples_;

  if(ctrl_aliasedOutput_->to<mrs_bool>())
    fftSize_ = inSamples_; //will create aliasing!
  else
  {
    //compute fft with a size of the next power of 2 of 2*inSamples-1 samples
    fftSize_ = (mrs_natural)pow(2.0, ceil(log(2.0*numSamples_-1.0)/log(2.0)));
  }

  scratch_.create(fftSize_);


  // only working for hanning window
  normalize_ = 0;
  if(getctrl("mrs_natural/normalize")->to<mrs_natural>())
  {
    cout << "NORM INIT" << endl;
    realvec tmp(getctrl("mrs_natural/onSamples")->to<mrs_natural>());
    normalize_ = 1;
    norm_.create(getctrl("mrs_natural/onSamples")->to<mrs_natural>());
    norm_.setval(1);
    Windowing win("Windowing");
    win.updControl("mrs_string/type", "Hanning");
    win.updControl("mrs_natural/inSamples", norm_.getCols());
    win.updControl("mrs_natural/inObservations", norm_.getRows());
    win.process(norm_, tmp);

    AutoCorrelation autocorr("Autocorrelation");
    autocorr.updControl("mrs_natural/inSamples", norm_.getCols());
    autocorr.updControl("mrs_natural/inObservations", norm_.getRows());
    autocorr.update();
    autocorr.process(tmp, norm_);

    for (mrs_natural i = 0 ; i < norm_.getSize() ; ++i)
      norm_(i) = 1/norm_(i);
  }

  octaveCost_ = getctrl("mrs_real/octaveCost")->to<mrs_real>();
  voicing_ = getctrl("mrs_real/voicingThreshold")->to<mrs_real>();
  if(octaveCost_)
  {
    octaveCost_ *= octaveCost_;
    octaveMax_ = octaveCost_*log(36.0*inSamples_);
  }
}

void
AutoCorrelation::myProcess(realvec& in, realvec& out)
{





  mrs_natural t,o;
  k_ = ctrl_magcompress_->to<mrs_real>();

  // Copy to output to perform inplace fft and zeropad to double size

  scratch_.create(fftSize_); //scratch_ needs to be reset every time
  for (o=0; o < inObservations_; o++)
  {
    for (t=lowSamples_; t < (lowSamples_+numSamples_); t++)
    {
      scratch_(t-(lowSamples_)) = in(o,t);
    }





    //zeropad
    for(t=(lowSamples_+numSamples_); t < fftSize_; t++)
      scratch_(t) = 0.0;


    //get pointer to data (THIS BREAKS ENCAPSULATION! FIXME [!])
    mrs_real *tmp = scratch_.getData();

    //compute forward FFT (of size fftSize_)
    myfft_->rfft(tmp, fftSize_/2, FFT_FORWARD); //rfft() takes as second argument half of the desired FFT size (see fft.cpp)

    // Special case for zero and Nyquist/2,
    // which only have real part
    if (k_ == 2.0)
    {
      re_ = tmp[0];
      tmp[0] = re_ * re_;
      re_ = tmp[1];
      tmp[1] = re_ * re_;
    }
    else
    {
      re_ = tmp[0];
      re_ = sqrt(re_ * re_);
      tmp[0] = pow(re_, k_);
      re_ = tmp[1];
      re_ = sqrt(re_ * re_);
      tmp[1] = pow(re_, k_);
    }

    // Compress the magnitude spectrum and zero
    // the imaginary part.
    for (t=1; t < fftSize_/2; t++)
    {
      re_ = tmp[2*t];
      im_ = tmp[2*t+1];
      if (k_ == 2.0)
        am_ = re_ * re_ + im_ * im_;
      else
      {
        am_ = sqrt(re_ * re_ + im_ * im_);
        am_ = pow(am_, k_);
      }
      tmp[2*t] = am_;
      tmp[2*t+1] = 0;
    }

    // Take the inverse Fourier Transform (of size fftSize_)
    myfft_->rfft(tmp, fftSize_/2, FFT_INVERSE);

    // Copy result to output
    if(normalize_)
    {
      cout << "NORM Normalization happening" << endl;
      for (t=0; t < onSamples_; t++)
      {
        out(o,t) = scratch_(t)*norm_(t);
      }
    }
    else
      for (t=0; t < onSamples_; t++)
      {
        // out(o,t) = 0.1 * scratch_(t) + 0.99 * out(o,t);
        out(o,t) = 1.0 * scratch_(t) + 0.0 * out(o,t);
        // out(o,t) = 0.5 * scratch_(t) + 0.5 * out(o,t);
        // out(o,t) +=  scratch_(t);

      }

  }


  if (ctrl_makePositive_->to<mrs_bool>())
  {
    out -= out.minval();
  }

  if(octaveCost_) //is there a reference for this octaveCost computation [?]
  {
    for (o=0; o < inObservations_; o++)
    {
      mrs_real maxOut = 0;
      for (t=1 ; t<onSamples_/2 ; t++)
        if (out(o, t)> out(o, t+1) && out(o, t) > out(o, t-1) && out(o, t)>maxOut)
          maxOut = out(o, t) ;
      //cout << maxOut/out(o, 0)<< " " << 1+voicing_ << << endl;

      if(maxOut && maxOut/out(o, 0) > 1-voicing_)
        for (t=1; t < onSamples_; t++)
          out(o, t) += octaveMax_-octaveCost_*log(36.0*t);
      else
        out.setval(0);
    }
  }

  if (ctrl_setr0to1_->to<mrs_bool>())
  {
    // out -= out.minval();

    /* for (o=0; o < onObservations_; o++)
      for (t=0; t< onSamples_-1; t++)
    {
    out(o,t) = out(o,t) / (onSamples_ - 1 - t);
    if (t > onSamples_-1-100)
      out(o,t) = 0.0;
    }
    */



    // mrs_real myNorm = out(0,0);
    // if (myNorm > 0)
    // out	/= myNorm;
  }





  if (ctrl_setr0to0_->to<mrs_bool>())
  {

    // for (o=0; o < onObservations_; o++)
    // out(o,0) = 0.0;



    for (o=0; o < onObservations_; o++)
      for (t=0; t < onSamples_; t++)
      {
        out(o,t) = out(o,t);
      }
  }

  /*
  MATLAB_PUT(in, "corr_in");
  MATLAB_PUT(out, "corr");

  MATLAB_EVAL("subplot(211)");
  MATLAB_EVAL("plot(corr_in)");
  MATLAB_EVAL("subplot(212)");
  MATLAB_EVAL("plot(corr)");
  */
}
