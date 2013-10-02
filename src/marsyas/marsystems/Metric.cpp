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

#include "Metric.h"
#include <marsyas/NumericLib.h>

using std::ostringstream;
using namespace Marsyas;

static mrs_real
randomDistance(const realvec& Vi, const realvec& Vj, const realvec& covMatrix)
{
  (void) Vi;
  (void) Vj;
  (void) covMatrix;
  return rand()/mrs_real(RAND_MAX);
}

Metric::Metric(mrs_string name):MarSystem("Metric", name)
{
  addControls();
}

Metric::Metric(const Metric& a) : MarSystem(a)
{
  ctrl_metric_ = getctrl("mrs_string/metric");
  ctrl_covMatrix_ = getctrl("mrs_realvec/covMatrix");
}

Metric::~Metric()
{

}

MarSystem*
Metric::clone() const
{
  return new Metric(*this);
}

void
Metric::addControls()
{
  addControl("mrs_string/metric", "euclideanDistance", ctrl_metric_);
  ctrl_metric_->setState(true);

  addControl("mrs_realvec/covMatrix", realvec(), ctrl_covMatrix_);
}

void
Metric::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  if(inSamples_ > 1) {
    MRSWARN("Metric::myUpdate - inSamples > 1 : only first column will be processed!");
  }

  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_onSamples_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE); //[?]
  ctrl_onObsNames_->setValue("metric", NOUPDATE);

  //the input has the two vectors/matrices to process stacked vertically
  if(inObservations_ % 2 != 0) {
    MRSWARN("Metric::myUpdate - input flow controls do not seem to be in a valid format!");
  }
  vec_i_.create(ctrl_inObservations_->to<mrs_natural>()/2, ctrl_inSamples_->to<mrs_natural>());
  vec_j_.create(ctrl_inObservations_->to<mrs_natural>()/2, ctrl_inSamples_->to<mrs_natural>());

  //get the pointer for the correct metric function
  mrs_string metricName = ctrl_metric_->to<mrs_string>();
  if(metricName == "euclideanDistance")
  {
    metricFunc_ = &NumericLib::euclideanDistance;
  }
  else if(metricName == "mahalanobisDistance")
  {
    metricFunc_ = &NumericLib::mahalanobisDistance;
  }
  else if(metricName == "cosineDistance")
  {
    metricFunc_ = &NumericLib::cosineDistance;
  }
  else if(metricName == "randomDistance")
  {
    metricFunc_ = &randomDistance;
  }
  else
  {
    MRSWARN("Metric::myUpdate: unsuported metric funtion: " + metricName);
    metricFunc_ = NULL;
  }
}

void
Metric::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  if(metricFunc_)
  {
    //get the two stacked vectors from the input
    for(o=0; o < inObservations_/2; ++o)
    {
      for(t=0; t < inSamples_; ++t)
      {
        vec_i_(o,t) = in(o,t);
        vec_j_(o,t) = in(o+inObservations_/2,t);
      }
    }

    //do the actual metric calculation
    out(0) = metricFunc_(vec_i_, vec_j_, ctrl_covMatrix_->to<mrs_realvec>());
  }
  else
    out(0) = 0; //default output value when no metric is defined
}
