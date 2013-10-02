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

#include "ResampleNearestNeighbour.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

ResampleNearestNeighbour::ResampleNearestNeighbour(mrs_string name):MarSystem("ResampleNearestNeighbour", name)
{
  addControls();
}

ResampleNearestNeighbour::ResampleNearestNeighbour(const ResampleNearestNeighbour& a) : MarSystem(a)
{
  ctrl_stretch_ = getctrl("mrs_real/stretch");
  ctrl_samplingRateAdjustmentMode_ = getctrl("mrs_bool/samplingRateAdjustmentMode");
}


ResampleNearestNeighbour::~ResampleNearestNeighbour()
{
}

MarSystem*
ResampleNearestNeighbour::clone() const
{
  return new ResampleNearestNeighbour(*this);
}

void
ResampleNearestNeighbour::addControls()
{
  addctrl("mrs_real/stretch", 1.0, ctrl_stretch_);
  addctrl("mrs_bool/samplingRateAdjustmentMode", (mrs_bool)true , ctrl_samplingRateAdjustmentMode_);

  setctrlState("mrs_real/stretch", true);
  setctrlState("mrs_bool/samplingRateAdjustmentMode",(mrs_bool)true);
}

void
ResampleNearestNeighbour::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
  MRSDIAG("ResampleNearestNeighbour.cpp - ResampleNearestNeighbour:myUpdate");
  mrs_real alpha = ctrl_stretch_->to<mrs_real>();
  ctrl_onSamples_->setValue((mrs_natural) (alpha * ctrl_inSamples_->to<mrs_natural>()), NOUPDATE);
  ctrl_onObservations_->setValue(ctrl_inObservations_->to<mrs_natural>());

  if (!(ctrl_samplingRateAdjustmentMode_->to<mrs_bool>()))
  {
    alpha=1.0;
  }

  ctrl_osrate_->setValue(ctrl_israte_->to<mrs_real>()*alpha);

  mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
  // Add prefix to the observation names.
  ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "ResampleNearestNeighbour_"), NOUPDATE);

}

void
ResampleNearestNeighbour::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  mrs_real tp;
  mrs_natural tl, tr;
  mrs_real alpha = ctrl_stretch_->to<mrs_real>();


  for (o=0; o < onObservations_; o++)
  {
    for (t = 0; t < onSamples_; t++)
    {
      tp = t / alpha;
      tl= (mrs_natural)tp;
      tr = tl + 1;
      if (tl<inSamples_)
      {

        out(o,t) = (tr-tp) * in(o,tl) + (tp-tl) * in(o,tr);
        if((tp-tl)>(tr-tp)) //on equality default to the leftmost value
        {
          out(o,t) =in(o,tr);
        }
        else
        {
          out(o,t) =in(o,tl);
        }
      }
      else // "reflect" on boundary
      {
        out(o,t) = in(o,inSamples_-1); //  alternative possibilities would include:

        //periodic on boundary:
        //out(o,t) = in(o,0);

        //zero pad on boundary:
        //out(o,t) == 0;
      }
    }
  }
}
