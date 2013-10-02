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

#include "PvMultiResolution.h"
#include "../common_source.h"

using std::ostringstream;
using namespace Marsyas;

PvMultiResolution::PvMultiResolution(mrs_string name):MarSystem("PvMultiResolution", name)
{
  flux_ = new Flux("flux");
  r_ = 0.1;
  m_ = 0.75;

  addControls();
}

PvMultiResolution::PvMultiResolution(const PvMultiResolution& a) : MarSystem(a)
{
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_transient_ = getctrl("mrs_bool/transient");
  ctrl_shortmag_ = getctrl("mrs_realvec/shortmag");
  ctrl_longmag_ = getctrl("mrs_realvec/longmag");

  r_ = 0.1;
  m_ = 0.75;
  flux_ = new Flux("flux");
}


PvMultiResolution::~PvMultiResolution()
{
  delete flux_;
}

MarSystem*
PvMultiResolution::clone() const
{
  return new PvMultiResolution(*this);
}

void
PvMultiResolution::addControls()
{

  addctrl("mrs_string/mode", "long", ctrl_mode_);
  addctrl("mrs_bool/transient", false, ctrl_transient_);
  addctrl("mrs_realvec/shortmag", realvec(), ctrl_shortmag_);
  addctrl("mrs_realvec/longmag", realvec(), ctrl_longmag_);
  addctrl("mrs_real/flux", 0.0);
}

void
PvMultiResolution::myUpdate(MarControlPtr sender)
{
  MRSDIAG("PvMultiResolution.cpp - PvMultiResolution:myUpdate");

  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>() / 2, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  median_buffer_.create(10);
  mbindex_ = 0;


  powerSpectrum_.create(onObservations_/2,1);
  whiteSpectrum_.create(onObservations_/2,1);

  {
    MarControlAccessor acc(ctrl_shortmag_);
    mrs_realvec& shortmag = acc.to<mrs_realvec>();
    shortmag.create(onObservations_/2);
  }

  {
    MarControlAccessor acc(ctrl_longmag_);
    mrs_realvec& longmag = acc.to<mrs_realvec>();
    longmag.create(onObservations_/2);
  }


  flux_->updControl("mrs_natural/inSamples", 1);
  flux_->updControl("mrs_natural/inObservations", onObservations_/2);
  flux_->updControl("mrs_real/israte", 44100);
  flux_->updControl("mrs_string/mode", "DixonDAFX06");
  fluxval_.create(1,1);

}

void
PvMultiResolution::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  const mrs_string& mode = ctrl_mode_->to<mrs_string>();


  MarControlAccessor acc1(ctrl_shortmag_);
  mrs_realvec& shortmag = acc1.to<mrs_realvec>();

  MarControlAccessor acc2(ctrl_longmag_);
  mrs_realvec& longmag = acc2.to<mrs_realvec>();




  if (mode == "short")
  {
    // short window
    for (o=0; o < inObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) = in(o, t);
      }
    for (o=0; o < onObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {

        out(2*o, t) = 0.75 * out(2*o,t);
      }


  }
  else if (mode == "long")
  {

    // long window
    for (o=inObservations_/2; o < inObservations_; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o-inObservations_/2,t) = in(o,t);
      }

    for (o=0; o < onObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(2*o, t) = out(2*o,t);
      }
  }
  else if (mode == "shortlong_mixture")
  {


    for (o=0; o < inObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) = in(o, t);
      }

    /* use long window for frequencies lower than approx. 2000 Hz
     and short window for higher frequencies */
    for (o=inObservations_/2; o < inObservations_/2 + 200; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o-inObservations_/2,t) = in(o,t);
      }

    for (o=0; o < 200; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(2*o, t) = 2 * out(2*o,t);
      }
  }

  else if (mode == "transient_switch")
  {

    // short window
    for (o=0; o < inObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {
        out(o,t) = in(o, t);
      }


    /* calculate power and use median for dynamic thresholding */
    for (o=0; o < onObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {
        powerSpectrum_(o,0) = out(2*o,t) * out(2*o,t);
      }


    // adaptive pre-whitening
    for (o=0; o < onObservations_/2; o++)
    {

      if (powerSpectrum_(o,0) < r_)
        whiteSpectrum_(o,0) = r_;
      else
      {
        if (m_ * whiteSpectrum_(o,0) > powerSpectrum_(o,0))
          whiteSpectrum_(o,0) = m_ * whiteSpectrum_(o,0);
        else
          whiteSpectrum_(o,0) = powerSpectrum_(o,0);
      }
      powerSpectrum_(o,0) = powerSpectrum_(o,0) / whiteSpectrum_(o,0);
    }



    flux_->process(powerSpectrum_, fluxval_);

    median_buffer_(mbindex_) = fluxval_(0,0);
    mbindex_++;
    if (mbindex_ == 10)
    {
      mbindex_ = 0;
    }


    updControl("mrs_real/flux", fluxval_(0,0) - median_buffer_.median());

    mrs_real longSum = 0.0;
    mrs_real shortSum = 0.0;

    mrs_real ratio1;
    //mrs_real ratio2;



    for (o=0; o < onObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {
        shortmag(o) = in(2*o,t);
        longmag(o) = in(2*o + inObservations_/2, t);
        shortSum += shortmag(o);
        longSum += longmag(o);
      }


    ratio1 = longSum / shortSum;
    //ratio2 = shortSum / longSum;


    for (o=0; o < onObservations_/2; o++)
      for (t = 0; t < inSamples_; t++)
      {
        shortmag(o) = ratio1 * shortmag(o);
      }






    if (fluxval_(0,0) - median_buffer_.median() <= 0.0000010)    // steady state use long window
    {

      // use long
      for (o=inObservations_/2; o < inObservations_; o++)
        for (t = 0; t < inSamples_; t++)
        {
          out(o-inObservations_/2,t) = in(o,t);
        }

      for (o=0; o < onObservations_/2; o++)
        for (t = 0; t < inSamples_; t++)
        {
          out(2*o, t) = out(2*o,t);
        }

      ctrl_transient_->setValue(false, NOUPDATE);
    }
    else // transient
    {
      // use short
      for (o=0; o < inObservations_/2; o++)
        for (t = 0; t < inSamples_; t++)
        {
          out(o,t) = in(o, t);
        }

      for (o=0; o < onObservations_/2; o++)
        for (t = 0; t < inSamples_; t++)
        {
          out(2*o, t) = ratio1 * out(2*o,t);
        }



      ctrl_transient_->setValue(true, NOUPDATE);
      // cout<< fluxval_(0,0)-median_buffer_.median() << endl;
    }
    // cout << "RATIO = " << ratio << endl;
  }






}
