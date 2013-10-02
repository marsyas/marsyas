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

#include "Product.h"
#include "../common_source.h"

#include <iostream>

using std::ostringstream;
using namespace Marsyas;

Product::Product(mrs_string name):MarSystem("Product",name)
{
  addControls();
}

void
Product::addControls()
{
  addctrl("mrs_realvec/mask", realvec(),ctrl_mask_);
  addctrl("mrs_bool/use_mask", false);
}

Product::~Product()
{
}

MarSystem*
Product::clone() const
{
  return new Product(*this);
}


void
Product::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("Product.cpp - Product:myUpdate");

  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue(ctrl_inObsNames_, NOUPDATE);
}


void
Product::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

  if (getctrl("mrs_bool/use_mask")==true) {
    MarControlAccessor acc(getctrl("mrs_realvec/mask"));
    mrs_realvec& mask = acc.to<mrs_realvec>();
    for (t = 0; t < inSamples_; t++)
      out(0,t) = mask(t);
  }
  else {
    for (t = 0; t < inSamples_; t++)
      out(0,t) = 1;
  }
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      out(0,t) *= in(o,t);
    }
}
