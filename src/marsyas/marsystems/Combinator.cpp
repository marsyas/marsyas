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

#include "Combinator.h"
#include "../common_source.h"
#include <algorithm>

using std::ostringstream;
using std::min;
using std::max;

using namespace Marsyas;

const mrs_string Combinator::combinatorStrings[kNumCombinators] =
{
  "+",
  "*",
  "max",
  "min"
};

Combinator::Combinators_t Combinator::GetCombinatorIdx (const mrs_string ctrlString)
{
  Combinators_t ret = kAdd;	// default

  for (mrs_natural i = 0; i < kNumCombinators; i++)
  {
    if (ctrlString == combinatorStrings[i])
      return (Combinators_t)i;
  }

  return ret;
}

Combinator::Combinator(mrs_string name) : MarSystem("Combinator", name)
{
  /// Add any specific controls needed by this MarSystem.
  // Default controls that all MarSystems should have (like "inSamples"
  // and "onObservations"), are already added by MarSystem::addControl(),
  // which is already called by the constructor MarSystem::MarSystem(name).
  // If no specific controls are needed by a MarSystem there is no need to
  // implement and call this addControl() method (see for e.g. Rms.cpp)
  addControls();
}

Combinator::Combinator(const Combinator& a) : MarSystem(a)
{
  // IMPORTANT!
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  // Otherwise this would result in trying to deallocate them twice!
  ctrl_combinator_ = getctrl("mrs_string/combinator");
  ctrl_weights_ = getctrl("mrs_realvec/weights");
  ctrl_numInputs_ = getctrl("mrs_natural/numInputs");
}


Combinator::~Combinator()
{
}

MarSystem*
Combinator::clone() const
{
  // Every MarSystem should do this.
  return new Combinator(*this);
}

void
Combinator::addControls()
{
  mrs_realvec tmp(1);
  tmp(0)	= 1;

  addctrl("mrs_string/combinator", "+", ctrl_combinator_);
  addctrl("mrs_realvec/weights", tmp, ctrl_weights_);
  addctrl("mrs_natural/numInputs", 1, ctrl_numInputs_);

  setctrlState("mrs_natural/numInputs", true);
}

void
Combinator::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Combinator.cpp - Combinator:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  //if (ctrl_inObservations_->to<mrs_natural>() % ctrl_numInputs_->to<mrs_natural>())
  //	MRSWARN("Combinator: dimension mismatch");
  if (ctrl_numInputs_->to<mrs_natural>())
    updControl ("mrs_natural/onObservations", ctrl_inObservations_->to<mrs_natural>() / ctrl_numInputs_->to<mrs_natural>());
  else
    updControl ("mrs_natural/onObservations", 0);

}

void
Combinator::myProcess(realvec& in, realvec& out)
{
  mrs_natural k,i,j;
  mrs_real weight		= 1;
  mrs_bool useWeights = false;

  const Combinators_t		combi		= GetCombinatorIdx (ctrl_combinator_->to<mrs_string>());
  const mrs_natural		numInputs	= ctrl_numInputs_->to<mrs_natural>();

  // initialize output buffer
  if(combi == kAdd)
    out.setval(0); //identity operator
  if(combi == kMult)
    out.setval(1); //identity operator
  if(combi == kMax)
    out.setval(-1.0*MAXREAL);
  if(combi == kMin)
    out.setval(MAXREAL);

  // sanity check
  if ((inObservations_%numInputs) != 0)
  {
    MRSWARN("Combinator: dimension mismatch");
    MRSASSERT(false);
    out.setval(0);
    return;
  }

  if (ctrl_weights_->to<mrs_realvec>().getSize () == numInputs)
    useWeights	= true;

  /// Iterate over the observations and samples and do the combination.
  for (k = 0; k < numInputs; k++)
  {
    weight	= (useWeights) ? (ctrl_weights_->to<mrs_realvec>())(k) : weight;

    for (i = 0; i < onObservations_; i++)
    {
      // do the col-loop inside the switch to avoid calling the switch for every entry
      // we might also do this with function pointers...
      switch (combi)
      {
      case kAdd:
      default:
      {
        for (j = 0; j < onSamples_; j++)
          out(i,j)	+= weight * in(i+k*onObservations_,j);
        break;
      }
      case kMult:
      {
        if (weight == 1.0)   // avoid pow if possible
        {
          for (j = 0; j < onSamples_; j++)
            out(i,j)	*=  in(i+k*onObservations_,j);
        }
        else
        {
          for (j = 0; j < onSamples_; j++)
            out(i,j)	*=  pow (in(i+k*onObservations_,j), weight);
        }
        break;
      }
      case kMax:
      {
        for (j = 0; j < onSamples_; j++)
          out(i,j)	=  max (out(i,j),in(i+k*onObservations_,j));
        break;
      }
      case kMin:
      {
        for (j = 0; j < onSamples_; j++)
          out(i,j)	=  min (out(i,j),in(i+k*onObservations_,j));
        break;
      }
      }
    }
  }
}
