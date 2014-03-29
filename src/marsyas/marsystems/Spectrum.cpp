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


#include "Spectrum.h"
#include <cmath>

using namespace std;
using namespace Marsyas;

template<typename NUM>
static bool is_power_of_two(NUM n)
{
  double real_power_of_two = std::log( (double) n ) / std::log(2.0);
  int int_power_of_two = (int) real_power_of_two;
  return real_power_of_two == int_power_of_two;
}

Spectrum::Spectrum(mrs_string name):
  MarSystem("Spectrum",name),
  cutoff_(1.0),
  lowcutoff_(0.0),
  re_(0.0),
  im_ (0.0),
  ponObservations_(0),
  correct_input_format_(false)
{
  addControls();
  checkInputFormat();
}

Spectrum::~Spectrum()
{
}

Spectrum::Spectrum(const Spectrum& a):
  MarSystem(a),
  cutoff_(1.0),
  lowcutoff_(0.0),
  re_(0.0),
  im_ (0.0),
  ponObservations_(0),
  correct_input_format_(false)
{
  ctrl_cutoff_ = getctrl("mrs_real/cutoff");
  ctrl_lowcutoff_ = getctrl("mrs_real/lowcutoff");
  checkInputFormat();
}

void
Spectrum::addControls()
{
  addctrl("mrs_real/cutoff", 1.0, ctrl_cutoff_);
  setctrlState("mrs_real/cutoff", true);
  addctrl("mrs_real/lowcutoff", 0.0, ctrl_lowcutoff_);
  setctrlState("mrs_real/lowcutoff", true);
}

MarSystem*
Spectrum::clone() const
{
  return new Spectrum(*this);
}

void Spectrum::checkInputFormat()
{
  if (inSamples_ == 0)
    return;

  correct_input_format_ = is_power_of_two(inSamples_);
  if (!correct_input_format_)
  {
    MRSERR("Spectrum: input amount of samples not power of two: " << inSamples_);
  }
}

void
Spectrum::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onSamples_->setValue((mrs_natural)1, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>() / ctrl_inSamples_->to<mrs_natural>());

  cutoff_ = ctrl_cutoff_->to<mrs_real>();
  lowcutoff_ = ctrl_lowcutoff_->to<mrs_real>();

  onObservations_ = ctrl_onObservations_->to<mrs_natural>();

  if (ponObservations_ != onObservations_)
  {
    checkInputFormat();

    /* ostringstream oss;
      oss << "rbin_0" << ","; //DC bin (only has real part)
      oss << "rbin_" << onObservations_/2 << ","; //Nyquist bin (only has real part)
      for (mrs_natural n=2; n < onObservations_/2; n++)
      {
      	oss << "rbin_" << n-1 << ",";
      	oss << "ibin_" << n-1 << ",";
      }
      ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);
    */
  }


  // gtzan: output gets to cluttered if we annotate every bin of the
  // spectrum - commented out detailed output

  // Set the observation names by prefixing it with FFTxxx,
  // based on the first item of inObsNames_, because we ignore the
  // other observation channels.
  // mrs_string inObsName = stringSplit(ctrl_inObsNames_->to<mrs_string>(), ",")[0];
  ostringstream oss;

  // for (mrs_natural i = 0; i < inSamples_; ++i)
  // {
  // oss << "FFT" << inSamples_
  // \todo: shouldn't there be some sort of index included in the obsName? like
  // << "i" << i
  // << "_" << inObsName << ",";
  // }

  // 	ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  ctrl_onObsNames_->setValue("FFT" + oss.str() + "_" + ctrl_inObsNames_->to<mrs_string>(), NOUPDATE);




  ponObservations_ = onObservations_;
}

void
Spectrum::myProcess(realvec& in, realvec& out)
{
  if (!correct_input_format_)
    return;

  mrs_natural t;
  // copy to output to perform inplace fft
  // notice transposition of matrix
  // from row to column
  for (t=0; t < inSamples_; t++)
  {
    out(t,0) = in(0,t);
  }

  mrs_real *tmp = out.getData();
  myfft_.rfft(tmp, inSamples_/2, FFT_FORWARD);

  if (cutoff_ != 1.0)
  {
    for (t= (mrs_natural)((cutoff_ * inSamples_) / 2); t < inSamples_/2; t++)
    {
      out(2*t,0) = 0;
      out(2*t+1,0) = 0;
    }
  }

  if (lowcutoff_ != 0.0)
  {
    for (t=0; t < (mrs_natural)((lowcutoff_ * inSamples_) /2); t++)
    {
      out(2*t,0) = 0;
      out(2*t+1,0) = 0;
    }
  }

  //compare with matlab fft
  //	 MATLAB_PUT(in, "vec");
  //	 MATLAB_EVAL("out=fft(vec);");
  //   MATLAB_GET("vec", out);

  return;
}
