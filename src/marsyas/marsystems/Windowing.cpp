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

#include "Windowing.h"
#include <stdexcept>

using namespace std;
using namespace Marsyas;

Windowing::Windowing(mrs_string name):MarSystem("Windowing",name)
{
  zeroPadding_ = 0;
  size_ = 0;
  addcontrols();
}

Windowing::Windowing(const Windowing& a):MarSystem(a)
{
  ctrl_type_ = getctrl("mrs_string/type");
  ctrl_zeroPhasing_ = getctrl("mrs_bool/zeroPhasing");
  ctrl_zeroPadding_ = getctrl("mrs_natural/zeroPadding");
  ctrl_size_ = getctrl("mrs_natural/size");
  ctrl_variance_ = getctrl("mrs_real/variance");
  ctrl_normalize_ = getctrl("mrs_bool/normalize");
  zeroPadding_ = 0;
  size_ = 0;
}

Windowing::~Windowing()
{
}

MarSystem*
Windowing::clone() const
{
  return new Windowing(*this);
}

void
Windowing::addcontrols()
{
  addctrl("mrs_string/type", "Hamming", ctrl_type_);
  addctrl("mrs_bool/zeroPhasing", false, ctrl_zeroPhasing_);
  addctrl("mrs_natural/zeroPadding", 0, ctrl_zeroPadding_);
  addctrl("mrs_natural/size", 0, ctrl_size_);
  addctrl("mrs_real/variance", 0.4, ctrl_variance_);// used for the gaussian window
  addctrl("mrs_bool/normalize", false, ctrl_normalize_);

  setctrlState("mrs_string/type", true);
  setctrlState("mrs_bool/zeroPhasing", true);
  setctrlState("mrs_natural/zeroPadding", true);
  setctrlState("mrs_natural/size", true);
  setctrlState("mrs_real/variance", true);
  setctrlState("mrs_bool/normalize", true);
}



void
Windowing::myUpdate(MarControlPtr sender)
{
  mrs_natural t;
  (void) sender;  //suppress warning of unused parameter(s)
  mrs_string type = ctrl_type_->to<mrs_string>();

  ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);

  // Add prefix to the observation names.
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  mrs_string prefix = mrs_string("Win") + type + mrs_string("_");
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, prefix), NOUPDATE);

  //if zeroPadding control changed...
  if (ctrl_zeroPadding_->to<mrs_natural>() != onSamples_- inSamples_)
  {
    //zero padding should always be a positive or zero value
    if (ctrl_zeroPadding_->to<mrs_natural>() < 0)
    {
      ctrl_zeroPadding_->setValue(0, NOUPDATE);
    }
    ctrl_size_->setValue(ctrl_inSamples_->to<mrs_natural>() +
                         ctrl_zeroPadding_->to<mrs_natural>(), NOUPDATE);
    onSamples_ = ctrl_size_->to<mrs_natural>();
  }
  //if size control changed...
  if (ctrl_size_->to<mrs_natural>() != onSamples_)
  {
    //size should never be smaller than inSamples
    if (ctrl_size_->to<mrs_natural>() < inSamples_)
    {
      ctrl_size_->setValue(inSamples_, NOUPDATE);
    }
    ctrl_zeroPadding_->setValue(ctrl_size_->to<mrs_natural>() -
                                ctrl_inSamples_->to<mrs_natural>(), NOUPDATE);
  }

  ctrl_onSamples_->setValue(ctrl_size_, NOUPDATE);

  //check if zero phasing should be performed
  if (ctrl_zeroPhasing_->isTrue())
  {
    delta_ = inSamples_/2+1;
  }
  else
  {
    delta_=0;
  }


  tmp_.create(inSamples_);

  // Precalculate the envelope.
  // \todo only do this when the type or size is changed.
  envelope_.create(inSamples_);

  if (type == "Hamming")
  {
    windowingFillHamming(envelope_);
  }
  else if (type == "Hanning" || type == "Hann")
  {
    windowingFillHanning(envelope_);
  }
  else if (type == "Triangle")
  {
    windowingFillTriangle(envelope_);
  }
  else if (type == "Bartlett")
  {
    windowingFillBartlett(envelope_);
  }
  else if (type == "Gaussian")
  {
    windowingFillGaussian(envelope_, ctrl_variance_->to<mrs_real>());
  }
  else if (type == "Blackman")
  {
    windowingFillBlackman(envelope_, 0.16);
  }
  else if (type == "Blackman-Harris")
  {
    windowingFillBlackmanHarris(envelope_);
  }
  else if (type == "Cosine" || type == "Sine")
  {
    windowingFillCosine(envelope_);
  }
  else
  {
    ostringstream oss;
    oss << "Invalid windowing type \"" << type << "\"";
    throw invalid_argument(oss.str());
  }



  if (ctrl_normalize_->to<mrs_bool>() == true)
  {
    mrs_real sum = 0.0;

    for (t =0; t < inSamples_; t++)
    {
      sum += envelope_(t);
    }
    mrs_real afac = (mrs_real)(2.0 /sum); // \todo: why is there a factor 2 here?
    envelope_ *= afac;
    ctrl_normalize_->setValue(false, NOUPDATE);
  }


}

