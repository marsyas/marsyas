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

#include "../common_source.h"
#include "OrcaSnip.h"

#define MTLB_DBG_LOG

using std::ostringstream;
using namespace Marsyas;

OrcaSnip::OrcaSnip(mrs_string name):MarSystem("OrcaSnip", name)
{
  //Add any specific controls needed by OrcaSnip
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
}

OrcaSnip::OrcaSnip(const OrcaSnip& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_startSnip_ = getctrl("mrs_natural/startSnip");
  ctrl_stopSnip_ = getctrl("mrs_natural/stopSnip");
  ctrl_decisionThresh_ = getctrl("mrs_real/decisionThresh");
}

OrcaSnip::~OrcaSnip()
{
}

MarSystem*
OrcaSnip::clone() const
{
  return new OrcaSnip(*this);
}

void
OrcaSnip::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_natural/startSnip", -1, ctrl_startSnip_);
  addctrl("mrs_natural/stopSnip", -1, ctrl_stopSnip_);
  addctrl("mrs_real/decisionThresh", .4, ctrl_decisionThresh_);
}

void
OrcaSnip::myUpdate(MarControlPtr sender)
{
  // no need to do anything OrcaSnip-specific in myUpdate
  MarSystem::myUpdate(sender);

}


void
OrcaSnip::myProcess(realvec& in, realvec& out)
{
  mrs_real decisionThresh = ctrl_decisionThresh_->to<mrs_real>();
  mrs_natural startStopIdx[2] = {inSamples_-1,0};

  out = in;

  // normalize rms
  mrs_real tmpMax = 0;
  for (mrs_natural t = 0; t < inSamples_; t++)
  {
    mrs_real tmp = in(1,t);
    if (tmp > tmpMax)
      tmpMax = tmp;
  }
  for (mrs_natural t = 0; t < inSamples_; t++)
    out(1,t)	/= tmpMax;


  while ((startStopIdx[0] >= (inSamples_-1)) && (decisionThresh > .01))
  {
    // set start Idx
    for (mrs_natural t = 0; t < inSamples_; t++)
    {
      mrs_real avg = .5*(out(0,t) + out(1,t));
      if (avg > decisionThresh)
      {
        startStopIdx[0]	= t;
        break;
      }
    }

    // set stop Idx
    for (mrs_natural t = inSamples_-1; t >= 0; t--)
    {
      mrs_real avg = .5*(out(0,t) + out(1,t));
      if (avg > decisionThresh)
      {
        startStopIdx[1]	= t;
        break;
      }
    }
    // lower threshold until we find something
    decisionThresh	*= .9;
  }

  if (startStopIdx[0] == inSamples_-1)
    startStopIdx[0]	= 0;
  if (startStopIdx[1] == 0)
    startStopIdx[1]	= inSamples_-1;

  ctrl_startSnip_->setValue (startStopIdx[0], false);
  ctrl_stopSnip_->setValue (startStopIdx[1], false);
#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(startStopIdx[0], "iStart");
  MATLAB_PUT(startStopIdx[1], "iStop");
  MATLAB_PUT(out, "output");
  MATLAB_EVAL ("figure(513),plot([output; mean(output,1)]'), hold on, stem([iStart iStop], [.9 .9],'r','fill'),axis([0 size(output,2) 0 1]), grid on,hold off");
#endif
#endif
}








