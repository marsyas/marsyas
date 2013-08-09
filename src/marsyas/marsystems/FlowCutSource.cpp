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


#include "FlowCutSource.h"

using std::ostringstream;
using namespace Marsyas;

FlowCutSource::FlowCutSource(mrs_string name):MarSystem("FlowCutSource", name)
{

  addControls();
}

FlowCutSource::FlowCutSource(const FlowCutSource& a) : MarSystem(a)
{

}

FlowCutSource::~FlowCutSource()
{
}

MarSystem*
FlowCutSource::clone() const
{
  return new FlowCutSource(*this);
}

void
FlowCutSource::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/setSamples", 0);
  addctrl("mrs_natural/setObservations", 0);
  addctrl("mrs_real/setRate", 0.0);
  setctrlState("mrs_natural/setSamples", true);
  setctrlState("mrs_natural/setObservations", true);
  setctrlState("mrs_real/setRate", true);
}

void
FlowCutSource::myUpdate(MarControlPtr sender)
{
  MarSystem::myUpdate(sender);

  if(getctrl("mrs_natural/setSamples")->to<mrs_natural>())
    setctrl("mrs_natural/onSamples", getctrl("mrs_natural/setSamples")->to<mrs_natural>());
  if(getctrl("mrs_natural/setObservations")->to<mrs_natural>())
    setctrl("mrs_natural/onObservations", getctrl("mrs_natural/setObservations")->to<mrs_natural>());
  if(getctrl("mrs_real/setRate")->to<mrs_real>())
    setctrl("mrs_real/osrate", getctrl("mrs_real/setRate")->to<mrs_real>());
}


void
FlowCutSource::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  (void) in;
  for (o=0; o < onObservations_; o++)
  {
    for (t = 0; t < onSamples_; t++)
    {
      out(o,t) =0;
    }
  }
}