void
Windowing::myProcess(realvec& in, realvec& out)
{
  out.setval(0.0);
  mrs_natural o,t;

  for (o=0; o < inObservations_; o++)
  {
    //shift windowed data in case zeroPhasing is selected
    if (ctrl_zeroPhasing_->isTrue())
    {
      //apply the window to the input data
      for (t = 0; t < inSamples_; t++)
      {
        tmp_(t) =  in(o,t)*envelope_(t); // /(norm_);
      }
      for (t = 0; t < inSamples_/2; t++)
      {
        out(o,t)=tmp_((t+delta_)%inSamples_);
      }
      for (t = inSamples_/2; t < inSamples_; t++)
      {
        out(o,t+(onSamples_-inSamples_))=tmp_((t+delta_)%inSamples_);
      }
    }
    else
    {
      for (t=0; t< inSamples_; ++t)
      {
        out(o,t) = in(o,t) * envelope_(t);
      }
    }
  }
}



/**
 * \ingroup Basic
 *
 * A raised cosine window \f$ w(t) \f$ is of the form
 * \f[ w(t) = \alpha + \beta \cos \left( \frac{2 \pi t}{N-1} \right) \f]
 * windowingFillRaisedCosine() fills a given realvec with the values
 * \f$ w(t) \f$ for increasing values of \f$ t \f$ (note that the realvec
 * is handled as a one dimensional array). The \f$ N \f$ is determined
 * by the size of the realvec.
 *
 * \param envelope a pre-allocated realvec
 * \param alpha the \f$ \alpha \f$ from the raised cosine formula.
 * \param beta the \f$ \beta \f$ from the raised cosine formula.
 *
 * \see windowingFillHamming(), windowingFillHanning(), \see Windowing
 */
void
Marsyas::windowingFillRaisedCosine(realvec& envelope, mrs_real alpha, mrs_real beta)
{
  mrs_natural N = envelope.getSize();
  for (mrs_natural t = 0; t < N; t++)
  {
    envelope(t) = alpha - beta * cos(2.0 * PI * t / (N - 1.0));
  }
}





/**
 * \ingroup Basic
 *
 * \f[ w(n)=\frac{2}{N}\cdot\left(\frac{N}{2}-\left |n-\frac{N-1}{2}\right |\right) \f]
 *
 * \see windowingFillRaisedCosine(), \see windowingFillBartlett(), \see Windowing
 */
void
Marsyas::windowingFillTriangle(realvec& envelope)
{
  mrs_natural N = envelope.getSize();
  for (mrs_natural t = 0; t < N; t++)
  {
    envelope(t) = 2.0/N * (N/2.0 - abs(t - (N - 1.0)/2.0));
  }
}

