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

#include "Fanin.h"


using std::ostringstream;
using std::vector;


using namespace Marsyas;

Fanin::Fanin(mrs_string name):MarSystem("Fanin", name)
{
  isComposite_ = true;
}

Fanin::Fanin(const Fanin & other):
  MarSystem(other)
{
  // do not copy slices
}

Fanin::~Fanin()
{
  deleteSlices();
}

void
Fanin::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter= slices_.begin(); iter != slices_.end(); iter++)
  {
    delete *(iter);
  }
  slices_.clear();
}

MarSystem*
Fanin::clone() const
{
  return new Fanin(*this);
}

void
Fanin::myUpdate(MarControlPtr sender)
{
  child_count_t child_count = marsystems_.size();
  if (child_count)
  {
    //propagate in flow controls to first child
    marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
    marsystems_[0]->setctrl("mrs_real/israte", israte_);
    //marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
    //marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
    marsystems_[0]->update(sender);

    ostringstream oss;
    oss << name_ <<"_mix_0, ";

    mrs_natural inObservations = marsystems_[0]->getctrl("mrs_natural/inObservations")->to<mrs_natural>();

    for (child_count_t i=1; i < child_count; ++i)
    {
      marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
      marsystems_[i]->setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte")); //[!] israte
      marsystems_[i]->update(sender);
      inObservations += marsystems_[i]->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
      oss << name_ << "_mix_" << i << ", ";
    }

    // forward flow propagation
    setctrl(ctrl_onSamples_, marsystems_[0]->getctrl("mrs_natural/onSamples"));
    setctrl(ctrl_onObservations_, 1);
    setctrl(ctrl_osrate_, marsystems_[0]->getctrl("mrs_real/osrate"));
    setctrl(ctrl_onObsNames_, oss.str());

    // update slices for child MarSystems
    if (slices_.size() < child_count)
      slices_.resize(child_count, NULL);

    for (child_count_t i=0; i< child_count; ++i)
    {
      if (slices_[i] != NULL)
      {
        if ((slices_[i])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>()  ||
            (slices_[i])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>())
        {
          delete slices_[i];
          slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                                   marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
        }
      }
      else
      {
        slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                                 marsystems_[i]->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
      }
      (slices_[i])->setval(0.0);
    }
  }
  else //if composite is empty...
    MarSystem::myUpdate(sender);

}

void
Fanin::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  child_count_t child_count = marsystems_.size();
  if(child_count)
  {
    out.setval(0.0);

    // Add assertions about sizes
    realvec ob(1,inSamples_);

    for (o=0; o < inObservations_; o++)
    {
      // process each observation
      for (t=0; t < inSamples_; t++)
        ob(0,t) = in(o,t);
      marsystems_[o]->process(ob, *(slices_[o]));
      for (t=0; t < onSamples_; t++)
        out(0,t) += (*(slices_[o]))(0,t);
    }
  }
  else //composite has no children!
  {
    MRSWARN("Fanin::process: composite has no children MarSystems - passing input to output without changes.");
    out = in;
  }
}
