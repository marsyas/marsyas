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

#include "Deinterleave.h"

using std::ostringstream;
using namespace Marsyas;

Deinterleave::Deinterleave(mrs_string name):MarSystem("Deinterleave", name)
{

  pat_ = new PatchMatrix("pat");

  outindex_.stretch(inObservations_);
  inindex_.stretch(inObservations_);
  weights_.stretch(inObservations_,inObservations_);

  addControls();
}

Deinterleave::Deinterleave(const Deinterleave& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_numSets_ = getctrl("mrs_natural/numSets");

  pat_ = new PatchMatrix("pat");

  outindex_.stretch(inObservations_);
  inindex_.stretch(inObservations_);
  weights_.stretch(inObservations_,inObservations_);
}

Deinterleave::~Deinterleave()
{
  delete pat_;
}

MarSystem*
Deinterleave::clone() const
{
  return new Deinterleave(*this);
}

void
Deinterleave::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/numSets", 2, ctrl_numSets_);
  //setControlState("mrs_realvec/consts",true);
  setControlState("mrs_natural/numSets",true);
}

void
Deinterleave::myUpdate(MarControlPtr sender)
{

  MarSystem::myUpdate(sender);

  mrs_natural numSets = ctrl_numSets_->to<mrs_natural>();

  outindex_.stretch(inObservations_);
  inindex_.stretch(inObservations_);
  weights_.stretch(inObservations_,inObservations_);


  mrs_natural rest=inObservations_%numSets;
  mrs_natural part=inObservations_/numSets;
  mrs_natural count=0;

  for (mrs_natural t = 0; t < rest; t++)
  {
    for (mrs_natural n = 0; n <1+part ; n++)
    {
      //get Reordering indices for all rows for which there is a rest (they get one output more)
      outindex_(count)=n+part*t;
      inindex_(count)=numSets*n+t;
      count++;
    }
  }
  for (mrs_natural t = rest; t < numSets; t++)
  {
    for (mrs_natural n = 0; n < part; n++)
    {
      //get Reordering indices for all remaining rows
      outindex_(count)=n+rest+part*t;
      inindex_(count)=numSets*n+t;
      count++;
    }
  }

  //Weights deinterleave n
  for(mrs_natural i=0; i<inindex_.getSize(); ++i)
  {
    weights_((mrs_natural)(outindex_(i)),(mrs_natural)(inindex_(i)))=1.0;
  }

  pat_->setctrl("mrs_realvec/weights",weights_);


}


void
Deinterleave::myProcess(realvec& in, realvec& out)
{
  //Weights deinterleave n

  pat_->process(in,out);

}








