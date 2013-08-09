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


#include "PvConvert.h"
#include <algorithm>
#include <functional>



using std::ostringstream;
using std::greater;
using std::sort;

using namespace Marsyas;

PvConvert::PvConvert(mrs_string name):MarSystem("PvConvert",name)
{
  //type_ = "PvConvert";
  //name_ = name;

  psize_ = 0;
  size_ = 0;

  addControls();
}

PvConvert::PvConvert(const PvConvert& a):MarSystem(a)
{
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_phases_ = getctrl("mrs_realvec/phases");
  ctrl_regions_ = getctrl("mrs_realvec/regions");

  psize_ = 0;
}



PvConvert::~PvConvert()
{
}

MarSystem*
PvConvert::clone() const
{
  return new PvConvert(*this);
}


void
PvConvert::addControls()
{
  addctrl("mrs_natural/Decimation",MRS_DEFAULT_SLICE_NSAMPLES/4);
  addctrl("mrs_natural/Sinusoids", 1);
  setctrlState("mrs_natural/Sinusoids", true);
  addctrl("mrs_string/mode", "sorted", ctrl_mode_);
  addctrl("mrs_realvec/phases", realvec(), ctrl_phases_);
  addctrl("mrs_realvec/regions", realvec(), ctrl_regions_);


}

void
PvConvert::myUpdate(MarControlPtr sender)
{

  (void) sender;  //suppress warning of unused parameter(s)
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>() + 2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte")->to<mrs_real>() * getctrl("mrs_natural/inObservations")->to<mrs_natural>());

  //defaultUpdate(); [!]
  onObservations_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();

  size_ = onObservations_ /2 +1;

  if (size_ != psize_)
  {
    lastphase_.stretch(size_);
    // phase_.stretch(size_);
    MarControlAccessor acc(ctrl_phases_);
    realvec& phase = acc.to<mrs_realvec>();
    MarControlAccessor acc1(ctrl_regions_);
    realvec& regions = acc1.to<mrs_realvec>();

    phase.stretch(size_);
    regions.stretch(size_);
    mag_.stretch(size_);
    sortedmags_.stretch(size_);
    sortedpos_.stretch(size_);
  }

  psize_ = size_;

  factor_ = ((getctrl("mrs_real/osrate")->to<mrs_real>()) /
             (mrs_real)( getctrl("mrs_natural/Decimation")->to<mrs_natural>()* TWOPI));



  fundamental_ = (mrs_real) (getctrl("mrs_real/osrate")->to<mrs_real>() / (mrs_real)getctrl("mrs_natural/inObservations")->to<mrs_natural>());




  kmax_ = getctrl("mrs_natural/Sinusoids")->to<mrs_natural>();
}

void
PvConvert::myProcessFull(realvec& in, realvec& out)
{
  mrs_natural t;
  mrs_natural N2 = inObservations_/2;

  mrs_real a;
  mrs_real b;
  mrs_real phasediff;

  MarControlAccessor acc(ctrl_phases_);
  mrs_realvec& phases = acc.to<mrs_realvec>();

  MarControlAccessor acc1(ctrl_regions_);
  mrs_realvec& regions = acc1.to<mrs_realvec>();


  mrs_real decimation = getctrl("mrs_natural/Decimation")->to<mrs_natural>() * 1.0;
  mrs_real one_over_decimation = 1.0 / decimation;

  mrs_real omega_k;

  const mrs_string& mode = ctrl_mode_->to<mrs_string>();



  // handle amplitudes
  for (t=0; t <= N2; t++)
  {
    if (t==0)
    {
      a = in(0,0);
      b = 0.0;
    }
    else if (t == N2)
    {
      a = in(1, 0);
      b = 0.0;
    }
    else
    {
      a = in(2*t, 0);
      b = in(2*t+1, 0);
    }

    omega_k = (TWOPI * t) / (N2*2) ;

    // computer magnitude value
    out(2*t,0) = sqrt(a*a + b*b);

    if (out(2*t,0) == 0.0)
      phasediff = 0.0;
    else
    {
      phases(t) = -atan2(b,a);

      if (mode == "analysis_scaled_phaselock")
      {
        // scaled phase-locking
        phasediff = phases(t) - lastphase_((mrs_natural)regions(t)) - decimation * omega_k;
      }
      else
      {
        // classic, identity, loose phase_propagation
        phasediff = phases(t) - lastphase_(t) - decimation * omega_k;
      }

      lastphase_(t) = phases(t);

      while (phasediff > PI)
        phasediff -= TWOPI;
      while (phasediff < -PI)
        phasediff += TWOPI;
    }


    out(2*t+1, 0) = omega_k + one_over_decimation * phasediff;
  }
}



