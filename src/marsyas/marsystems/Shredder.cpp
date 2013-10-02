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



#include "Shredder.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

Shredder::Shredder(mrs_string name):MarSystem("Shredder", name)
{
  isComposite_ = true;
  addControls();
  nTimes_ = 5;
}

Shredder::Shredder(const Shredder& a) : MarSystem(a)
{
  ctrl_nTimes_ = getctrl("mrs_natural/nTimes");
  ctrl_accumulate_ = getctrl("mrs_bool/accumulate");
  nTimes_ = a.nTimes_;
}

Shredder::~Shredder()
{
}

MarSystem*
Shredder::clone() const
{
  return new Shredder(*this);
}

void
Shredder::addControls()
{
  addctrl("mrs_natural/nTimes", 5, ctrl_nTimes_);
  setctrlState("mrs_natural/nTimes", true);

  addctrl("mrs_bool/accumulate", false, ctrl_accumulate_);
  ctrl_accumulate_->setState(true);
}

void
Shredder::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Shredder.cpp - Shredder:myUpdate");

  nTimes_ = ctrl_nTimes_->to<mrs_natural>();

  // update dataflow component MarSystems in order
  if (marsystems_.size())
  {
    //propagate in flow controls to first child
    marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
    marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_ / nTimes_);
    marsystems_[0]->setctrl("mrs_real/israte", israte_);
    marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
    marsystems_[0]->update();

    childOnSamples_ = marsystems_[0]->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

    // forward flow propagation
    if(ctrl_accumulate_->isTrue())
    {
      setctrl("mrs_natural/onSamples", childOnSamples_*nTimes_);
    }
    else
    {
      setctrl("mrs_natural/onSamples", childOnSamples_);
    }
    setctrl("mrs_natural/onObservations",
            marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
    setctrl("mrs_real/osrate",
            marsystems_[0]->getctrl("mrs_real/osrate"));
    setctrl("mrs_string/onObsNames",
            marsystems_[0]->getctrl("mrs_string/onObsNames"));

    childIn_.create(marsystems_[0]->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                    marsystems_[0]->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

    if(ctrl_accumulate_->isTrue())
      childOut_.create(marsystems_[0]->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                       childOnSamples_);
    else
      childOut_.create(0,0); //just to save memory...
  }
  else //if composite is empty...
    MarSystem::myUpdate(sender);
}

void
Shredder::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o,c;
  if(marsystems_.size())
  {
    for (c = 0; c < nTimes_; ++c)
    {
      //shred input
      for (o=0; o < inObservations_; o++)
        for (t = 0; t < inSamples_/nTimes_; t++)
        {
          childIn_(o,t) = in(o, t + c * (inSamples_/nTimes_)) ;
        }

      if(ctrl_accumulate_->isTrue()) //accumulate child output
      {
        marsystems_[0]->process(childIn_, childOut_);

        for (o=0; o < onObservations_; o++)
          for (t = 0; t < childOnSamples_; t++)
          {
            out(o, t + c * childOnSamples_) = childOut_(o,t);
          }
      }
      else //no child output accumulation
      {
        marsystems_[0]->process(childIn_, out);
      }
    }
  }
  else //composite has no children!
  {
    MRSWARN("Shredder::process: composite has no children MarSystems - passing input to output without changes.");
    out = in;
  }

  /*
  MATLAB_PUT(in, "Schredder_in");
  MATLAB_PUT(out, "Schredder_out");
  MATLAB_EVAL("figure(1);imagesc(Schredder_in(1:2:end, :))");
  MATLAB_EVAL("figure(2);plot(Schredder_out)");
  */
}
