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

#include "SilenceRemove.h"
#include "../common_source.h"

using namespace std;
using namespace Marsyas;

SilenceRemove::SilenceRemove(mrs_string name):MarSystem("SilenceRemove",name)
{
  isComposite_ = true;
  addControls();
}

SilenceRemove::SilenceRemove(const SilenceRemove& a): MarSystem(a)
{
  ctrl_threshold_ = getctrl("mrs_real/threshold");
}

SilenceRemove::~SilenceRemove()
{

}

MarSystem*
SilenceRemove::clone() const
{
  return new SilenceRemove(*this);
}

void
SilenceRemove::addControls()
{
  addctrl("mrs_real/threshold", 0.01, ctrl_threshold_);
  setctrlState("mrs_real/threshold", true);
}

void
SilenceRemove::myUpdate(MarControlPtr sender)
{
  MRSDIAG("SilenceRemove.cpp - SilenceRemove:myUpdate");

  threshold_ = ctrl_threshold_->to<mrs_real>();

  if (marsystems_.size())
  {
    //propagate in flow controls to first child
    marsystems_[0]->setctrl("mrs_natural/inObservations", inObservations_);
    marsystems_[0]->setctrl("mrs_natural/inSamples", inSamples_);
    marsystems_[0]->setctrl("mrs_real/israte", israte_);
    marsystems_[0]->setctrl("mrs_string/inObsNames", inObsNames_);
    marsystems_[0]->update();

    // forward flow propagation
    ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
    ctrl_onObservations_->setValue(ctrl_inObservations_, NOUPDATE);
    ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
    ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);

    //marsystems_[0]->update(); //lmartins: shouldn't this have already been called?! [?]

    if (ctrl_hasData_.isInvalid())
      ctrl_hasData_ = marsystems_[0]->getctrl("mrs_bool/hasData");
  }
  else //if composite is empty...
    MarSystem::myUpdate(sender);
}

void
SilenceRemove::myProcess(realvec& in, realvec& out)
{
  mrs_real rms = 0.0;
  mrs_natural count = 0;
  mrs_natural t,o;

  if(marsystems_.size())
  {
    do
    {
      marsystems_[0]->process(in, out);

      for (o=0; o < onObservations_; o++)
        for (t = 0; t < onSamples_; t++)
        {
          rms += (out(o,t) * out(o,t));
          count++;
        }
      rms /= count;
      rms = sqrt(rms);
      count = 0;
    } while (rms < threshold_ && (ctrl_hasData_->isTrue()));
  }
  else //composite has no children!
  {
    MRSWARN("SilenceRemove::process: composite has no children MarSystems - passing input to output without changes.");
    out = in;
  }
}