void
PvConvert::myProcess(realvec& in, realvec& out)
{


  const mrs_string& mode = ctrl_mode_->to<mrs_string>();
  if ((mode == "full")||(mode == "analysis_scaled_phaselock"))
    myProcessFull(in,out);
  else if (mode == "sorted")
    myProcessSorted(in,out);
  else if (mode == "neighbors")
    myProcessNeighbors(in,out);

}




void
PvConvert::myProcessSorted(realvec& in, realvec& out)
{
  mrs_natural c,t;

  MarControlAccessor acc(ctrl_phases_);
  mrs_realvec& phases = acc.to<mrs_realvec>();

  mrs_real decimation = getctrl("mrs_natural/Decimation")->to<mrs_natural>() * 1.0;

  mrs_real one_over_decimation = 1.0 / decimation;


  mrs_natural N2 = inObservations_/2;
  mrs_real a;
  mrs_real b;
  mrs_real phasediff;

  mrs_real omega_k;

  // handle amplitudes
  for (t=0; t <= N2; t++)
  {
    if (t==0)
    {
      a = in(2*t,0);
      b = 0.0;
    }
    else if (t == N2)
    {
      a = in(1, 0);
      b = 0.0;
    }
    else
    {
      a = in(2*t, 0);
      b = in(2*t+1, 0);
    }

    // computer magnitude value
    mag_(t) = sqrt(a*a + b*b);
    sortedmags_(t) = mag_(t);
    // compute phase
    phases(t) = -atan2(b,a);

  }

  mrs_real* data = sortedmags_.getData();
  sort(data, data+(N2+1), greater<mrs_real>());

  bool found;
  mrs_real val;


  mrs_real sum = 0;
  mrs_real sorted_sum = 0;

  for (t=0; t <= N2; t++)
    sum += mag_(t);


  int k = 0;

  for (t=0; t <= N2; t++)
  {
    found = false;
    val = mag_(t);


    for (c=0; c < kmax_; ++c)
    {

      if (val == sortedmags_(c))
      {
        sorted_sum += val;
        found = true;
        k++;
        break;
      }
    }



    out(2*t,0) = 0.0;
    out(2*t+1,0) = t * fundamental_;

    omega_k = (TWOPI * t) / (N2*2) ;

    phasediff = phases(t) - lastphase_(t) - decimation * omega_k;
    // phasediff = phases(t) - lastphase_(t);
    lastphase_(t) = phases(t);

    // phase unwrapping
    while (phasediff > PI)
      phasediff -= TWOPI;
    while (phasediff < -PI)
      phasediff += TWOPI;

    if (found)
    {
      if (val == 0.0)
        phasediff = 0.0;
      else
      {
        out(2*t,0) = val;
      }
      out(2*t+1, 0) = omega_k + one_over_decimation * phasediff;
    }
    else
    {
      out(2*t+1, 0) = omega_k + one_over_decimation * phasediff;
    }
  }




}

void
PvConvert::myProcessNeighbors(realvec& in, realvec& out)
{


  MarControlAccessor acc(ctrl_phases_);
  mrs_realvec& phases = acc.to<mrs_realvec>();

  mrs_natural t;
  mrs_natural N2 = inObservations_/2;
  mrs_real a;
  mrs_real b;
  mrs_real phasediff;

  // handle amplitudes
  for (t=0; t <= N2; t++)
  {
    if (t==0)
    {
      a = in(2*t,0);
      b = 0.0;
    }
    else if (t == N2)
    {
      a = in(1, 0);
      b = 0.0;
    }
    else
    {
      a = in(2*t, 0);
      b = in(2*t+1, 0);
    }

    // computer magnitude value
    mag_(t) = sqrt(a*a + b*b);
    sortedmags_(t) = mag_(t);
    // compute phase
    phases(t) = -atan2(b,a);


  }

  mrs_real* data = sortedmags_.getData();
  sort(data, data+(N2+1), greater<mrs_real>());

  mrs_real val;
  val = 0.0;

  for (t=0; t <= N2; t++)
  {

    phasediff = phases(t) - lastphase_(t);
    lastphase_(t) = phases(t);

    // phase unwrapping
    while (phasediff > PI)
      phasediff -= TWOPI;
    while (phasediff < -PI)
      phasediff += TWOPI;

    if ((t > 3) && (t <= N2-2))
    {
      if (
        (mag_(t) > mag_(t-1)) &&
        // (mag_(t) > mag_(t-2)) &&
        // (mag_(t) > mag_(t+2)) &&
        (mag_(t) > mag_(t+1))
      )

      {
        val = mag_(t);
      }
      else
        val = 0.0;
    }

    if (val == 0.0)
      phasediff = 0.0;

    out(2*t,0) = val;
    out(2*t+1, 0) = phasediff * factor_ + t * fundamental_;
  }
}


