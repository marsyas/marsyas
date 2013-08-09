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

#include "PeakRatio.h"

using std::cout;
using std::endl;

using std::ostringstream;

using namespace Marsyas;

/** Marsystem for incorporating the ratio of the higest peak to minimal/average peak of each observation samplewise to the output vector
 *
 *This Marsystem appends the ratio of the higest peak to minimal peak and ratio of the higest peak to average peak of each observation samplewise to the output vector.
 *
 *
 *
 *
 */
PeakRatio::PeakRatio(mrs_string name):MarSystem("PeakRatio", name)
{
  //Add any specific controls needed by PeakRatio
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  //addControls();

}

PeakRatio::PeakRatio(const PeakRatio& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work

}

PeakRatio::~PeakRatio()
{
}

MarSystem*
PeakRatio::clone() const
{
  return new PeakRatio(*this);
}

void
PeakRatio::addControls()
{
  //Add specific controls needed by this MarSystem.
  //none used for PeakRatio
}

void
PeakRatio::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();

  setControl("mrs_natural/onSamples",  (mrs_natural)1);
  setControl("mrs_natural/onObservations",  (mrs_natural)(ctrl_inObservations_->to<mrs_natural>()+2));

  // Add Names of The Observations to the observation names.
  mrs_string inObsName = stringSplit(ctrl_inObsNames_->to<mrs_string>(), ",")[0];
  inObsNames+="Average_" + inObsName + ",Minimum_" + inObsName + ",";

  // Add prefix to the observation names.
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "PeakRatio_"), NOUPDATE);

  maxima_.stretch(inSamples_);
  minima_.stretch(inSamples_);

}


void
PeakRatio::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;



  mrs_real max_ = -1.0 * DBL_MAX;
  mrs_real min_ = DBL_MAX;
  mrs_real avg_ = 0.0;


  for (t=0; t < inSamples_; t++)
  {
    for (o=0; o < inObservations_; o++)
    {
      out(o,t)=in(o,t); // copy everything from input to output

      if (in(o,t) > max_)
      {
        max_ = in(o,t);
      }
      if (in(o,t) < min_)
      {
        min_ = in(o,t);
      }
      avg_=avg_+in(o,t);

    }
    avg_=avg_/(inSamples_*inObservations_);
    maxima_(t)=max_;
    minima_(t)=min_;
  }
  mrs_real res1=0.0;
  mrs_real res2=0.0;

  for (t=0; t < inSamples_; t++)
  {
    //compute ratios
    if(minima_(t)!=0.0) res1=maxima_(t)/minima_(t);
    if(minima_(t)!=0.0) res2=maxima_(t)/avg_;

    //add the ratios to the output
    out(onObservations_-1,t)=res1;
    out(onObservations_-2,t)=res2;
  }
}
