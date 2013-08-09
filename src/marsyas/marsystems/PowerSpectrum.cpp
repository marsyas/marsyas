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

#include "PowerSpectrum.h"

using std::ostringstream;
using namespace Marsyas;

#define PSD_POWER 1
#define PSD_MAG 2
#define PSD_DB  3
#define PSD_WDB  4
#define PSD_PD  5
#define PSD_LOGMAG 6
#define PSD_LOGMAG2 7

PowerSpectrum::PowerSpectrum(mrs_string name):MarSystem("PowerSpectrum",name)
{
  ntype_ = PSD_POWER;
  N2_ = 0;
  re_ = 0.0;
  im_ = 0.0;
  dB_ = 0.0;
  pwr_ = 0.0;

  addControls();
}

PowerSpectrum::PowerSpectrum(const PowerSpectrum& a):MarSystem(a)
{
  ctrl_spectrumType_ = getctrl("mrs_string/spectrumType");
}


PowerSpectrum::~PowerSpectrum()
{
}

void
PowerSpectrum::addControls()
{
  addctrl("mrs_string/spectrumType", "power", ctrl_spectrumType_);
  setctrlState("mrs_string/spectrumType", true);
}

MarSystem*
PowerSpectrum::clone() const
{
  return new PowerSpectrum(*this);
}

void
PowerSpectrum::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  //Spectrum outputs N values, corresponding to N/2+1
  //complex and unique spectrum points - see Spectrum.h documentation
  N2_ = ctrl_inObservations_->to<mrs_natural>()/2 + 1;

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(N2_, NOUPDATE); //outputs N/2+1 real values
  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>(), NOUPDATE);

  stype_ = ctrl_spectrumType_->to<mrs_string>();
  if (stype_ == "power")
    ntype_ = PSD_POWER;
  else if (stype_ == "magnitude")
    ntype_ = PSD_MAG;
  else if (stype_ == "decibels")
    ntype_ = PSD_DB; //PUT DB!! (DB->NEW)
  else if (stype_ == "wrongdBonsets")
    ntype_ = PSD_WDB;
  else if (stype_ == "powerdensity")
    ntype_ = PSD_PD;
  else if (stype_ == "logmagnitude")
    ntype_ = PSD_LOGMAG;
  else if (stype_ == "logmagnitude2")
    ntype_ = PSD_LOGMAG2;

  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  ctrl_onObsNames_->setValue("Power_" + stype_ + ctrl_inObsNames_->to<mrs_string>(), NOUPDATE);

  // gtzan: removed cluttered output
  // ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "Power" + stype_ + "_"), NOUPDATE);
}

void
PowerSpectrum::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  for (t=0; t < inSamples_; ++t)
  {
    for (o=0; o < N2_; o++)
    {
      if (o==0) //DC bin (i.e. 0)
      {
        re_ = in(0,t);
        im_ = 0.0;
      }
      else if (o == N2_-1) //Nyquist bin (i.e. N/2)
      {
        re_ = in(1,t);
        im_ = 0.0;
      }
      else //all other bins
      {
        re_ = in(2*o, t);
        im_ = in(2*o+1, t);
      }

      switch (ntype_)
      {
      case PSD_POWER:
        out(o, t) = re_*re_ + im_*im_;
        break;
      case PSD_MAG:
        out(o,t) = sqrt(re_ * re_ + im_ * im_);
        break;
      case PSD_DB:
        // TODO(sness) - Check validity of this magic number.  Should probably be FLT_MIN
        dB_ = (mrs_real)(10*log10(re_ * re_ + im_ * im_ + 0.000000001));
        out(o,t) = dB_;
        break;
      case PSD_WDB:
        // NOTE : FIXME!!!
        // 20*log10() seems to work better for toy_with_onsets
        // This is not good, and should be fixed inside the code that
        // looks for onsets.
        dB_ = (mrs_real)(20*log10(re_ * re_ + im_ * im_ + 0.000000001));
        if (dB_ < -100)	dB_ = -100;
        out(o,t) = dB_;
        break;
      case PSD_PD:
        pwr_ = re_ * re_ + im_ * im_;
        out(o,t) = (mrs_real)(2.0 * pwr_) / N2_;
        break;
      case PSD_LOGMAG2:
        out(o,t) = (mrs_real)log10(1+sqrt(re_ * re_ + im_ * im_));
        break;
      case PSD_LOGMAG:
        out(o,t) = log(1+1000.0 * sqrt(re_ * re_ + im_ * im_));
        //  out(o,t) = pow(sqrt(re_ * re_ + im_ * im_), 0.5);
        // out(o,t) = asin(sqrt(re_ * re_ + im_ * im_));

      }
    }
  }

  //MATLAB_PUT(out, "PowerSpectrum");
  //MATLAB_EVAL("plot(PowerSpectrum)");
}
