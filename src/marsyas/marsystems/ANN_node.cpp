/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.princeton.edu>
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

#include "ANN_node.h"
#include "../common_source.h"


using std::ostringstream;

using namespace Marsyas;

ANN_node::ANN_node(mrs_string name):MarSystem("ANN_node", name)
{
  //type_ = "ANN_node";
  //name_ = name;

  addControls();
}


ANN_node::~ANN_node()
{
}


MarSystem*
ANN_node::clone() const
{
  return new ANN_node(*this);
}

void
ANN_node::addControls()
{
  addctrl("mrs_realvec/weights", weights_);
  setctrlState("mrs_realvec/weights", true);
  addctrl("mrs_real/bias", bias_);
  setctrlState("mrs_real/bias", true);
}

void
ANN_node::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("ANN_node.cpp - ANN_node:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)1);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  //weights_.create(getctrl("mrs_realvec/weights")->to<mrs_realvec>().getSize());
  weights_ = getctrl("mrs_realvec/weights")->to<mrs_realvec>();
  bias_ = getctrl("mrs_real/bias")->to<mrs_real>();
}

void
ANN_node::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  //checkFlow(in,out);

  for (t = 0; t < inSamples_; t++)
  {
    out(0,t) = bias_; // initialize output to bias

    for (o=0; o < inObservations_; o++)
    {
      out(0,t) += weights_(o) * in(o,t); // calculate weighted sum
    }

  }
}












