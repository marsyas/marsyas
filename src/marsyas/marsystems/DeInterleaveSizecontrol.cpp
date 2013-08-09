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

#include "DeInterleaveSizecontrol.h"

using std::ostringstream;
using namespace Marsyas;

DeInterleaveSizecontrol::DeInterleaveSizecontrol(mrs_string name):MarSystem("DeInterleaveSizecontrol", name)
{
  //Add any specific controls needed by DeInterleaveSizecontrol
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
}

DeInterleaveSizecontrol::DeInterleaveSizecontrol(const DeInterleaveSizecontrol& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_numSets_ = getctrl("mrs_natural/numSets");
  ctrl_sizes_ = getctrl("mrs_realvec/sizes");
}

DeInterleaveSizecontrol::~DeInterleaveSizecontrol()
{
}

MarSystem*
DeInterleaveSizecontrol::clone() const
{
  return new DeInterleaveSizecontrol(*this);
}

void
DeInterleaveSizecontrol::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/numSets", 1, ctrl_numSets_);
  addctrl("mrs_realvec/sizes",realvec() , ctrl_sizes_);
}

void
DeInterleaveSizecontrol::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);
}


void
DeInterleaveSizecontrol::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  for (o=0; o < inSamples_; o++)
  {
    mrs_natural size=(mrs_natural)(ctrl_sizes_->to<mrs_realvec>()(o));
    if (size<=0) size=onSamples_;
    mrs_natural rest=size%ctrl_numSets_->to<mrs_natural>();
    mrs_natural part=size/ctrl_numSets_->to<mrs_natural>();
    mrs_natural count=0;
    for(int i=0; i<onObservations_/size; ++i)
    {
      for (t = 0; t < rest; t++)
      {
        for (mrs_natural n = 0; n <1+part ; n++)
        {
          //first deinterleave once to all rows for the first "rest" number
          //(which happen one time more often then the others)
          mrs_natural outindex=i*size+n+part*t;
          mrs_natural inindex=i*size+ctrl_numSets_->to<mrs_natural>()*n+t;
          out(outindex,o) = in(inindex,o);
          count++;
        }
      }
      for (t = rest; t < ctrl_numSets_->to<mrs_natural>(); t++)
      {
        for (mrs_natural n = 0; n < part; n++)
        {
          //apply deinterleave to all remaining rows
          mrs_natural outindex=i*size+n+rest+part*t;
          mrs_natural inindex=i*size+ctrl_numSets_->to<mrs_natural>()*n+t;
          out(outindex,o) = in(inindex,o);
          count++;
        }
      }
    }
  }
}