/**
 * \ingroup Basic
 *
 * \f[ w(n) = \frac{2}{N-1}\cdot\left(\frac{N-1}{2}-\left |n-\frac{N-1}{2}\right |\right) \f]
 *
 * \see windowingFillRaisedCosine(), \see windowingFillTriangle(), \see Windowing
 */
void
Marsyas::windowingFillBartlett(realvec& envelope)
{
  mrs_natural N = envelope.getSize();
  for (mrs_natural t = 0; t < N; t++)
  {
    envelope(t) = 2.0/(N-1.0) * ((N-1.0)/2.0 - abs(t - (N - 1.0)/2.0));
  }
}

/**
 * \ingroup Basic
 *
 * \f[ w(n) = \exp \left( -\frac{1}{2} \left( \frac{n-(N-1)/2}{\sigma (N-1)/2} \right)^{2} \right) \f]
 *
 * \see windowingFillRaisedCosine(), \see Windowing
 */
void Marsyas::windowingFillGaussian(realvec& envelope, mrs_real sigma)
{
  mrs_natural N = envelope.getSize();
  mrs_real tmp;
  for (mrs_natural t = 0; t < N; t++)
  {
    tmp = (t - (N - 1.0) / 2.0) / (sigma * (N - 1.0) / 2.0);
    envelope(t) = exp(-0.5*tmp*tmp);
  }
}

/**
 * \ingroup Basic
 * \f[ w(n) = a_0 - a_1 \cos \left ( \frac{2 \pi n}{N-1} \right) + a_2 \cos \left ( \frac{4 \pi n}{N-1} \right) \f]
 * with \f$ a_0=\frac{1-\alpha}{2} \f$, \f$ a_1=\frac{1}{2} \f$ and \f$ a_2=\frac{\alpha}{2}\f$
 *
 * \see windowingFillRaisedCosine(), \see windowingFillBlackmanHarris(), \see Windowing
 */
void Marsyas::windowingFillBlackman(realvec& envelope, mrs_real alpha)
{
  mrs_natural N = envelope.getSize();
  mrs_real a0 = (1.0 - alpha) / 2.0;
  mrs_real a2 = alpha / 2.0;
  for (mrs_natural t = 0; t < N; t++)
  {
    envelope(t) = a0  - 0.5 * cos(2.0 * PI * t / (N - 1.0))
                  + a2 * cos(4.0 * PI * t / (N - 1.0));
  }
}

/**
 * \ingroup Basic
 *
 * \f[ w(n) = a_0 - a_1 \cos \left ( \frac{2 \pi n}{N-1} \right)+ a_2 \cos \left ( \frac{4 \pi n}{N-1} \right)- a_3 \cos \left ( \frac{6 \pi n}{N-1} \right) \f]
 * with  \f$ a_0=0.35875 \f$,  \f$ a_1=0.48829 \f$,  \f$ a_2=0.14128 \f$ and \f$ a_3=0.01168 \f$
 *
 * \see windowingFillRaisedCosine(), \see windowingFillBlackman(), \see Windowing
 */
void Marsyas::windowingFillBlackmanHarris(realvec& envelope)
{
  mrs_natural N = envelope.getSize();
  mrs_real a0 = 0.35875, a1 = 0.48829, a2 = 0.14128, a3 = 0.01168;
  for (mrs_natural t = 0; t < N; t++)
  {
    envelope(t) = a0  - a1 * cos(2.0 * PI * t / (N - 1.0))
                  + a2 * cos(4.0 * PI * t / (N - 1.0))
                  - a3 * cos(6.0 * PI * t / (N - 1.0));
  }
}

/**
 * \ingroup Basic
 *
 * \f[ w(n) = \sin \left( \pi \frac{n}{N-1} \right)\f]
 *
 * \see windowingFillRaisedCosine(), \see Windowing
 */
void Marsyas::windowingFillCosine(realvec& envelope)
{
  mrs_natural N = envelope.getSize();
  for (mrs_natural t = 0; t < N; t++)
  {
    envelope(t) = sin(PI * t / (N - 1.0));
  }
}
